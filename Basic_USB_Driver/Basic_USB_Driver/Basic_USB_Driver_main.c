#include "Basic_USB_Driver_main.h"
#include "usb_device_events.h"

static struct  usb_device *dev;
static struct usb_class_driver usb_device_class;
static struct usb_device_id usb_driver_table[] = { 
	{ USB_DEVICE(USB_DEVICE_VENDOR_ID, USB_DEVICE_PRODUCT_ID)},
	{}
}; 
 
MODULE_DEVICE_TABLE(usb, usb_driver_table);

static struct usb_driver our_usb_device = {
	.name = "Our Example USB Device",
	.id_table = usb_driver_table,
	.probe = usb_device_plugged,
	.disconnect = usb_device_disconnected
};

static int usb_device_plugged(struct usb_interface *intf, const struct usb_device_id *id)
{
	dev = interface_to_usbdev(intf);     //get usb interface
	usb_device_class.name = "/usb/example%d";
	
	
}

static struct file_operations file_oprts = { 
	.read  =	usb.read,
	.write =	usb_write,
	.open =		usb_open,
	.release =  usb_close
	}

static void usb_device_disconnected(struct usb_interface *intf)
{
	
}

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
