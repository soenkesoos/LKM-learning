#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/gpio/consumer.h>

#define IO_LED 21
#define IO_OFFSET 512

static struct gpio_desc *led;
static struct hrtimer my_hrtimer;

u64 start_t;

static enum hrtimer_restart test_hrtimer_callback(struct hrtimer *timer) {
	u64 now_t = jiffies;
	printk("now_t - start_t = %u\n", jiffies_to_msecs(now_t - start_t));
	gpiod_set_value(led, 0);
	return HRTIMER_NORESTART;
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

	/* Init of hrtimer */
	hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	my_hrtimer.function = &test_hrtimer_callback;
	start_t = jiffies;
	hrtimer_start(&my_hrtimer, ms_to_ktime(100), HRTIMER_MODE_REL);
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


