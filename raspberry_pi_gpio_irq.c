/*
 * Basic Linux Kernel module using GPIO interrupts.
 *
 * Author:
 * 	murat demirtas <muratdemirtastr@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "raspberry_pi_gpio_irq.h"        //used for debugging based printk()
#include <linux/sched.h>                  //used for getting module infos
#include <linux/vermagic.h>               //used for getting current kernel and process info
#include <linux/gpio.h>			  //used for hardware GPIO
#include <linux/interrupt.h>		  //used for interrupt handling

/**
 * @brief variables used for this example
 */
#define GPIO_PIN_DESC "For detecting rising edges"
#define GPIO_IRQ_DESC "Example GPIO Interrupt"
#define MODULE_DEBUG_MODE

/**
 * @brief we want GPIO_17 of BCM2836 (pin 11 on P5 pinout raspberry pi rev. 2 board)
 * to generate interrupt
 */
#define BCM2836_HW_GPIO_PIN 17
short int IRQ_NUMBER_OF_GPIO  = 0;

/**
 * @brief for return code of LKM
 */
enum {
	MODULE_OK,
	GPIO_FAILURE
}ret_code_of_mod;

/**
 * @brief enum for GPIO request return code of ARM BCM2836
 */
enum
{
	NO_ERR,
	GPIO_TO_REQ_ERR,
	GPIO_TO_IRQ_ERR,
	IRQ_TO_REQ_ERR
}gpio_err;

/**
 * @brief enum for detecting edges on GPIO
 */
enum 
{ 
	FALLING,
	RISING,
	BOTH_FALLING_RISING,
}edge_types;

/**
 * @brief function for print module parameters to dmesg log
 * @return none
 */
static void debug_when_init(void)
{
	#ifdef MODULE_DEBUG_MODE
	PINFO("Kernel version is: %s ",UTS_RELEASE);
	PINFO("Process ID is: %d",current->pid);
	PINFO("Module called with:%s ",current->comm);
	#endif
	return;
}

/**
 * @brief module settings.
 */
MODULE_ALIAS("interrupting");     //it will be module alias
MODULE_LICENSE("GPL");           //module license (must be define)
MODULE_AUTHOR("murat demirtas <muratdemirtastr@gmail.com>");   //module author
MODULE_DESCRIPTION("RPi GPIO interrupt with IRQ");  //module description

/**
 * @brief  irq handler cb function, will be fire when interrupt generated
 * @params irq number and device id of GPIO Controller
 * @return irqreturn_t 
 * @attention printk() should not be used for performance and stability
 */
static irqreturn_t r_irq_handler(int irq, void *dev_id)
{
	unsigned long interrupt_flag;
	long int count = 0;
	/**
 	 * @brief  clear and restore interrupt flags using local_irq_save
	 * and restore
 	*/
	
	local_irq_save(interrupt_flag);
	
	//ATTENTION//
	//PRINTK IS BLOCKING CALL  AND SHOULD BE NOT USED IN
	//REAL INTERRUPT APPLICATIONS
	
	printk(KERN_NOTICE "Rising edge detected,Interrupt! on  GPIO [%d]\n",
			BCM2836_HW_GPIO_PIN);

	//////////////////////////////////////////////////////////////////////
	////////////////////YOUR FUNCTIONS WILL COME HERE/////////////////////
	//////////////////////////////////////////////////////////////////////
	count  = count + 1;
	
	local_irq_restore(interrupt_flag);

        return IRQ_HANDLED;
}

/**
 * @brief  Setup BCM2836 GPIO Peripherals
 * @params none
 * @return GPIO request return code
 */
static int setup_gpio_interrupt(void)
{

	PDEBUG("Requesting BCM2836 Pin...\n");

	if(gpio_request(BCM2836_HW_GPIO_PIN,GPIO_IRQ_DESC))	{
		PERR("GPIO request failure for pin %d\n",BCM2836_HW_GPIO_PIN);
		return GPIO_TO_REQ_ERR;
	}

	IRQ_NUMBER_OF_GPIO = gpio_to_irq(BCM2836_HW_GPIO_PIN);

	if(IRQ_NUMBER_OF_GPIO < 0) {
		PERR("IRQ request failure for pin %d\n",BCM2836_HW_GPIO_PIN);
		return GPIO_TO_IRQ_ERR;
	}

	PDEBUG("IRQ mapped to gpio pin, IRQ no: %d\n",IRQ_NUMBER_OF_GPIO);

	if(request_irq(IRQ_NUMBER_OF_GPIO,(irq_handler_t) r_irq_handler,
			IRQF_TRIGGER_RISING, GPIO_PIN_DESC, GPIO_IRQ_DESC))
	{
		PERR("GPIO request failure for pin %d\n",BCM2836_HW_GPIO_PIN);
		return IRQ_TO_REQ_ERR;
	}

	PDEBUG("IRQ Request successfull:no %d\n",IRQ_NUMBER_OF_GPIO);

	return NO_ERR;
}

/**
 * @brief  for clearing IRQ request and GPIO settings
 * @params none
 * @return error code
 */
static int release_gpio_interrupt(void)
{

	PDEBUG("freeing irq and gpio pin\n");
	free_irq(IRQ_NUMBER_OF_GPIO,GPIO_IRQ_DESC);
	gpio_free(BCM2836_HW_GPIO_PIN);

	return NO_ERR;
}

/**
 * @brief  will fire when module removed
 * @params none
 * @return err code of module
 */
static int __init init_gpio_irq_fn(void)
{
	PINFO("GPIO IRQ module called,debug info:\n");
	debug_when_init();

	if(setup_gpio_interrupt() != NO_ERR)
		return GPIO_FAILURE;

	return MODULE_OK;
}

/**
 * @brief  will fire when module removed
 */
static void __exit exit_gpio_irq_fn(void)
{
	PINFO("GPIO IRQ module removing,IRQ no :%d\n",IRQ_NUMBER_OF_GPIO);
	release_gpio_interrupt();
	return;
}

/**
 * @brief  functions that will fire when module installed and removed
 */
module_init(init_gpio_irq_fn);
module_exit(exit_gpio_irq_fn);













