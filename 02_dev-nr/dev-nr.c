#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("soenke");
MODULE_DESCRIPTION("test LKM for dev numbers and callback func");


/*
* @brief function called when fileop open is called
*/
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev-nr - open was called \n");
	return 0;
}

/*
* @brief function called when fileop release is called
*/
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev-nr - close was called \n");
	return 0;
}

/*
 * @brief struct declares file operations
 */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close	
};

#define MYMAJOR 90

int __init ModuleInit(void) {
	int retval;
	printk("Hello Kernel :)\n");
	// registering a device number
	retval = register_chrdev(MYMAJOR, "dev-nr", &fops);
	if(retval == 0) {
		printk("dev-nr - registerd MAJOR: %d MINOR: %d\n", MYMAJOR, 0);
	} else if(retval > 0) {
		printk("dev-nr - registerd MAJOR: %d MINOR: %d\n", retval>>20, retval&0xfffff);
	} else {
		printk("dev-nr could not be registered\n");
		return -1;
	}
	return 0;
}

void __exit ModuleExit(void) {
	unregister_chrdev(MYMAJOR, "dev-nr");
	printk("Bye Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);



