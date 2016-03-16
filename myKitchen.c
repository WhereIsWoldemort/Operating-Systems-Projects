#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm-generic/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");  /* Kernel needs this license. */

#define ENTRY_NAME "kitchen"
#define PERMS 0644
#define PARENT NULL

#define QUEUE_SIZE 20

#define STOP_KITCHEN -1
#define START_KITCHEN 0
#define CAESAR_SALAD 1
#define HAMBURGER 2
#define PERSONAL_PIZZA 3
#define BEEF_WELLINGTON 4

#define ACCESS_PENALTY 1
#define SALAD_PROCESS_TIME 2
#define HAMBURGER_PROCESS_TIME 3
#define PIZZA_PROCESS_TIME 4
#define BEEFWELL_PROCESS_TIME 8

///////////////////////////////////////////////////////////////////////////////////////////////////
// type definitions ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	int queue[QUEUE_SIZE];			// a queue of menu items being processed 
	int currentSize; 			// contains the current number of elements in the queue
} fifoQueue;

///////////////////////////////////////////////////////////////////////////////////////////////////
// function declarations //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t procfile_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos);

ssize_t procfile_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos);

static int t_kitchen(void* data);
void addItem(int menuItem);
int removeItem(void);
void processItem(void);
void checkArray(void); //for testing only

///////////////////////////////////////////////////////////////////////////////////////////////////
// global variables ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
static struct file_operations hello_proc_ops = {
   .owner = THIS_MODULE,
   .read = procfile_read,
   .write = procfile_write,
};
struct mutex queueMutex;
struct task_struct *t; /* Printer thread task struct (use to start/stop)*/
fifoQueue kitchenQueue;

/* The kitchen thread will run this function.  The thread will stop
 * when either kitchen_stop(t) is called or else the function ends. */
static int t_kitchen(void* data) {
   int i=0;

   /* Print hello 50 times, then stop on it's own */
   while(!kthread_should_stop()) {
		processItem();	
      /* Sleep one second */
      ssleep(1);
	  printk("The currentSize = %d\n", kitchenQueue.currentSize);
	  checkArray();
	  printk("\n");    
      i++;
	  
    }

   return 0;
}


int hello_proc_init(void) {

   proc_create_data(ENTRY_NAME, 0, NULL, &hello_proc_ops, NULL);
   mutex_init(&queueMutex);   
   kitchenQueue.currentSize = 0;

   /* This message will print in /var/log/syslog or on the first tty. */
   printk("/proc/%s created\n", ENTRY_NAME);

   /* Start the printer -- move out of here later. */
   t = kthread_run(t_kitchen, NULL, "kitchen_thread");


   return 0;
}

ssize_t procfile_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

   static int finished = 0;
   int ret;
   char ret_buf[80];

   /* Are we done reading? If so, we return 0 to indicate end-of-file */
   if (finished) {
	finished=0;
	return 0;
   }

   finished = 1;

   /* This message will print in /var/log/syslog or on the first tty. */
   printk("/proc/%s read called.\n", ENTRY_NAME);

   ret=sprintf(ret_buf, "Kitchen read called.\n");
   if(copy_to_user(buf, ret_buf, ret)) {
      ret = -EFAULT;  
   }

   return ret;
}

ssize_t procfile_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    char *page; 
    int my_data = 0;
    
    // 1. allocate kernal memory (no touch!)
    page = (char *) vmalloc(count);
    if (!page)
       return -ENOMEM;   

    // 2. Copy the data from the user space.  Data is placed in page. 
    if (copy_from_user(page, buf, count)) {
       vfree(page);
       return -EFAULT;
    }

	// 3. we will want to check user input so they can't enter in "echo pie > /proc/kitchen"

    // 4. copy the kernel page into a variable we will use 
    sscanf(page,"%d",&my_data);

	// 5. check if the kitchen is full
	if ((kitchenQueue.currentSize >= QUEUE_SIZE) && (my_data != STOP_KITCHEN) && (my_data != START_KITCHEN)) {
		return -ENOMEM;
	}

	// 6. determine user input	
	switch(my_data) {
		case STOP_KITCHEN:
			printk("Kitchen stopped.\n");
			break;
		case START_KITCHEN:
			printk("Kitched Started.\n");
			break;
		case CAESAR_SALAD:
			printk("Salad added to the queue.\n");
			addItem(CAESAR_SALAD);
			break;
		case HAMBURGER:
			printk("Hamburger added to the queue.\n");
			addItem(HAMBURGER);
			break;
		case PERSONAL_PIZZA:
			printk("Pizza added to the queue\n");
			addItem(PERSONAL_PIZZA);
			break;
		case BEEF_WELLINGTON:
			printk("Beef wellington added to the queue\n");
			addItem(BEEF_WELLINGTON);
			break;
		default:
			printk("WHAT THE HELL ARE YOU DOING?!.txt\n");
			break;
	}

	printk("User has sent the value of %d\n", my_data);
    /* Free the allocated memory, don't touch. */
    vfree(page); 

    return count;
}

