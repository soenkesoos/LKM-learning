#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio/consumer.h>

#define IO_LED 21
#define IO_OFFSET 512

static struct gpio_desc *led;
static struct timer_list my_timer;

void timer_callback(struct timer_list *data) {
	gpiod_set_value(led, 0);	// Turn LED back off
	printk("led: off\n");
}

static int init_gpio(void) {
	int status;

	led = gpio_to_desc(IO_LED + IO_OFFSET);
	if(!led) {
		printk("gpioctrl - Error getting pin %d\n", IO_LED);
		return -1;
	}
	status = gpiod_direction_output(led, 0);
	if (status) {
		printk("gpioctrl - Error setting pin %d to output\n", IO_LED);
		return -1;
	}
	return 0;
}

int myInit(void) {
	printk("Hello Kernel :)\n");

	if(init_gpio() == -1)
		return -1;

	printk("led: on");
	gpiod_set_value(led, 1);

	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	printk("led: still on\n");	// call back was only scheduled

	return 0;
}

void myExit(void) {
	printk("Bye Kernel\n");
}

module_init(myInit);
module_exit(myExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes4Linux and Soenke");
MODULE_DESCRIPTION("small test on Timers");


