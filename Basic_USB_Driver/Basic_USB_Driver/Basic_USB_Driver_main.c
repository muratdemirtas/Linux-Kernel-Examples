#include "Basic_USB_Driver_main.h"



static int __init usb_driver_init(void)
{
	printk("Basic_USB_Driver: Hello, world!\n");
	return 0;
}

static void __exit usb_driver_exit(void)
{
	printk("Basic_USB_Driver: Goodbye, world!\n");
}

module_init(usb_driver_init);
module_exit(usb_driver_exit);
