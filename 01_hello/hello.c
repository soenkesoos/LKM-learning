#include <linux/module.h>
#include <linux/init.h>

int myInit(void) {
	printk("Hello Kernel :)\n");
	return 0;
}

void myExit(void) {
	printk("Bye Kernel\n");
}

module_init(myInit);
module_exit(myExit);

MODULE_LICENSE("GPL");


