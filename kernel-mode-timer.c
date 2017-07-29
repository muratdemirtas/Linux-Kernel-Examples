/****************************************************************************
 * Copyright (C) 2017 by Murat DEMIRTAS                                     *
 *                                                                          *
 * This file is part of Box.                                                *
 *                                                                          *
 *   Box is free software: you can redistribute it and/or modify it         *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   Box is distributed in the hope that it will be useful,                 *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Box.  If not, see <http://www.gnu.org/licenses/>.   *
 ****************************************************************************/

/**
 * @file kernel-mode-timer.c
 * @author Murat Demirtas <muratdemirtastr@gmail.com>
 * @date 29 June 2017
 * @brief Example kernel mode timer usage example
 * @see http://www.makelinux.net/ldd3/chp-7-sect-4
 */

 /**
 * @brief function prototypes for printing message to kernel log
 * @usage use dmesg function for logs
 */
#define DMESG_ALIAS "timerlog"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DMESG_ALIAS, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DMESG_ALIAS, ##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DMESG_ALIAS, ##args)

#include <linux/kernel.h>	     //kernel mode library, used for dmesg facility
#include <linux/module.h>            //kernel module library
#include <linux/timer.h>             //kernel mode timer library
#include <linux/sched.h>             //for module infos
#include <linux/moduleparam.h>       //for module parameters
#include <linux/vermagic.h>          //for getting current kernel and process info
#include <linux/init.h>              //for freeing shared memories

 /**
 * @brief module descriptions
 */
MODULE_ALIAS("timerModule");     //it will be module alias
MODULE_LICENSE("GPL");           //module license (must be define)
MODULE_AUTHOR("murat demirtas <muratdemirtastr@gmail.com>");   //module author
MODULE_DESCRIPTION("Example kernel mode timer usage");         //module descriptoin

 /**
 * @brief variables used in this module 
 */
static int timer_delay = 1000;
static struct timer_list my_timer;
#define DEBUG_MODE

 /**
 * @brief debug function with printk()
 * @params data
 * @return none 
 * @note timer function only call one once, you must reinit for periodic callback
 */
void my_timer_callback( unsigned long data )
{
        /* print log */
	PINFO("kernel timer executing\n");
	/* setup timer interval to msecs */
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_delay));
}

 /**
 * @brief debug function with printk()
 * @params none
 * @return none 
 */
static void debug_when_init(void)
{
	#ifdef DEBUG_MODE
	PINFO("Kernel version is: %s ",UTS_RELEASE);
	PINFO("Process ID is: %d",current->pid);
	PINFO("Module called with:%s ",current->comm);
	PINFO("Received parameter: %d ",timer_delay);
	#endif
	return;
}

 /**
 * @brief this callback function will be fired when module removed
 * @params none
 * @info useful for freeing used objects,tasks, timers and memories
 * @return none 
 */
static int __init kernel_timer_init(void)
{         
	PINFO("Timer module initializing\n");
	debug_when_init();
	/* setup your timer to call my_timer_callback */
	setup_timer(&my_timer, my_timer_callback, 1);
	/* setup timer interval to msecs */
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_delay));
	return 0;
}

 /**
 * @brief this callback function will be fired when module removed
 * @params none
 * @info useful for freeing used objects,tasks, timers and memories
 * @return none 
 */
static void __exit kernel_timer_exit(void)
{	
	/* remove kernel timer when unloading module */
	del_timer(&my_timer);
	PINFO("Timer module removing\n");
	return;
}

 /**
 * @brief this function will fire when module installed from kernel space
 * @params parameter can define with insmod, ex: sudo insmod module.ko 1000
 * @info S_IRUGO for a parameter that can be read by the world but cannot be changed; 
 */
module_init(kernel_timer_init);
module_exit(kernel_timer_exit);
module_param(timer_delay, int, S_IRUGO);







