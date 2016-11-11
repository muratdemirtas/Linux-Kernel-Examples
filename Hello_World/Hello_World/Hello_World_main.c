 //Hello World Example 

//include needed libraries for building and printk() function
#include  <linux/module.h>
#include  <linux/init.h>

//start function for loading our module
int init_module(void)
{
	//kernel message, you can see with dmesg command
	printk(KERN_INFO "MOD: HELLO WORLD EXAMPLE LOADED.\n");	
	//return 0 (success) if module was loaded correctly, a non 0 return means module failed.
	return 0;
}

//this function will execute when you remove this module from kernel.
void cleanup_module(void)
{
	//print debug message to kernel.
	printk(KERN_INFO "MOD: WORLD EXAMPLE LEAVING.\n");
}

