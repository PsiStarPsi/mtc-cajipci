#include "lib/chardev.h"
#include "lib/pcidev.h"
#include <asm/div64.h>

struct cajipci_chardev * chardev;

struct file_operations cajipci_chardev_fops;

int cajipci_chardev_major=-1;


#define READBUFFERSIZE 4096

int cajipci_init_chardev()
{
	dev_t tmpchrdev;
	chardev = kmalloc(sizeof(struct cajipci_chardev), GFP_KERNEL);
	chardev->open_sem = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	chardev->pos = 0;
	chardev->buffersize=READBUFFERSIZE;
	chardev->buffer = kmalloc(chardev->buffersize,GFP_KERNEL);


	sema_init(chardev->open_sem, 1);

	cajipci_chardev_fops.open =  cajipci_chardev_open;
	cajipci_chardev_fops.read = cajipci_chardev_read;
	cajipci_chardev_fops.write = cajipci_chardev_write;
	cajipci_chardev_fops.release = cajipci_chardev_release;
	cajipci_chardev_fops.llseek = cajipci_chardev_lseek;
	if(alloc_chrdev_region (&tmpchrdev,0,1,"chardev")<0)
	{
		printk("cajipci CPCI Driver: couldnt get major/minor number.\n");
		return -1;
	}
	cajipci_chardev_major=MAJOR(tmpchrdev);
	cdev_init ( &chardev->cdev, &cajipci_chardev_fops);
	chardev->cdev.owner = THIS_MODULE;
	chardev->cdev.ops = &cajipci_chardev_fops;
	if(cdev_add(&chardev->cdev, MKDEV(cajipci_chardev_major, 0), 1) < 0)
	{
		printk("cajipci CPCI Driver: Failed to initialize character device\n");
		return -1;
	}
	printk("cajipci CPCI Driver: Initialized character device with major number %d\n", cajipci_chardev_major);
	return 0;
}

void cajipci_exit_chardev()
{
	cdev_del(&chardev->cdev);
	unregister_chrdev_region (MKDEV(cajipci_chardev_major, 0),1);
	kfree(chardev->open_sem);
	kfree(chardev->buffer);
	kfree(chardev);
}

int cajipci_chardev_open(struct inode *inode,struct file *filep)
{
	printk(KERN_INFO "cajipci CPCI Driver: %d opened the pci driver' chr dev\n", current->pid);
	return 0;
}

int cajipci_chardev_release(struct inode *inode,struct file *filep)
{
	printk(KERN_INFO "cajipci CPCI Driver: %d close the pci driver's chr dev\n", current->pid);
	return 0;
}



ssize_t cajipci_chardev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t t_count;
	void* udata;
	//printk("cajipci CPCI Driver: %d tried to read %d bytes\n", (int)current->pid, (int)count);
	if (down_interruptible (chardev->open_sem))
	{
		printk(KERN_WARNING "cajipci CPCI Driver: %d tried to read the cajipcipci driver but its already in use\n", current->pid);
		return -ERESTARTSYS;
	}
	udata = kmalloc(sizeof(char)*count, GFP_KERNEL);
	t_count = cajipci_readregion_pci(udata, *f_pos, count);
	if(copy_to_user(buf, udata, count) != 0)
	{
		printk("KERN_ERROR" "cajipci CPCI Driver: copy to user failed\n");
	}
	*f_pos += t_count;
	kfree(udata);
	up(chardev->open_sem);
	return t_count;
}


ssize_t cajipci_chardev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t t_count;
	void * udata;
	//printk("cajipci CPCI Driver: %d tried to write %d bytes\n", (int)current->pid, (int)count);
	if (down_interruptible (chardev->open_sem))
	{
		printk(KERN_WARNING "cajipci CPCI Driver: %d tried to write to the cajipci pci driver but its already in use\n", current->pid);
		return -ERESTARTSYS;
	}
	udata = kmalloc(sizeof(char)*count, GFP_KERNEL);
	if(copy_from_user(udata, buf, count) != 0)
	{
		printk("KERN_ERROR" "cajipci CPCI Driver: copy from user failed\n");
	}
	t_count = cajipci_writeregion_pci(udata, *f_pos, count);
	*f_pos += t_count;
	kfree(udata);
	up(chardev->open_sem);
	return t_count;
}

loff_t cajipci_chardev_lseek(struct file *file, loff_t offset, int orig)
{
	switch (orig)
	{
		case 0:
			return file->f_pos = offset;
		case 1:
			return file->f_pos += offset;
		case 2:
			return file->f_pos = offset;
		default:
			return -1;
	}
}
