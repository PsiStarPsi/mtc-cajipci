//Required imports from the kernel tree
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
//Configuration Imports
#include "lib/about.h"
#include "lib/util.h"
#include "lib/config.h"
#include "lib/pci.h"
#include "lib/chardev.h"
static int altix_module_init(void)
{
	int err_pci = 0;
	int err_char = 0;
	printk(KERN_WARNING "Initializing altix driver.\n" );
	printk(KERN_WARNING "Initializing altix pci ...\n" );
	err_pci=altix_init_pci();
	if (err_pci != 0)
	{
		printk(KERN_ERR "..altix_init_pci() == %d\n",err_pci);
		goto fail_pci;
	}
	printk(KERN_WARNING "Done\n" );
	printk(KERN_WARNING "Initializing altix chardev \n" );
	err_char=altix_init_chardev();
	if (err_char != 0)
	{
		printk(KERN_ERR "..altix_init_pci() == %d\n",err_pci);
		goto fail_chardev;
	}
	printk(KERN_WARNING "Done\n" );
	printk(KERN_WARNING "Initialized altix driver.\n" );
	return 0;	//Initialization is compleated
fail_chardev:
	printk(KERN_ERR "Failed to initialize altix char driver.\n" );
	altix_exit_chardev();
fail_pci:
	printk(KERN_ERR "Failed to initialize altix pci driver, pci was not initialized.\n" );
	altix_exit_pci();
	return -1;
}

static void altix_module_exit(void)
{
	//Remove the character device
	printk(KERN_WARNING "exit altix3 chardev driver.\n" );
	altix_exit_chardev();
	altix_exit_pci();
}
module_init(altix_module_init);
module_exit(altix_module_exit);
