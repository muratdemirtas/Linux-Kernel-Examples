//BASIC LINUX USB DRIVER EXAMPLE

//protect for redefine our class, define class
#ifndef BASIC_USB_DRIVER_MAIN_H
#define  BASIC_USB_DRIVER_MAIN_H
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

//Define Example Module infos
#define AUTHOR "MURAT DEMIRTAS <muratdemirtastr@gmail.com>"
#define DESCRIPTION "BASIC USB DEVICE DRIVER"
#define LICENSE "GPL"

//register infos to module
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);

static int __init usb_driver_init(void);
static void __exit usb_driver_exit(void);


#endif // !BASIC_USB_DRIVER_MAIN_H

