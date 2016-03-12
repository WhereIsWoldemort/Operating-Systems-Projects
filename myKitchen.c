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

///////////////////////////////////////////////////////////////////////////////////////////////////
// type definitions ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	int queue[QUEUE_SIZE];			// a queue of menu items being processed 
	int currentSize = 0; 			// contains the current number of elements in the queue
} specialQueue;

///////////////////////////////////////////////////////////////////////////////////////////////////
// function declarations //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t procfile_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos);

ssize_t procfile_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos);

static int t_kitchen(void* data);
void addItem(int menuItem);

///////////////////////////////////////////////////////////////////////////////////////////////////
// global variables ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
static struct file_operations hello_proc_ops = {
   .owner = THIS_MODULE,
   .read = procfile_read,
   .write = procfile_write,
};
struct mutex myMutex;
struct task_struct *t; /* Printer thread task struct (use to start/stop)*/
specialQueue kitchenQueue;

/* The kitchen thread will run this function.  The thread will stop
 * when either kitchen_stop(t) is called or else the function ends. */
static int t_kitchen(void* data) {
   int i=0;

   /* Print hello 50 times, then stop on it's own */
   while(!kthread_should_stop()) {
      printk("Hello from the kitchen!  Run %i\n", i);

      /* Sleep one second */
      ssleep(1);
      i++;
    }

   return 0;
}


int hello_proc_init(void) {

   proc_create_data(ENTRY_NAME, 0, NULL, &hello_proc_ops, NULL);
   mutex_init(&myMutex);   

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

ssize_t procfile_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{

    char *page; /* don't touch */
    int my_data = 0;
    
    /* Allocating kernel memory, don't touch. */
    page = (char *) vmalloc(count);
    if (!page)
       return -ENOMEM;   

    /* Copy the data from the user space.  Data is placed in page. */ 
    if (copy_from_user(page, buf, count)) {
       vfree(page);
       return -EFAULT;
    }

   /* Now do something with the data, here we just print it */
    sscanf(page,"%d",&my_data);

	// determine user input	
	switch(my_data) {
		case STOP_KITCHEN:
			printk("I STARTED THE KITCHEN!\n");
			break;
		case START_KITCHEN:
			printk("I STOPPED THE KITCHEN!\n");
			break;
		case CAESAR_SALAD:
			printk("SALAADDD.jpg\n");
			break;
		case HAMBURGER:
			printk("HAMBURGER.jpg\n");
			break;
		case PERSONAL_PIZZA:
			printk("PIZZA.txt\n");
			break;
		case BEEF_WELLINGTON:
			printk("BEEF_WASHINGTON.jpg\n");
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
	// 1. add the item to the queue
	kitchenQueue.queue[kitchenQueue.currentSize] = menuItem;	
	// 2. update currentSize variable
	kitchenQueue.currentSize++;
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

	// 1. remove the item from the queue
	menuItem = kitchenQueue.queue[0];

	// 2. update the currentSize variable
	kitchenQueue.currentSize--;	

	// 3. shift queue items to the left by 1
	for (i = 1; i++; i <= kitchenQueue.currentSize) {
		kitchenQueue.queue[i - 1] = kitchenQueue[i];
	}	

	// 4. return menuItem
	return menuItem;
}

module_init(hello_proc_init);
module_exit(hello_proc_exit);

