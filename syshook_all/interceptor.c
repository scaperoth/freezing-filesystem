#include <linux/version.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/highmem.h>
#include <asm/unistd.h>

/* IOCTL commands */
#define IOCTL_PATCH_TABLE 0x00000001
#define IOCTL_FIX_TABLE   0x00000004
int is_set = 0;
unsigned long *sys_call_table = (unsigned long*)0xc11f04e0;

static int our_open(struct inode *inode, struct file *file);
static int our_release(struct inode *inode, struct file *file);
static int our_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
asmlinkage int custom_open(const char *__user file_name, int flags, int mode);
static int __init init_interceptor(void);
static void __exit cleanup_interceptor(void);
int make_rw(unsigned long address);
int make_ro(unsigned long address);
pte_t *lookup_address(unsigned long address);

/* Pointer to the original sys_open */
asmlinkage int (*real_open)(const char *__user, int, int);  

static const struct file_operations our_fops = \
{
    .owner = THIS_MODULE,
    .open = &our_open,
    .release = &our_release,
    .unlocked_ioctl = (void *) &our_ioctl,
    .compat_ioctl = (void *) &our_ioctl
};

static struct miscdevice our_device = {
    MISC_DYNAMIC_MINOR,
    "interceptor",
    &our_fops
};

/* Beware, that some of  kernel functions may not be available
      to your code if you use license other then GPL */
MODULE_LICENSE("GPL");
/* Your name and email goes here */
MODULE_AUTHOR("Scaperoth");
/* Version of your module */
MODULE_VERSION("1.0");
/* Write a line about what this module is */
MODULE_DESCRIPTION("Hooks into system calls");

module_init(init_interceptor);
module_exit(cleanup_interceptor);

/* We will set this variable to 1 in our open handler and erset it
       back to zero in release handler*/
int in_use = 0;

/* This function will be invoked each time a user process attempts
    to open our device. You should keep in mind that the prototype
   of this function may change along different kernel versions. */
static int our_open(struct inode *inode, struct file *file)
{
    /* We would not like to allow multiple processes to open this device */
    if (in_use)
        return -EBUSY;
    in_use++;
    printk("device has been opened\n");
    return 0;
}

/* This function, in turn, will be called when a process closes our device */
static int our_release(struct inode *inode, struct file *file)
{
    in_use--;
    printk("device has been closed\n");
    return 0;
}

/* This function will handle ioctl calls performed on our device */
static int our_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    switch (cmd)
    {
    case IOCTL_PATCH_TABLE:
        make_rw((unsigned long)sys_call_table);
        real_open = (void *) * (sys_call_table + __NR_open);
        *(sys_call_table + __NR_open) = (unsigned long)custom_open;
        make_ro((unsigned long)sys_call_table);
        is_set = 1;
        break;
    case IOCTL_FIX_TABLE:
        make_rw((unsigned long)sys_call_table);
        *(sys_call_table + __NR_open) = (unsigned long)real_open;
        make_ro((unsigned long)sys_call_table);
        is_set = 0;
        break;
    default:
        printk("Ooops....\n");
        break;
    }
    /* We will fill this function in the Part III of this series */

    return retval;
}



/* Our replacement */
asmlinkage int custom_open(const char *__user file_name, int flags, int mode)
{
    printk("interceptor: open(\"%s\", %X, %X)\n", file_name,
           flags,
           mode);
    return real_open(file_name, flags, mode);
}

pte_t *lookup_address(unsigned long address) 
  { 
          pgd_t *pgd = pgd_offset_k(address);
          pud_t *pud;
          pmd_t *pmd;
          if (pgd_none(*pgd))
                  return NULL;
          pud = pud_offset(pgd, address);
          if (pud_none(*pud))
                  return NULL;
          pmd = pmd_offset(pud, address);
          if (pmd_none(*pmd))
                  return NULL;
          if (pmd_large(*pmd))
                  return (pte_t *)pmd;
          return pte_offset_kernel(pmd, address);
  } 

/* Make the page writable */
int make_rw(unsigned long address)
{
    unsigned int level;
    pte_t *pte = lookup_address(address);
    if (pte->pte_low &~ _PAGE_RW)
        pte->pte_low |= _PAGE_RW;
    return 0;
}


/* Make the page write protected */
int make_ro(unsigned long address)
{
    unsigned int level;
    pte_t *pte = lookup_address(address);
    pte->pte_low = pte->pte_low &~ _PAGE_RW;
    return 0;
}

static int __init init_interceptor(void) /* You may use any name other than
                                          init_module */
{
    int retval = misc_register(&our_device);
    printk(KERN_INFO "We are in kernel space\n");
    return retval;
}

static void __exit cleanup_interceptor(void)
{

    if (is_set)
    {
        make_rw((unsigned long)sys_call_table);
        *(sys_call_table + __NR_open) = (unsigned long)real_open;
        make_ro((unsigned long)sys_call_table);
    }

    misc_deregister(&our_device);
    printk(KERN_INFO "Elvis has left the building\n");
    return;
}

