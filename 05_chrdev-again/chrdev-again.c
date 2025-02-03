#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_AUTHOR("Johannes4Linux, soenkesoos");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Different way to initialize a character device");

static int major;
static char buff[128];

static ssize_t chrdev_read (struct file *file, char __user *user_buffer, size_t requested, loff_t *pos) {
	printk("chrdev-again: chrdev_read() executed\n");
	char text[10] = "abcdefhi\n";
	int to_copy = min(requested, sizeof(text)), not_copied, delta;
	printk("copying '%s' to userspace so it is read by user\n", text);

	not_copied = copy_to_user(user_buffer, text, to_copy);

	delta = to_copy - not_copied;
	return delta;
}

static ssize_t chrdev_write (struct file *file, const char __user *user_buffer, size_t to_write, loff_t *pos) {
	printk("chrdev-again: chrdev_write() executed\n");
	int to_copy = min(to_write, 127), not_copied, delta;
	buff[to_copy] = '\0';

	not_copied = copy_from_user(buff, user_buffer, to_copy);
	
	delta = to_copy - not_copied;
	buff[delta] = '\0';

	printk("chrdev-again: %s\n", buff);
	return 0;
	
}

int chrdev_open (struct inode *node, struct file *file) {
	printk("chrdev-again: character device file was opened");
	return 0;
}

int chrdev_release (struct inode *node, struct file *file) {
	printk("chrdev-again: character device file was released");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = chrdev_read,
	.write = chrdev_write,
	.open = chrdev_open,
	.release = chrdev_release
};

static int __init my_init(void) {
	printk("chrdev-again: Hello Kernel :)\n");

	major = register_chrdev(0, "chrdev-again", &fops);
	if(major < 0) {
		printk("chrdev-again: Error registering chrdev\n");
		return major;
	}
	printk("chrdev-again: Major Device Number = %d\n", major);

	return 0;
}

static void __exit my_exit(void) {
	unregister_chrdev(major, "chrdev-again");
	printk("chrdev-again: Bye Kernel :(\n");
}

module_init(my_init);
module_exit(my_exit);


