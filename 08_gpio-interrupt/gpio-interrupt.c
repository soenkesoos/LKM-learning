#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>

#define IO_LED 21
#define IO_BUTTON 20
#define IO_OFFSET 512

static struct gpio_desc *led;
static struct gpio_desc *button;

unsigned int irq_number;

static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	printk("Interrupt was triggered\n");
	return (irq_handler_t) IRQ_HANDLED;
}

static int init_gpio(void) {
	int status;

	led = gpio_to_desc(IO_LED+ IO_OFFSET);
	if(!led) {
		printk("gpioctrl - Error getting pin %d\n", IO_LED);
		return -1;
	}
	button = gpio_to_desc(IO_BUTTON+ IO_OFFSET);
	if(!led) {
		printk("gpioctrl - Error getting pin %d\n", IO_BUTTON);
		return -1;
	}
	status = gpiod_direction_input(button);
	if (status) {
		printk("gpioctrl - Error setting pin %d to input\n", IO_BUTTON);
		return -1;
	}
	return 0;
}

int myInit(void) {
	printk("Hello Kernel :)\n");

	if(init_gpio() == -1)
		return -1;

	gpiod_set_value(led, 1);

	/* Init of Interrupt*/
	irq_number = gpiod_to_irq(button);

	if(request_irq(irq_number, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, "my gpio irq", NULL) != 0) {
		printk("gpio_irq_handler could not be set\n");
	}
	
	return 0;
}

void myExit(void) {
	printk("Bye Kernel\n");
}

module_init(myInit);
module_exit(myExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes4Linux and Soenke");
MODULE_DESCRIPTION("test on gpio interrupts");


