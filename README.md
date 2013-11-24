###Freezing the Linux Filesystem Using Kernel Modules 
####CentOS5 Linux V. 2.6.18
---
A system to "freeze" or make changes to a hard coded file path non persistent. 

The main component of the system is a hook into the sysw_write call that calls a function using a function pointer and blocks, setting up a wait queue, for all other sys calls using write.

From there, a user-helper thread is woken up and gets the information from the kernel to log any changes made to a file before it is rolled back.

The exact kernel being used can be found from the main [kernel.org linux v2.6 index](https://www.kernel.org/pub/linux/kernel/v2.6/).
---
###Which Files to use
The main file that will be used for the module is chardev_with_*. All others will be moved to a "practice" or "test" file.