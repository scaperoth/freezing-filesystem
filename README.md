###Freezing the Linux Filesystem Using Kernel Modules 
####CentOS5 Linux V. 2.6.18
---
A system to "freeze" or make changes to a hard coded file path non persistent. 

The main component of the system is a hook into the sys_write call that calls a function using a function pointer and blocks, setting up a wait queue, for all other sys calls using write.

From there, a user-helper thread is woken up and gets the information from the kernel to log any changes made to a file before it is rolled back.

The exact kernel being used can be found from the main [kernel.org linux v2.6 index](https://www.kernel.org/pub/linux/kernel/v2.6/).

###Dependecies (other than OS version)
This project requires the fs/read_write.c kernel file to be manipulated adding a function pointer in the sys_write function.

```
void (*sys_wr_hook)(unsigned int fd,struct file* file, const char __user * buf, size_t count)=NULL;
EXPORT_SYMBOL(sys_wr_hook);

...

//inside the sys_write system call
if(sys_wr_hook) sys_wr_hook(fd, file, buf, count);
...

```
