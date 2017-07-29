/*
===============================================================================
Driver Name		:		kernel_timer
Author			:		MURAT DEMIRTAS
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#define DMESG_ALIAS "timerlog"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DMESG_ALIAS, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DMESG_ALIAS, ##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DMESG_ALIAS, ##args)

#include <linux/kernel.h>			 //kernel mode library
#include <linux/module.h>            //kernel module library
#include <linux/timer.h>             //kernel mode timer library
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/vermagic.h>
#include <linux/moduleparam.h>

MODULE_ALIAS("timerModule");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("murat demirtas <muratdemirtastr@gmail.com>");
MODULE_DESCRIPTION("Example kernel mode timer usage");

static int timer_delay;
static struct timer_list my_timer;

#define DEBUG_MODE

void my_timer_callback( unsigned long data )
{
	PINFO("kernel_timer_timer executing\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));
}

static void debug_when_init(void)
{

	#ifdef DEBUG_MODE
	PINFO("Kernel version is: %s ",UTS_RELEASE);
	PINFO("Process ID is: %d",current->pid);
	PINFO("Module called with:%s ",current->comm);
	PINFO("Received parameter: %d ",timer_delay);
	#endif

}

static int __init kernel_timer_init(void)
{
	  debug_when_init();
	/* setup your timer to call my_timer_callback */
	  setup_timer(&my_timer, my_timer_callback, 1);
	  /* setup timer interval to 200 msecs */
	  mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));

	  return 0;

	PINFO("INIT\n");

	return 0;
}

static void __exit kernel_timer_exit(void)
{	

	/* remove kernel timer when unloading module */
	del_timer(&my_timer);
	return;
	PINFO("EXIT\n");

}

module_init(kernel_timer_init);
module_exit(kernel_timer_exit);

module_param(timer_delay, int, S_IRUGO);







