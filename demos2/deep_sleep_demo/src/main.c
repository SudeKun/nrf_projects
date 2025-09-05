#include <zephyr/kernel.h>
#include <zephyr/pm/pm.h>
#include <hal/nrf_power.h>
#include <hal/nrf_gpio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>

#define SLEEP_TIME_MS 5000
#define BUTTON_NODE DT_ALIAS(sw0)
#define LED_NODE DT_ALIAS(led0)

// Timer for CPU idle wake-up
K_TIMER_DEFINE(wake_timer, NULL, NULL);

// Global GPIO device pointer
const struct device *gpio_dev;

void system_on_sleep(void)
{
        printk("\n1-System ON Sleep: sleeping for %d ms...\n", SLEEP_TIME_MS);
        
        // Turn off LED before sleep
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);
        
        k_sleep(K_MSEC(SLEEP_TIME_MS));
        
        // Turn on LED when waking up
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 1);
        printk("Woke up from System ON sleep - LED ON!\n");
        
        // Keep LED on for 2 seconds
        k_sleep(K_MSEC(2000));
        
        // Turn off LED
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);
}

void system_off_sleep(void)
{
        printk("\n4-System OFF Sleep: entering deep sleep. Press button to wake.\n");

        // Configure button pin (P0.11) as wake-up source
        // Enable SENSE for the button pin (low level detection)
        nrf_gpio_cfg_sense_set(11, NRF_GPIO_PIN_SENSE_LOW);

        // Small delay to ensure configuration is applied
        k_msleep(10);

        // Force system into SOFT_OFF (System OFF)
        nrf_power_system_off(NRF_POWER);

        // Should never reach here
        printk("ERROR: System OFF failed\n");
}

void system_on_sleep_with_cpu_idle(void)
{
        printk("\n2-CPU Idle Sleep: using k_cpu_idle() with timer interrupt...\n");
        
        // Turn off LED before sleep
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);

        // Use a timer to generate interrupts that will wake up k_cpu_idle()
        for (int i = 0; i < 5; i++)
        {
                printk("CPU idle cycle %d/5...\n", i + 1);

                // Schedule a timer to wake us up
                k_timer_start(&wake_timer, K_MSEC(1000), K_NO_WAIT);

                // Go to CPU idle - will wake up when timer expires
                k_cpu_idle();

                // Stop the timer
                k_timer_stop(&wake_timer);

                // Blink LED to show wake-up
                gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 1);  // LED ON
                k_msleep(200);
                gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);  // LED OFF
                
                printk("Woke up from CPU idle cycle %d - LED blinked!\n", i + 1);
        }

        printk("Completed CPU idle sleep cycles.\n");
}

void system_on_sleep_with_yield(void)
{
        printk("\n3-Thread Yield Sleep: using k_yield() and k_sleep()...\n");
        
        // Turn off LED before sleep
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);

        // Yield to other threads and then sleep
        k_yield();
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        // Turn on LED when waking up
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 1);
        printk("Woke up from yield sleep - LED ON!\n");
        
        // Keep LED on for 2 seconds
        k_sleep(K_MSEC(2000));
        
        // Turn off LED
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);
}

int main(void)
{
        printk("\n");
        gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

        if (!device_is_ready(gpio_dev))
        {
                printk("GPIO device not ready!\n");
                return 1;
        }

        gpio_pin_configure(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios),
                           GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(LED_NODE, gpios));
        gpio_pin_configure(gpio_dev, DT_GPIO_PIN(BUTTON_NODE, gpios),
                           GPIO_INPUT | DT_GPIO_FLAGS(BUTTON_NODE, gpios));

        // Initial LED state - turn it on briefly to show system is starting
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 1);  // LED ON
        k_sleep(K_MSEC(500));
        gpio_pin_set(gpio_dev, DT_GPIO_PIN(LED_NODE, gpios), 0);  // LED OFF

        printk("\n\n=== Power Mode Demo on nRF52840-DK ===\n");
        printk("Watch the LED to see wake-up indicators!\n");
        printk("System started - LED blinked to indicate boot/wake-up from System OFF\n");

        // Method 1: Standard k_sleep() (System ON sleep)
        system_on_sleep();
        k_sleep(K_SECONDS(1));

        // Method 2: CPU idle cycles
        system_on_sleep_with_cpu_idle();
        k_sleep(K_SECONDS(1));

        // Method 3: Thread yield + sleep
        system_on_sleep_with_yield();
        k_sleep(K_SECONDS(1));

        // Method 4: System OFF
        system_off_sleep();
}
