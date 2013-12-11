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
/*around ln 360 above the sys_write call*/
void (*sys_wr_hook)(unsigned int fd,struct file* file, const char __user * buf, size_t count)=NULL;
EXPORT_SYMBOL(sys_wr_hook);

...

/*inside the sys_write system call*/
file = fget_light(fd, &fput_needed);
/*this next line should be added*/
if(sys_wr_hook) sys_wr_hook(fd, file, buf, count);

if(file) {
	...

```
###USAGE
Other than what is mentioned in the dependencies section, here is how you can get started with the module.  

Hardcoded filenames and paths will need to be changed in the chardev.c and callee.c files. These declarations are defined constants in the top section of each file.  
   
The make file compiles your user space code then builds and installs your kernel module without any arguments. So you only need to call ```make``` and ```make clean```.  
   
To test if the module works, run ```make``` then open one of the files in "freezer/*" (like test.txt for example) and write to the file then save. Once you save you can run ```make clean``` to uninstall the module which should restore your modified file back to its previous state before you wrote to it.   

>so a full test may look like:   
```  
make   
emacs freezer/test.txt   
```  
this is where you will make changes to the file   
```  
make clean   
```  

and *voila*. Your file is restored. 