#include "chardev.h"
#include "list.h"
/* Globals localized to file (by use of static */
static int Major;       /* assigned to device driver */
static char msg[BUF_LEN];   /* a stored message */
static struct linked_list* ll;
const char *FREEZERPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/freezer";

//wait_queue_head_t queue;

static struct file_operations fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

DECLARE_WAIT_QUEUE_HEAD(wq);

static int flag = 0;

volatile int should_wait = 1;

static int umh_test(char *argument, char* filename )
{
    char **argv = kmalloc(sizeof(char *[3]), GFP_KERNEL);

    if(!argv){
        printk("Userspace args failed.\n");
        return 0;
    }

    char filepath[BUFF]="";

    if(filename!=NULL){
        strncat(filepath, FREEZERPATH,200);
        strncat(filepath, "/", 1);
        strncat(filepath, filename,200);
    }

    int ret = 0;

    argv[0] ="/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/callee";
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


static void freezer_hook(unsigned int fd, struct file *file, const char __user *buf, size_t count)
{


    //printk("hooked write\n");
    //
    int buff_length = PATH_MAX + 11;
    char *p, *path;
    struct dentry *dentry = file->f_dentry->d_parent;
    struct vfsmount *vfsmnt = file->f_vfsmnt;

    path = kmalloc(buff_length, GFP_KERNEL);

    if (!path)
    {
        printk("Malloc failed...\n");
        return;
    }


    p = d_path(dentry, vfsmnt, path, buff_length);

    int comparison = strcmp(p, FREEZERPATH);

    //printk("comparison: %d\n",comparison);

    if (comparison == 0)
    {
        wait_event_interruptible(wq, flag != 0);
        printk(KERN_INFO "Waking up in %i\n", current->pid);

        flag = 0;
        /*This is where we catch that a write has been done inside our folder*/

        printk("Freezer folder name:  %s\n", "freezer");

        ll_add(ll, file->f_dentry->d_name.name);
        ll_display(ll);

        umh_test("log", file->f_dentry->d_name.name);

        flag = 1;
        wake_up_interruptible(&wq);

    }
    kfree(path);

    return;
}

static int device_open(struct inode *inode, struct file *file)
{
    printk("opener\n");
    try_module_get(THIS_MODULE);
    /**
    if(down_interruptible(&char_arr.sem)) {
        printk(KERN_INFO " could not hold semaphore");
        return -1;
    }*/
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    /*up(&char_arr.sem);*/
    return 0;
}


/* Called when a process writes to dev file: echo "hi" > /dev/hello */
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

int init_module(void)
{
    ll = ll_create();


    printk("Created ring buffer\n");
    //sema_init(&rb->sem,1);

    sys_wr_hook = freezer_hook;

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

    return 0;
}
void cleanup_module(void)
{
    ll_destroy(ll);
    sys_wr_hook = NULL;
    int ret = unregister_chrdev(Major, DEVICE_NAME);
    if (ret < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}
