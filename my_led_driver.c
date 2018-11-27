#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/delay.h>

/*
 * The name for our device, as it will appear in /proc/devices
 */
#define DEVICE_NAME "my_leddriver"
static int my_leddriver_probe(struct platform_device *pdev)
{
    int i=0;
	struct pinctrl *pinctrl;
    
    pinctrl = devm_pinctrl_get_select_default(&pdev->dev);
    if (IS_ERR(pinctrl)) {
        dev_warn(&pdev->dev, "unable to select pin group\n");
        return 1;
    }
        
    dev_info(&pdev->dev, "That seemed to have worked\n");
    gpio_request(26,"gpio26");
    gpio_direction_output(26,1);
    gpio_export(26,0);
    gpio_request(27,"gpio27");
        gpio_direction_output(27,1);
    gpio_export(27,0);
while(i < 20)    
{
	gpio_set_value(26,1);
    msleep(200);
    gpio_set_value(26,0);
    msleep(200);
    gpio_set_value(27,1);
    msleep(200);
    gpio_set_value(27,0);
    msleep(200);
	i++;
}
    return 0;
}

static int my_leddriver_remove(struct platform_device *pdev)
{
	return 0;
}

/*
 * Initialize the driver
 */
 
#ifdef CONFIG_OF
static const struct of_device_id my_leddriver_dt_ids[] = {
    { .compatible = "led test", },
    { }
};
#endif

static struct platform_driver my_leddriver_driver = {
    .driver = {
        .name = "my_leddriver",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(my_leddriver_dt_ids),
    },

    .probe = my_leddriver_probe,
    .remove = my_leddriver_remove,
};

/* For simple modules that contain a single platform driver this replaces the
 * module_init and module_exit boilerplate - see http://lwn.net/Articles/461644/
 */
module_platform_driver(my_leddriver_driver);

MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Simple MUX setting device-tree based driver");
