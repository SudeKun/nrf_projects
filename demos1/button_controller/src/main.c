#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

//Controlling LED through a button press
//This code uses the Zephyr RTOS to read a button state and control an LED accordingly

#define button1	DT_ALIAS(sw0)
#define led1	DT_ALIAS(led0)
#define SLEEP_TIME_MS 100

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(button1, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(led1, gpios);


int main(void)
{
        int ret;
        if (!device_is_ready(led1.port)) {
                return -1;
        }
        if (!device_is_ready(button1.port)) {
                return -1;
        }
        ret= gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
                return -1;
        }
        ret= gpio_pin_configure_dt(&button1, GPIO_INPUT);
        if (ret < 0) {
                return -1;
        }
        while (1){
                bool val= gpio_pin_get_dt(&button1);
                if (val){
                        printk("Button pressed\n");
                }
                gpio_pin_set_dt(&led1,val);
                k_msleep(SLEEP_TIME_MS);
        }
        return 0;
}

/*
#define SLEEP_TIME_MS 100
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#define SW1_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static struct gpio_callback button_cb_data;
static struct gpio_callback button1_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        gpio_pin_toggle_dt(&led);
}
void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        gpio_pin_toggle_dt(&led1);
}

int main(void){
        int ret,ret1;
        if (!device_is_ready(led.port)) return -1;
        if (!device_is_ready(button.port)) return -1;
        if (!device_is_ready(led1.port)) return -1;
        if (!device_is_ready(button1.port)) return -1;

        // Configure the LED as output and set it to active state
        ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
        if (ret < 0) return -1;
        ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
        if (ret < 0) return -1;
        ret1 = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        if (ret1 < 0) return -1;
        ret1 = gpio_pin_configure_dt(&button1, GPIO_INPUT);
        if (ret1 < 0) return -1;

        // Configure the button to trigger an interrupt on rising edge
        ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
        if (ret < 0) return -1;
        ret1 = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
        if (ret1 < 0) return -1;

        // Initialize the GPIO callback
        gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
        gpio_add_callback(button.port, &button_cb_data);
        // Initialize the GPIO callback for button1
        gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
        gpio_add_callback(button1.port, &button1_cb_data);

        while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}
*/
