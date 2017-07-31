/*
===============================================================================
Driver Name		:		kernel_thread
Author			:		MURATDEMIRTAS 
License			:		DUAL BSD/GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include "kernel_thread.h"
#include <linux/kernel.h>
#include <linux/threads.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *worker_task,*default_task;
static int get_current_cpu,set_current_cpu;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("muratdemirtas <muratdemirtastr@gmail.com> ");
MODULE_DESCRIPTION("Kernel thread example");
MODULE_ALIAS("Threading");

#define WORKER_THREAD_DELAY 4
#define DEFAULT_THREAD_DELAY 6

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

static int __init kernel_thread_init(void)
{

	struct sched_param task_sched_params =
	{
			.sched_priority = MAX_RT_PRIO
	};

	task_sched_params.sched_priority = 50;


	PINFO("Initializing kernel mode thread example module\n");
	PINFO("Creating Threads\n");

	get_current_cpu = get_cpu();
	PDEBUG("Getting current CPU %d to binding worker thread\n",
											get_current_cpu);

	worker_task = kthread_create(worker_task_handler_fn,
			(void*)"arguments as char pointer","thread_name");
	kthread_bind(worker_task,get_current_cpu);

	if(worker_task)
		PINFO("Worker task created successfully\n");
	else
		PINFO("Worker task error while creating\n");

	set_current_cpu = 2;
	PDEBUG("Getting current CPU %d to binding default thread\n",
											set_current_cpu);

	default_task = kthread_create(default_task_handler_fn,
				(void*)"arguments as char pointer","thread_name");
	kthread_bind(default_task,set_current_cpu);


	sched_setscheduler(default_task, SCHED_FIFO, &task_sched_params);

	wake_up_process(worker_task);
	wake_up_process(default_task);

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

static void __exit kernel_thread_exit(void)
{	

	PERR("Module removing from kernel, threads stopping\n");

	/*@brief
	 * this functions will send SIGKILL's to threads when module removing
	*/

	if(worker_task)
		kthread_stop(worker_task);

	PERR("Worker task stopped\n");

	if(default_task)
		kthread_stop(default_task);

	PERR("Default task stopped\n");

	PINFO("Bye Bye\n");

}

module_init(kernel_thread_init);
module_exit(kernel_thread_exit);

