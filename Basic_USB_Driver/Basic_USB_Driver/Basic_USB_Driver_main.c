#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Proprietary");

static int __init Basic_USB_Driver_init(void)
{
	printk("Basic_USB_Driver: Hello, world!\n");
	return 0;
}

static void __exit Basic_USB_Driver_exit(void)
{
	printk("Basic_USB_Driver: Goodbye, world!\n");
}

module_init(Basic_USB_Driver_init);
module_exit(Basic_USB_Driver_exit);
