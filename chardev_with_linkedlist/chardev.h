#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kmod.h>
#include <linux/limits.h>
#include <asm/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>


/* Function Prototypes */
/* TODO: Add comments for each prototype */
int init_module(void); 
void cleanup_module(void);

MODULE_LICENSE("GPL");

/*Exported functions*/
extern void *sys_wr_hook;
extern int call_usermodehelper(char *path, char *argv[], char *envp[], int wait);

static int device_open(struct inode *, struct file *); 
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char*, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char*, size_t, loff_t *);
static void freezer_hook(unsigned int fd, struct file* file, const char __user *buf, size_t count);
static int umh_runner( char *argument, char* filename );

#define DEVICE_NAME "chardev" /* entry in /proc/devices */	
#define BUF_LEN (80)
#define BUFF (400)

