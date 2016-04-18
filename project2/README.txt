Names: Ethan Morisette, Kaleb Luse
Files: myKitchen.c, Makefile
	> myKitchen.c - contains program used to run the virtual kitchen
	> Makefile - converts the c file into a kernel object file

Instructions for compile: issue "make" at command line inside directory containing above files
Instructions for running: use echo to write to "/proc/kitchen" and cat to read from "/proc/kitchen"
	> echo -1 > /proc/kitchen to stop kitchen
	> echo 0 > /proc/kitchen to start kitchen
	> echo 1 > /proc/kitchen to add salad to kitchen queue
	> echo 2 > /proc/kitchen to add hamburger to kitchen queue
	> echo 3 > /proc/kitchen to add pizza to kitchen queue
	> echo 4 > /proc/kitchen to add beef wellington to kitchen queue

Challenges: had some issues with the tear down function of the proc file (probably related to indefinitely blocking on the kthread_stop function in the teardown funciton)


