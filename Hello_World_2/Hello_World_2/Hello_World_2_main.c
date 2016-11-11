//Hello World Example With Macros

//include header files for required functions
#include <linux/init.h>
#include <linux/module.h>

//Define Example Module infos
#define AUTHOR "MURAT DEMIRTAS <muratdemirtastr@gmail.com>"
#define DESCRIPTION "EXAMPLES FOR KERNEL"
#define LICENSE "GPL"

//register infos to module
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);

//_init macro used for this example.(init_module()), load module
static int __init Hello_World_2_init(void)
{
	//print message  
	printk(KERN_INFO "MOD: HELLO WORLD EXAMPLE 2\n");
	return 0;
}

//exit module and print message
static void __exit Hello_World_2_exit(void)
{
	printk("MOD: HELLO WORLD 2 EXITING, BYE.\n");
}

//register functions.
module_init(Hello_World_2_init);
module_exit(Hello_World_2_exit);
