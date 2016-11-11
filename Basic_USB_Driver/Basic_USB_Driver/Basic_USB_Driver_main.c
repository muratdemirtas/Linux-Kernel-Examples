//BASIC USB DEVICE DRIVER MODULE EXAMPLE FOR LINUX///

//include required headers
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

//usb device identifiers. NOTE: I USED STM32 STLINK V2.0 FOR THIS APP,BUT  YOU CAN CHANGE FOR ARDUINO
#define USB_DEVICE_VENDOR_ID  0x0483
#define USB_DEVICE_PRODUCT_ID 0x3748

//variables for usb device read and write 
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PCKG_SIZE 512
static unsigned char bulk_buf[MAX_PCKG_SIZE];
static int readcount = 0;
static int value = -1;
//USB DEVICE EVENT CALLBACKS, ALL STATIC
static int usb_open(struct inode*, struct file*);
static int usb_close(struct inode*, struct file*);
static ssize_t usb_write(struct file*, const char*, size_t, loff_t*);
static ssize_t usb_read(struct file*, char*, size_t, loff_t*);
static int usb_device_plugged(struct usb_interface *intf, const struct usb_device_id *id);
static void usb_device_disconnected(struct usb_interface *intf);
//DEFINE MODUL INFO
#define AUTHOR "MURAT DEMIRTAS <muratdemirtastr@gmail.com>"
#define DESCRIPTION "BASIC USB DEVICE DRIVER"
#define LICENSE "GPL"

MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);

//define module init exit functions.
static int __init usb_driver_init(void);
static void __exit usb_driver_exit(void);


//CREATE NEW USB DEVICE AND ITS CLASS DRIVER
static struct  usb_device *dev;
static struct usb_class_driver usb_device_class;

//SET VENDOR ID AND PRODUCT ID FOR USB RECOGNITION. WHEN YOU PLUG USB DEVICE , WILL TRIGGER
static struct usb_device_id usb_driver_table[] = { 
	{ USB_DEVICE(USB_DEVICE_VENDOR_ID, USB_DEVICE_PRODUCT_ID) },
{}
}; 
 
//REGISTER MODULE REGISTER TABLE FOR THIS USB DEVICE
MODULE_DEVICE_TABLE(usb, usb_driver_table);

//USB SERIAL COMMUNICATION PORT OPENED CALLBACK FUNCTION.
static int usb_open(struct inode *ind, struct file *f)
{
	printk(KERN_INFO "USB: USB SERIAL BUS OPENED.\n");
	return 0 ;
}

//USB SERIAL COMMUNICATION PORT CLOSED CALLBACK FUNCTION.
static int usb_close(struct inode *ind, struct file *f)
{
	printk(KERN_INFO "USB: USB SERIAL BUS CLOSED.\n");
	return 0 ;
}

//USB SERIAL COMMUNICATION(WRITE OPERATIONS) CALLBACK FUNCTION.
static ssize_t usb_write(struct file *f, const char *buffer, size_t buffer_len, loff_t *offsetCounter)
{
	readcount = 0;
	if (copy_from_user(bulk_buf, buffer, buffer_len))
	{
		printk(KERN_ALERT "USB: ERROR READING FROM USB DEVICE, ERR: BAD ADRESS\n");
		
		return -EFAULT;
	}
    //CTR//  //MESSAGE  //DEVICE  //SEND FUNCT. //DEV  //WRITE   //BUFFER   //MAX PCK SIZE  //ACTUAL LENGTH  //TIMEOUT	
	value = usb_bulk_msg(dev, usb_sndbulkpipe(dev, BULK_EP_OUT), bulk_buf, MAX_PCKG_SIZE, &readcount, 5000);
	if (value < 0)
	{
		printk(KERN_ALERT "USB: CANT WRITE TO USB DEVICE.\n");
		return value;
	}
	printk(KERN_INFO "USB: WRITING TO USB DEVICE.\n");
	return 0;
	
}

