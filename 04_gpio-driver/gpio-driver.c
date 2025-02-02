#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>

static struct gpio_desc *led, *button;

#define IO_LED 21
#define IO_BUTTON 20

#define IO_OFFSET 512

static int __init my_init(void)
{

	printk("gpio-driver: Hello Kernel :)\n");

	int status;

	led = gpio_to_desc(IO_LED + IO_OFFSET);
	if (!led) {
		printk("gpioctrl - Error getting pin %d\n", IO_LED);
		return -ENODEV;
	}

	button = gpio_to_desc(IO_BUTTON + IO_OFFSET);
	if (!button) {
		printk("gpioctrl - Error getting pin %d\n", IO_BUTTON);
		return -ENODEV;
	}

	status = gpiod_direction_output(led, 0);
	if (status) {
		printk("gpioctrl - Error setting pin 20 to output\n");
		return status;
	}

	status = gpiod_direction_input(button);
	if (status) {
		printk("gpioctrl - Error setting pin 21 to input\n");
		return status;
	}

	gpiod_set_value(led, 1);

	printk("gpioctrl - Button is %spressed\n", gpiod_get_value(button) ? "" : "not ");

	return 0;
}

static void __exit my_exit(void)
{
	gpiod_set_value(led, 0);
	printk("gpio-driver: Bye Kernel :)\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux, soenke");
MODULE_DESCRIPTION("An example for using GPIOs without the device tree");
