//BASIC LINUX USB DRIVER EXAMPLE

//protect for redefine our class, define class
#ifndef BASIC_USB_DRIVER_MAIN_H
#define BASIC_USB_DRIVER_MAIN_H

//include required headers
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

//Define Example Module infos
#define AUTHOR "MURAT DEMIRTAS <muratdemirtastr@gmail.com>"
#define DESCRIPTION "BASIC USB DEVICE DRIVER"
#define LICENSE "GPL"

#define USB_DEVICE_VENDOR_ID  0x0000
#define USB_DEVICE_PRODUCT_ID 0x0000

//register infos to module
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);


//define module init exit functions.
static int __init usb_driver_init(void);
static void __exit usb_driver_exit(void);

#endif // !BASIC_USB_DRIVER_MAIN_H