//USB SERIAL COMMUNICATION(READ OPERATIONS) CALLBACK FUNCTION.
static ssize_t usb_read(struct file *f, char *buffer, size_t buffer_len, loff_t *offsetCounter)
{
	readcount = 0;
	 //CTR//  //MESSAGE  //DEVICE  //READ FUNCT. //DEV  //WRITE   //BUFFER   //MAX PCK SIZE  //ACTUAL LENGTH   //TIMEOUT	
	value = usb_bulk_msg(dev, usb_rcvbulkpipe(dev, BULK_EP_IN), bulk_buf, MAX_PCKG_SIZE, &readcount, 5000);

	if (value < 0)
	{
		printk(KERN_ALERT "USB:CANT READ MESSAGE FROM USB DEVICE.\n");
		return value;
	}
	
	if (copy_to_user(buffer, bulk_buf, buffer_len))
	{
		printk(KERN_ALERT "USB: ERROR READING FROM USB DEVICE, ERR: BAD ADRESS\n");
		return -EFAULT;
	}
	
	printk(KERN_INFO "USB: READING FROM USB DEVICE\n");
		
	return 0;			 //looks like smarter :)
}

//FILE OPERATION FUNCTION CALLBACKS FOR DEVICE I/O
static struct file_operations file_oprts = { 
	.read = usb_read,		//FOR READING USB READ
	.write = usb_write,		//FOR WRITING USB WRITE
	.open = usb_open,		//FOR OPENING SERIAL PORT
	.release = usb_close	//FOR CLOSING SERIAL PORT
};

//USB DEVICE RECOGNITION CALLBACK FUNCTION 
static int usb_device_plugged(struct usb_interface *intf, const struct usb_device_id *id)
{
	//if plugged to linux system.
	dev = interface_to_usbdev(intf);     //get usb interface arguments
	usb_device_class.name = "/usb/stm32%d";   //set name
	usb_device_class.fops = &file_oprts;      //set file operations.
	
	printk(KERN_INFO "USB: OUR USB PLUGGED IN!, Vendor ID: %d, Product ID: %d", id->idVendor, id->idProduct);
	printk(KERN_INFO "USB: OUR USB DEVICE NOW REGISTERING TO DEVICE TABLE");
	
	//IF NOT REGISTERED PRINT ERROR
	if ((value = usb_register_dev(intf, &usb_device_class)) < 0)
	{
		printk(KERN_ALERT "USB: USB DEVICE REGISTER FAILED, TRY RECONNECT.\n");
	}
	//ELSE 
	else
	{
		printk(KERN_INFO "USB:USB DEVICE HAS BEEN REGISTERED SUCCESSFULLY, MINOR NO: %d", intf->minor);
	}
	return value;
}

//USB DEVICE DISCONNECT CALLBACK, WE MUST DE REGISTER OUR USB DEVICE
static void usb_device_disconnected(struct usb_interface *intf)
{
	printk(KERN_INFO "USB: USB DEVICE NOW REMOVING FROM SYSTEM.\n");
	//REMOVE USB INTERFACE FROM KERNEL 
	usb_deregister_dev(intf, &usb_device_class);
	printk(KERN_INFO "USB: USB DEVICE DISCONNECTED.\n");
	
}

//OUR USB DEVICE IDENTIFIER
static struct usb_driver our_usb_device = {
	.name = "stm32",			//NAME FOR OUR USB
	.id_table = usb_driver_table,	///TABLES.
	.probe = usb_device_plugged,	//PLUGGED CALLBACK
	.disconnect = usb_device_disconnected  //DISCONNECTED CALLBACK
};

//INIT FUNCTION FOR LOAD USB DRIVER MODULE
static int __init usb_driver_init(void)
{
	printk(KERN_INFO "USB: USB DRIVER MODULE EXAMPLE NOW LOADED TO SYSTEM.\n");
	value = usb_register(&our_usb_device);		//REGISTER USB DEVICE AND CHECK 
	
	//IFNOT REGISTERED, EXIT AND RETURN VALUE
	if (value < 0)
	{
		printk(KERN_ALERT "USB: DEVICE CAN NOT REGISTERED.\n");
		return value;
	}
	//SO IF WE ARE HERE,THEN MODULE REGISTRATION IS SUCCESS.
	
	printk(KERN_INFO "USB: USB REGISTRATION SUCCESFULL.\n");
	return value;
}

//EXIT FUNCTION FOR CLOSE USB DRIVER MODULE
static void __exit usb_driver_exit(void)
{
	printk(KERN_INFO "USB: USB DRIVER MODULE WILL QUIT. BYE BYE.\n");
	usb_deregister(&our_usb_device);    //DEREGISTER USB DEVICE
}





//SET FUNCTIONS FOR USB MODULE
module_init(usb_driver_init);
module_exit(usb_driver_exit);
