#include "chardev.h"
#include "ring.h"
/* Globals localized to file (by use of static */
static int Major;		/* assigned to device driver */
static char msg[BUF_LEN];	/* a stored message */
static ring_buffer rb;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

struct scull_pipe {
	wait_queue_head_t inq, outq; /* read and write queues */
	char *buffer, *end; /* begin of buf, end of buf */
	int buffersize; /* used in pointer arithmetic */
	char *rp, *wp; /* where to read, where to write */
	int nreaders, nwriters; /* number of openings for r/w */
	struct fasync_struct *async_queue; /* asynchronous readers */
	struct semaphore sem; /* mutual exclusion semaphore */
};

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

static int spacefree(struct scull_pipe *dev)
{
	if (dev->rp == dev->wp)
		return dev->buffersize - 1;
	return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}

static int device_getwritespace(struct scull_pipe *dev, struct file *filp)
{
	while (spacefree(dev) == 0) { /* full */
		DEFINE_WAIT(wait);
		up(&dev->sem);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		//PDEBUG("\"%s\" writing: going to sleep\n",current->comm);
		prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
		if (spacefree(dev) == 0)
			schedule( );
		finish_wait(&dev->outq, &wait);
		if (signal_pending(current))
			return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		if (down_interruptible(&dev->sem))
			return -ERESTARTSYS;
	}

	return 0;
}


/* Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t device_write(struct file *filp, const char *buff,
			    size_t len, loff_t * off)
{
	printk("writer\n");

    struct scull_pipe *dev = filp->private_data;
 	int result;

 	printk("down_interruptible\n");
 	if (down_interruptible(&dev->sem)){
 		printk("inside down_interruptible\n");
 		return -ERESTARTSYS;
 	}
 		

 	printk("device_getwritespace\n");
 	/* Make sure there's space to write */
	result = device_getwritespace(dev, filp);

 	if (result)
 		return result; /* device_getwritespace called up(&dev->sem) */

	/* ok, space is there, accept something */
 	len = min(len, (size_t)spacefree(dev));

 	if (dev->wp >= dev->rp)
 		len = min(len, (size_t)(dev->end - dev->wp)); /* to end-of-buffer */
 	else /* the write pointer has wrapped, fill up to rp-1 */
		len = min(len, (size_t)(dev->rp - dev->wp - 1));

 	//PDEBUG("Going to accept %li bytes to %p from %p\n", (long)len, dev->wp, buff);
 	if (copy_from_user(dev->wp, buff, len)) {
 		up (&dev->sem);
 		return -EFAULT;
 	}

 	dev->wp += len; 
 	if (dev->wp == dev->end)
 		dev->wp = dev->buffer; /* wrapped */
 	up(&dev->sem);
 	/* finally, awake any reader */
 	wake_up_interruptible(&dev->inq); /* blocked in read( ) and select( ) */
 	/* and signal asynchronous readers, explained late in chapter 5 */
 	if (dev->async_queue)
 		kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
 	//PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)len);

 	return len;
}

static ssize_t device_read(struct file *filp, char *buff, size_t len,
			   loff_t * offset)
{
	printk("reader\n");
	struct scull_pipe *dev = filp->private_data;
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	while (dev->rp == dev->wp) { /* nothing to read */
	 	up(&dev->sem); /* release the lock */
	 	if (filp->f_flags & O_NONBLOCK)
	 		return -EAGAIN;
	 	//PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
	 	if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
	 		return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
	 	/* otherwise loop, but first reacquire the lock */
	 	if (down_interruptible(&dev->sem))
	 		return -ERESTARTSYS;
	 	}
	 	/* ok, data is there, return something */
	 	if (dev->wp > dev->rp)
	 		len = min(len, (size_t)(dev->wp - dev->rp));
	 	else /* the write pointer has wrapped, return data up to dev->end */
	 		len = min(len, (size_t)(dev->end - dev->rp));
	 	if (copy_to_user(buff, dev->rp, len)) {
	 		up (&dev->sem);
	 	return -EFAULT;
 	}
 	dev->rp += len;
 	if (dev->rp == dev->end)
 		dev->rp = dev->buffer; /* wrapped */
 	up (&dev->sem);
 	/* finally, awake any writers and return */
 	wake_up_interruptible(&dev->outq);
 	//PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)len);
 	return len;
}

int init_module(void)
{
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
	int ret = unregister_chrdev(Major, DEVICE_NAME);
	if (ret < 0)
		printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}
