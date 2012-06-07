//Required imports from the kernel tree
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
//Configuration Imports
#include "lib/about.h"
#include "lib/chardev.h"
#include "lib/pcidev.h"

static int dumb_module_init(void)
{
	int err_pci=0,err_char=0;

	printk(KERN_ERR "Initializing dumb CPCI driver..\n" );

	err_pci=dumb_init_pci();
	printk(KERN_ERR "..dumb_init_pci() == %d\n",err_pci);
	if (err_pci != 0) goto fail_pci;

	err_char=dumb_init_chardev();
	printk(KERN_ERR "..dumb_init_chardev() == %d\n",err_char);
	if (err_char != 0) goto fail_chardev;

	printk(KERN_ERR "Initialized dumb CPCI driver.\n" );
	return 0;	//Initialization is compleated

	fail_chardev:
		printk(KERN_ERR "Failed to initialize dumb char driver.\n" );
		dumb_exit_chardev();
	fail_pci:
		printk(KERN_ERR "Failed to initialize dumb CPCI driver, char dev was not initialized.\n" );
		dumb_exit_pci();
		return -1;
}

static void dumb_module_exit(void)
{
	//Remove the character device
	printk(KERN_WARNING "exit dumb CPCI driver.\nexit dumb char driver.\n" );
	dumb_exit_chardev();
	dumb_exit_pci();
}
module_init(dumb_module_init);
module_exit(dumb_module_exit);

