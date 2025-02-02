#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux, soenke");
MODULE_DESCRIPTION("An example for using GPIOs without the device tree");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "gpio-driver"
#define DRIVER_CLASS "MyModuleClass"


static struct gpio_desc *led, *button;

#define IO_LED 21
#define IO_BUTTON 20

#define IO_OFFSET 512
/*
 * @brief Read state of IO_BUTTON and copy string to userspace
 * @return difference between requested amount to read and actually read amount
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char tmp[3] = " \n";

	to_copy = min(count, sizeof(tmp));

	/* read Button-value */
	tmp[0] = gpiod_get_value(button) + '0';
	printk("gpio-driver: Button=%c", tmp[0]);

	/* copy data to userspace */
	not_copied = copy_to_user(user_buffer, tmp, to_copy);
	delta = to_copy - not_copied;
	
	return delta;
}

/*
 * @biref Write state that is copied form userspace to gpio_state
 * @return difference between requested amount to write and actually written amount 
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy*/
	to_copy = min(count, sizeof(value));

	/* copy data from userspace */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	delta = to_copy - not_copied;

	switch (value) {
	case '0':
		gpiod_set_value(led, 0);
		break;
	case '1':
		gpiod_set_value(led, 1);
		break;	
	default:
		printk("gpio-driver: tried to write unreqcognized gpio-state %c", value);
		break;
	}

	return delta;

		
}

static int driver_open(struct inode *device_file, struct file *instance) {
	printk("gpio-driver: open was called\n");
	return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
	printk("gpio-driver: close was called\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int init_gpio(void) {
	int status;
	
	led = gpio_to_desc(IO_LED + IO_OFFSET);
	if (!led) {
		printk("gpioctrl - Error getting pin %d\n", IO_LED);
		return -1;
	}

	button = gpio_to_desc(IO_BUTTON + IO_OFFSET);
	if (!button) {
		printk("gpioctrl - Error getting pin %d\n", IO_BUTTON);
		return -1;
	}

	status = gpiod_direction_output(led, 0);
	if (status) {
		printk("gpioctrl - Error setting pin 20 to output\n");
		return -1;
	}

	status = gpiod_direction_input(button);
	if (status) {
		printk("gpioctrl - Error setting pin 21 to input\n");
		return -1;
	}
	return 0;
}

static int __init my_init(void)
{

	printk("gpio-driver: Hello Kernel :)\n");

	/* Allocate device nr */
	if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocate\n");
		return -1;
	}
	printk("gpio-driver: Device Nr. Major=%d, Minor=%d", my_device_nr>>20, my_device_nr&0xfffff);

	/* create device class */
	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created\n");
		goto ClassError;
	}
	
	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Cannot create device file!\n");
		goto FileError;
	}

	/* initialize device file */
	cdev_init(&my_device, &fops);

	/* Registering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed\n");
		goto AddError;
	}

	if(init_gpio() == -1) {
		goto GpioError;
	}
	gpiod_set_value(led, 1);
	printk("gpioctrl - Button is %spressed\n", gpiod_get_value(button) ? "" : "not ");

	return 0;

GpioError:
	;
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	
	return -1;
}

static void __exit my_exit(void)
{
	gpiod_set_value(led, 0);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("gpio-driver: Bye Kernel :)\n");
}

module_init(my_init);
module_exit(my_exit);