void hello_proc_exit(void)
{

   /* Will block here and wait until kthread stops */
   kthread_stop(t);
	
  
   remove_proc_entry(ENTRY_NAME, NULL);
   printk("Removing /proc/%s.\n", ENTRY_NAME);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Name: addItem(int menuItem)																	//
// In: an integer (representing a menu item)													//
// Out: none																					//
// Description: adds an item to kitchenQueue and updates all relevant information in the struct //
//////////////////////////////////////////////////////////////////////////////////////////////////
void addItem(int menuItem) {
	mutex_lock(&queueMutex);
	//printk("I have locked to add an item!\n");
	// 1. add the item to the queue
	kitchenQueue.queue[kitchenQueue.currentSize] = menuItem;	
	// 2. update currentSize variable
	kitchenQueue.currentSize++;
	mutex_unlock(&queueMutex);
	//printk("I unlocked after adding an item!\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Name: removeItem(int menuItem)																		//
// In: none																								//
// Out: integer (menuItem																				//
// Description: removes an item from kitchenQueue and updates all relevant information in the struct	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int removeItem() {
	int menuItem;	// menu item to be returned
	int i; 			// looping variable

	mutex_lock(&queueMutex);
	//printk("I locked to remove an item!\n");
	// 1. remove the item from the queue
	menuItem = kitchenQueue.queue[0];

	// 2. update the currentSize variable
	kitchenQueue.currentSize--;	
	
	// 3. shift queue items to the left by 1
	for (i = 1; i <= kitchenQueue.currentSize; i++) {
		kitchenQueue.queue[i - 1] = kitchenQueue.queue[i];
	}	
	mutex_unlock(&queueMutex);
	//printk("I unlocked after removing an item!\n");	

	// 4. return menuItem
	return menuItem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Name: processItem()																						  //
// In: none																									  //
// Out: none																								  //
// Description: takes care of processing the item by removing and waiting for the amount of time it takes  	  //
// Notes: currently the sleep time is contained outside of a mutex, not sure if we should have it that way... //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void processItem() {
	int menuItem; // integer value of item being processed

	//printk("Trying to process an item\n");	
	// 1. remove first item from queue
	if (kitchenQueue.currentSize > 0) {
		menuItem = removeItem();
	}

	// 2. determine the time it takes to process the food and sleep for that time
	switch(menuItem) {
		case CAESAR_SALAD:
			printk("Processing salad... takes %d seconds\n", SALAD_PROCESS_TIME);
			ssleep(SALAD_PROCESS_TIME);
			break;
		case HAMBURGER:
			printk("Processing hamburger... takes %d seconds\n", HAMBURGER_PROCESS_TIME);
			ssleep(HAMBURGER_PROCESS_TIME);
			break;
		case PERSONAL_PIZZA:
			printk("Processing pizza... takes %d seconds\n", PIZZA_PROCESS_TIME);
			ssleep(PIZZA_PROCESS_TIME);
			break;
		case BEEF_WELLINGTON:
			printk("Processing beef wellington... takes %d seconds\n", BEEFWELL_PROCESS_TIME);
			ssleep(BEEFWELL_PROCESS_TIME);
			break;
		default:
			printk("Nothing was in the queue!\n");	
			break;
	}
}

// function for testing
void checkArray() {
	int i;
	
	for(i = 0; i < kitchenQueue.currentSize; i++) {
		printk("Array at index %d: %d\n", i, kitchenQueue.queue[i]);
	}
}

module_init(hello_proc_init);
module_exit(hello_proc_exit);

