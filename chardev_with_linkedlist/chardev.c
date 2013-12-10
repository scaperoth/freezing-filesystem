#include "chardev.h"
#include "list.h"
/* Globals localized to file (by use of static */
static int Major;       /* assigned to device driver */
static char msg[BUF_LEN];   /* a stored message */
static struct linked_list *ll;

/*constant (hardcoded) path names for freezing system*/
const char *FREEZERPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/freezer";
const char *SNAPSHOTPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/snapshot";

static struct file_operations fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

DECLARE_WAIT_QUEUE_HEAD(wq);

/*condition variable for wait queue*/
static int flag = 0;

/**
 * This function makes use of the call_usermodehelper function wakes up a user process and executes it.
 * This function has created so that it can be used in various ways with the "callee" user code.
 * @param  argument the argument that should be passed to the user process in argv[1]
 * @param  filename the argument for argv[2], is used in the logging of which file is being manipulated
 */
static int umh_runner(char *argument, char *filename )
{
    char **argv = kmalloc(sizeof(char *[3]), GFP_KERNEL);

    if (!argv)
    {
        printk("Userspace args failed.\n");
        return 0;
    }

    char filepath[BUFF] = "";

    if (filename != NULL)
    {
        strncat(filepath, SNAPSHOTPATH, 200);
        strncat(filepath, "/", 1);
        strncat(filepath, filename, 200);
    }

    int ret = 0;

    argv[0] = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/callee";
    argv[1] = argument;
    argv[2] = filepath;
    char *envp[] = {"HOME=/", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL  };
    printk("usermodehelper: init\n");
    ret = call_usermodehelper(argv[0], argv, envp, 1);

    if (ret != 0)
        printk("error in call to usermodehelper: %d\n", ret);
    else
        printk("everything all right\n");

    return 0;
}

/**
 * Function that hooks into system calls. This function will get the filename from the 
 * passed down arguments and compare it with our "frozen" folder.
 * @param fd    file descriptor of the file being written to
 * @param file  struct file of file being written to
 * @param buf   address of what is being written
 * @param count max # of bytes to write
 */
static void freezer_hook(unsigned int fd, struct file *file, const char __user *buf, size_t count)
{

    //printk("hooked write\n");
    
    int buff_length = PATH_MAX + 11;
    char *p, *path;
    struct dentry *dentry = file->f_dentry->d_parent;   /*get parent dentry struct*/
    struct vfsmount *vfsmnt = file->f_vfsmnt;       /*vfs subtree used for d_path()*/

    path = kmalloc(buff_length, GFP_KERNEL);    /*allocate memory for path and test*/

    if (!path)
    {
        printk("Malloc failed...\n");
        return;
    }


    p = d_path(dentry, vfsmnt, path, buff_length); /*get path of parent folder*/

    int comparison = strcmp(p, FREEZERPATH); /*get int to compare write path and freeze path*/

    //printk("comparison: %d\n",comparison);

    if (comparison == 0) /*check path comparison*/
    {
        /*This is where we catch that a write has been done inside our folder*/
        wait_event_interruptible(wq, flag != 0); /*make requests wait*/
        flag = 0; /*set condition when woken up*/

        printk(KERN_INFO "Waking up in %i\n", current->pid);    /*check which thread is working*/

        /*TODO: use the linked list for data passing?...*/
        ll_add(ll, file->f_dentry->d_name.name);    /*add the (dirty) file name to the linked list*/
        ll_display(ll);     /*print out the list in the kernel log*/

        umh_runner("log", file->f_dentry->d_name.name); /*wake up userspace helper with "log" argument and filename*/

        flag = 1;   /*set the condition to wake up waiting threads*/
        wake_up_interruptible(&wq); /*wait up next thread on wait queue*/

    }

    kfree(path);

    return;
}


/**
 * this module's "open" function
 * @param  inode 
 * @param  file  
 */
static int device_open(struct inode *inode, struct file *file)
{
    printk("opener\n");
    try_module_get(THIS_MODULE);

    return 0;
}

/**
 * this module's "close" function
 * @param  inode 
 * @param  file  
 */
static int device_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    /*up(&char_arr.sem);*/
    return 0;
}


/**
 * writes userspace argument from buff into the linked list.
 * @param  filp file pointer from open
 * @param  buff what to write from
 * @param  len  # of bytes to write
 * @param  off  offset for write
 * @return      0 successful, error otherwise
 */
static ssize_t device_write(struct file *filp, const char *buff,
                            size_t len, loff_t *off)
{
    printk("writer\n");
    int copy_len = len;
    unsigned long amnt_copied = 0;

    char *s = (char *)kmalloc(len, GFP_KERNEL);

    /* NOTE: copy_from_user returns the amount of bytes _not_ copied */
    amnt_copied = copy_from_user(s, buff, copy_len);

    ll_add(ll, s);
    ll_display(ll);

    if (copy_len == amnt_copied)
        return -EINVAL;

    return copy_len - amnt_copied;
}

/**
 * reads from kernel to userspace by dequeueing from the linked list
 * and copying to user
 * @param  filp   file pointer from open
 * @param  buffer what to write into
 * @param  len    # of bytes to write
 * @param  offset offset for write
 */
static ssize_t device_read(struct file *filp, char *buffer, size_t len,
                           loff_t *offset)
{
    char *c = (char *)kmalloc(len, GFP_KERNEL);
    if (ll->length != 0)
    {
        c = ll_dequeue(ll);
        if (c == NULL)
        {
            return 0;
        }
    }
    else return 0;

    printk("read\n");
    unsigned long amnt_copied;
    int amnt_left = BUF_LEN ;
    char *copy_position = c;
    int copy_len = len;
    printk("C is: %s\n", c);
    printk("1\n");
    /* are we at the end of the buffer? */
    if (amnt_left <= 0)
        return 0;
    printk("2\n");
    /* NOTE: copy_to_user returns the amount of bytes _not_ copied */
    amnt_copied = copy_to_user(buffer, copy_position, copy_len);
    if (copy_len == amnt_copied)
        return -EINVAL;


    return copy_len - amnt_copied;
}

/**
 * init function for this module. creates linked list, sets system call hook,
 * and wakes userspace helper with an argument of "snapshot" to copy a snapshot
 * from the frozen directory to a secure, persistent, directory
 */
int init_module(void)
{
    ll = ll_create();


    printk("Created ring buffer\n");
    //sema_init(&rb->sem,1);

    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0)
    {
        printk(KERN_ALERT "Failed to register char device.\n");
        return Major;
    }

    memset(msg, '+', BUF_LEN);
    printk(KERN_INFO "chardev is assigned to major number %d.\n",
           Major);

    //  init_waitqueue_head(&queue);
    umh_runner("snapshot", NULL);
    sys_wr_hook = freezer_hook;
    return 0;
}

/**
 * destroy function for module. Destroys linked list, unregisters device,
 * nullifies system hook, and wakes up the user space helper with an 
 * argument of "restore" to restore the frozen file path from the snapshot directory
 */
void cleanup_module(void)
{
    ll_destroy(ll);

    printk("Removing system hook...\n");
    sys_wr_hook = NULL;

    int ret = unregister_chrdev(Major, DEVICE_NAME);
    if (ret < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);

    printk("Attempting to restore\n");
    umh_runner("restore", NULL);

    return;
}
