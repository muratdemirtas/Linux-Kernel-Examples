//Plug and Disconnect callback functions 

#ifndef _USB_DEVICE_EVENTS_
#include  "Basic_USB_Driver_main.h"



static int usb_device_plugged(struct usb_interface *intf, const struct usb_device_id *id);
static void usb_device_disconnected(struct usb_interface *intf);

#endif // !_USB_DEVICE_EVENTS_
