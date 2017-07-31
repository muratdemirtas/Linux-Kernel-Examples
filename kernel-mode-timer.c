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
 * @file kernel-threads.c
 * @author Murat Demirtas <muratdemirtastr@gmail.com>
 * @date 31 June 2017
 * @brief Example kernel mode threads usage example
 * @see http://www.makelinux.net/ldd3/chp-7-sect-4
 */

#include "kernel_thread.h"          //header file includes some debug functions
#include <linux/kernel.h>           //used for do_exit()
#include <linux/threads.h>          //used for allow_signal
#include <linux/kthread.h>          //used for kthread_create
#include <linux/delay.h>            //used for ssleep()

/*@brief
 * variables for this example, task structs, cpu id's as integer
*/

static struct task_struct *worker_task,*default_task;
static int get_current_cpu,set_current_cpu;
#define WORKER_THREAD_DELAY 4
#define DEFAULT_THREAD_DELAY 6

/*@brief
 * Linux Kernel Module Parameters
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("muratdemirtas <muratdemirtastr@gmail.com> ");
MODULE_DESCRIPTION("Kernel thread example");
MODULE_ALIAS("Threading");


/*@brief  worker task main constructor
 *@note   it will forever after while() loop
 *@params arguments from when task creating with kthread_creat()
 *@return nothing
*/
static int worker_task_handler_fn(void *arguments)
{
	/*@brief
	 * here is your world, you can declare your variables,structs
	*/

	/* @note this macro will allow to stop thread from userspace
	 * or kernelspace
	 */
	allow_signal(SIGKILL);

	/*@attention while(true),while(1==1),for(;;) loops will can't
	 *receive signal for stopping thread
	 */
	while(!kthread_should_stop()){
		PINFO("Worker thread executing on system CPU:%d \n",get_cpu());
		ssleep(WORKER_THREAD_DELAY);

	/*@attention while(true),while(1==1),for(;;) loops will can't
     *kernel threads doesnt allow async signal handling unlike user space
	 *you must check signals in forever loops everytime
	 *if signal_pending function capture SIGKILL signal, then thread will exit
	 */

		if (signal_pending(worker_task))
			            break;
	}
	/*@brief
	 * do_exit is same as exit(0) function, but must be used with threads
	 */

	/*@brief
	 * Kernel Thread has higher priority than user thread because Kernel threads
	 * are used to provide privileged services to applications.
	 */

	do_exit(0);

	PERR("Worker task exiting\n");
	return 0;
}

/*@brief   default task main constructor
 *@attention this task have highest real time priority
 *@note   it will forever after while() loop
 *@params arguments from when task creating with kthread_creat()
 *@return nothing
*/
static int default_task_handler_fn(void *arguments)
{
	/*@brief
	 * here is your world, you can declare your variables,structs
	*/

	/* @note this macro will allow to stop thread from userspace
	 * or kernelspace
	 */
	allow_signal(SIGKILL);

	/*@attention while(true),while(1==1),for(;;) loops will can't
     *receive signal for stopping thread
	*/
	while(!kthread_should_stop())
	{
		PINFO("Default thread executing on system CPU:%d \n",get_cpu());
		ssleep(DEFAULT_THREAD_DELAY);

	/*@attention while(true),while(1==1),for(;;) loops will can't
	 *kernel threads doesnt allow async signal handling unlike user space
	 *you must check signals in forever loops everytime
	 *if signal_pending function capture SIGKILL signal, then thread will exit
	 */

	    if (signal_pending(default_task))
		            break;
	}

	PERR("Default task exiting\n");

	/*@brief
	 * do_exit is same as exit(0) function, but must be used with threads
	*/
	do_exit(0);

	return 0;
}

/*@brief linux kernel module initial macro function
 *@attention only run one once when module installed
 *@note   must be exit with return
 *@params nothing
 *@return signal status with int parameter
*/
static int __init kernel_thread_init(void)
{

	/*@brief scheduler priority structs to set task priority
	 */
	struct sched_param task_sched_params =
	{
			.sched_priority = MAX_RT_PRIO
	};

	task_sched_params.sched_priority = 90;

	PINFO("Initializing kernel mode thread example module\n");
	PINFO("Creating Threads\n");

	/*@brief get current cpu to bind over task
	 */
	get_current_cpu = get_cpu();
	PDEBUG("Getting current CPU %d to binding worker thread\n",
											get_current_cpu);

	/*@brief initialize worker task with arguments, thread_name and cpu
	 *@note you can see thread name using ps aux command on userspace
	 */
	worker_task = kthread_create(worker_task_handler_fn,
			(void*)"arguments as char pointer","thread_name");
	kthread_bind(worker_task,get_current_cpu);

	if(worker_task)
		PINFO("Worker task created successfully\n");
	else
		PINFO("Worker task error while creating\n");

	/*@brief initialize default task with arguments,thread_name and another cpu
	 *@note you can see thread name using ps aux command on userspace
	*/
	set_current_cpu = 2;
	PDEBUG("Getting current CPU %d to binding default thread\n",
											set_current_cpu);

	default_task = kthread_create(default_task_handler_fn,
				(void*)"arguments as char pointer","thread_name");
	kthread_bind(default_task,set_current_cpu);

	/*@brief set scheduler priority to default task
	*/
	sched_setscheduler(default_task, SCHED_FIFO, &task_sched_params);

	/*@brief tasks are now process, start them
	*/
	wake_up_process(worker_task);
	wake_up_process(default_task);

	/*@brief check task if they are started succesfully
	*/
	if(worker_task)
		PINFO("Worker task created successfully\n");
	else
		PINFO("Worker task error while creating\n");

	if(default_task)
		PINFO("Default task created successfully\n");
	else
		PINFO("Default task error while creating\n");

	return 0;
}

/*@brief linux kernel module deinitial macro function
 *@attention only run one once when module removed
 *@note   must be exit with return
 *@params nothing
 *@return signal status with int parameter
*/
static void __exit kernel_thread_exit(void)
{	

	PERR("Module removing from kernel, threads stopping\n");

	/*@brief
	 * this functions will send SIGKILL's to stop threads when module removing
	*/

	if(worker_task)
		kthread_stop(worker_task);

	PERR("Worker task stopped\n");

	if(default_task)
		kthread_stop(default_task);

	PERR("Default task stopped\n");

	PINFO("Bye Bye\n");

}

/*@brief
 * set initial and exit callback functions to module macro
*/
module_init(kernel_thread_init);
module_exit(kernel_thread_exit);

