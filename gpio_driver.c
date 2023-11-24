#include <linux/module.h>  
#include <linux/printk.h>  
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/**
 * @brief Driver constants
 *
 * These constants define parameters for the GPIO driver.
 */
#define	DEVICE_NAME	"my_gpio_device"
#define DRIVER_NAME	"my_gpio_driver"
#define DRIVER_CLASS	"my_gpio_class"
#define GPIO_PIN	20
#define	LED_ON		1
#define	LED_OFF		0

/**
 * @brief Driver state structure
 *
 * This structure holds the state of the GPIO driver, including device number,
 * character device structure, GPIO class, and LED status.
 */
static struct {
	dev_t my_gpio_dev;
	struct cdev my_gpio_dev;
	struct class *my_gpio_class;
	unsigned int led_status;
} led_module_st;

/**
 * @brief Set the LED status.
 *
 * This function sets the GPIO pin to the specified status.
 *
 * @param status The desired LED status (LED_ON or LED_OFF).
 */
static void led_driver_setled(unsigned int status) {
	led_module_st.led_status = status;
	gpio_set_value(GPIO_PIN, status);
}

/**
 * @brief Read LED status.
 *
 * This function reads the current LED status and provides it to the user.
 */
static ssize_t driver_read(struct file *file, char __user *buffer, size_t count, loff_t *offs) {
	int size;
	static const char * const led_state[] = { "OFF\n", "ON\n"};

	size = strlen(led_state[led_module_st.led_status]);
	if (size > count)
		size = count;

	if (copy_to_user(buffer, led_state[led_module_st.led_status], size))
		return -EFAULT;
	
	return size;
}

/**
 * @brief Write to LED.
 *
 * This function writes a character from the user to control the LED.
 */
static ssize_t driver_write(struct file *File, const char __user *buffer, size_t count, loff_t *offs) {
	
	char write_value = 0;

	if (copy_from_user(&write_value, buffer, 1))
		return -EFAULT;
	
	switch (write_value) {
		case '0': 
			led_driver_setled(LED_OFF);
			pr_info("LED OFF!\n");
			break;			
		case '1':
			led_driver_setled(LED_ON);
			pr_info("LED ON!\n");
			break;
		default:
			pr_err("Invalide Input\n");
			break;
	}

	return count;
}

static int driver_open(struct inode *device_file, struct file *instance) {
	pr_info("Device opened: %s!\n", DEVICE_NAME);
	return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
	pr_info("Device closed: %s\n", DEVICE_NAME);
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/**
 * @brief Initialize the GPIO driver module.
 *
 * This function initializes the GPIO driver, allocating resources and registering with the kernel.
 *
 * @return 0 on success, a negative error code on failure.
 */
static int __init initmodule(void) { 
	// Allocate a device number for the GPIO device
	if( alloc_chrdev_region(&led_module_st.my_gpio_dev, 0 , 1, DRIVER_NAME) < 0) {
		pr_err("Failed to allocate device number!\n");
		return -1;
	}

	// Create a character device class for the GPIO device
	if((led_module_st.my_gpio_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_err("Failed to create the character device class!\n");
		goto ClassError;
	}
	
	// Create the device file for the GPIO device
	if(device_create(led_module_st.my_gpio_class, NULL, led_module_st.my_gpio_dev, NULL, DRIVER_NAME) == NULL) {
		pr_err("Failed to create the character device file!\n");
		goto FileError;
	}

	// Initialize the character device and add it to /dev
	cdev_init(&led_module_st.my_gpio_dev, &fops);
	if(cdev_add(&led_module_st.my_gpio_dev, led_module_st.my_gpio_dev, 1) == -1) {
		pr_err("Failed to register the character device to /dev!\n");
		goto AddError;
	}
	
	// Request GPIO pin for the LED
	if(gpio_request(GPIO_PIN,"rpi_gpio_20")) {
		pr_err("unable to initialise gpio 20\n");
		goto AddError;
	}
	
	// Set GPIO pin 20 as an output
	if(gpio_direction_output(GPIO_PIN,0)) {
		pr_err("unable to set gpio 20 as output\n");
		goto Gpio20Error;
	}
	
	led_driver_setled(LED_OFF);
	return 0;

Gpio20Error:
	gpio_free(GPIO_PIN);
AddError:
	device_destroy(led_module_st.my_gpio_class, led_module_st.my_gpio_dev);
FileError:
	class_destroy(led_module_st.my_gpio_class);
ClassError:
	unregister_chrdev_region(led_module_st.my_gpio_dev, 1);
	return -1;

} 

static void __exit exitmodule(void) {
	gpio_set_value(GPIO_PIN,0);
	gpio_free(GPIO_PIN);
	cdev_del(&led_module_st.my_gpio_dev);
	device_destroy(led_module_st.my_gpio_class, led_module_st.my_gpio_dev);
	class_destroy(led_module_st.my_gpio_class);
	unregister_chrdev_region(led_module_st.my_gpio_dev, 1);
}

module_init(initmodule);
module_exit(exitmodule);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timo Rosenberg");
MODULE_DESCRIPTION("GPIO Driver for Raspberry Pi 3b");
