//Required imports from the kernel tree
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
//Configuration Imports
#include "lib/about.h"
#include "lib/chardev.h"
#include "lib/pcidev.h"
#include "lib/settings.h"

struct blab_buffered_data* blab_data;

static int blab_module_init(void)
{
	//Initialize the blab data windows
	int i;
	int err_pci;
	int err_char;
	blab_data = kmalloc(sizeof(struct blab_buffered_data), GFP_KERNEL);
	blab_data->data_stamp = (int*)kmalloc(sizeof(int)*BUFFSIZE, GFP_KERNEL);
	blab_data->stamp = 0;
	for(i = 0; i< BUFFNUM; i++)
	{
		blab_data->data[i] = kmalloc(sizeof(int32_t)*BUFFSIZE, GFP_KERNEL);
	}
	//Done.
	err_pci=0;
	err_char=0;

	printk(KERN_ERR "Initializing BLAB3 CPCI driver..\n" );

	err_pci=blab_init_pci();
	printk(KERN_ERR "..blab_init_pci() == %d\n",err_pci);
	if (err_pci != 0) goto fail_pci;

	err_char=blab_init_chardev();
	printk(KERN_ERR "..blab_init_chardev() == %d\n",err_char);
	if (err_char != 0) goto fail_chardev;


	printk(KERN_ERR "Initialized BLAB3 CPCI driver.\n" );
	return 0;	//Initialization is compleated

	fail_chardev:
		printk(KERN_ERR "Failed to initialize BLAB3 char driver.\n" );
		blab_exit_chardev();
	fail_pci:
		printk(KERN_ERR "Failed to initialize BLAB3 CPCI driver, char dev was not initialized.\n" );
		blab_exit_pci();
		return -1;
}

static void blab_module_exit(void)
{
	int i;
	kfree(blab_data->data_stamp);

	kfree(blab_data);
	for(i = 0; i< BUFFNUM; i++)
	{
		kfree(blab_data->data[i]);
	}
	//Remove the character device
	printk(KERN_WARNING "exit BLAB3 CPCI driver.\nexit BLAB3 char driver.\n" );
	blab_exit_chardev();
	blab_exit_pci();
}
module_init(blab_module_init);
module_exit(blab_module_exit);

