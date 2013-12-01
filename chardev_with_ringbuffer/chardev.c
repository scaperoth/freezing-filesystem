#include "chardev.h"
#include "ring.h"
/* Globals localized to file (by use of static */
static int Major;		/* assigned to device driver */
static char msg[BUF_LEN];	/* a stored message */
static ring_buffer rb;
static int MAXSIZE = 0xFFF;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static void freezer_hook(unsigned int fd, struct file* file, const char __user *buf, size_t count){
	char proclnk[0xFFF];
	printk("hooked write\n");
	
	struct dentry * d_entry = file->f_dentry;

	printk("FNAME: ");

	 	printk(" %s ",d_entry->d_name.name);
	 	d_entry = d_entry->d_parent;
	 	printk(" %s ",d_entry->d_name.name);
	 	d_entry = d_entry->d_parent;
	 	printk(" %s ",d_entry->d_name.name);
	 	d_entry = d_entry->d_parent;
	 	printk(" %s ",d_entry->d_name.name);
		
	printk("\n");
	//printk("FNAME = %s\n", filename);

	return;
}

static int device_open(struct inode *inode, struct file *file)
{
	rb = rb_create();
	try_module_get(THIS_MODULE);

	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	rb_delete(rb);
	module_put(THIS_MODULE);
	return 0;
}


/* Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t device_write(struct file *filp, const char *buff,
	size_t len, loff_t * off)
{
	printk("writer\n");
	int copy_len = len;
	unsigned long amnt_copied = 0;

	char* s = (char*)kmalloc(len,GFP_KERNEL);

     /* NOTE: copy_from_user returns the amount of bytes _not_ copied */
	amnt_copied = copy_from_user(s, buff, copy_len);

	if(!rb_isfull(rb)){
		rb_enqueue(rb, s);
		rb_display(rb);
	}else return 0;
	
	if (copy_len == amnt_copied)
		return -EINVAL;

	return copy_len - amnt_copied;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t len,
	loff_t * offset)
{
	char* c = (char*)kmalloc(len,GFP_KERNEL);
	if(!rb_isempty(rb)){
		c = rb_dequeue(rb);
		if(c==NULL){
			return 0;
		}
	}else return 0;

	printk("read\n");
	unsigned long amnt_copied;
	int amnt_left = BUF_LEN ;
	char *copy_position =c;
	int copy_len = len;
	printk("C is: %s\n",c);
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
	sys_wr_hook=freezer_hook;
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
		printk(KERN_ALERT "Failed to register char device.\n");
		return Major;
	}

	memset(msg, '+', BUF_LEN);
	printk(KERN_INFO "chardev is assigned to major number %d.\n",
		Major);

	return 0;
}
void cleanup_module(void)
{
	sys_wr_hook=NULL;
	int ret = unregister_chrdev(Major, DEVICE_NAME);
	if (ret < 0)
		printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}
