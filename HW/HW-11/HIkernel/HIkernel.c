#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

static int _init_(void)
{
	printk(KERN_EMERG"Hi, kernel!\n");
	return 0;
}

static void _exit_(void)
{
	printk(KERN_EMERG"Bye, kernel!\n");
}

module_init(_init_);
module_exit(_exit_);