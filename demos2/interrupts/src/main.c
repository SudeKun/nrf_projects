#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/irq.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define SLEEP_TIME_MS 5000
#define TIMER_INTERVAL_MS 3000

/* GPIO pins for nRF52840-DK */
#define BUTTON_PIN 11  /* Button 1 on nRF52840-DK */

/* Interrupt counters */
static uint32_t button_interrupt_count = 0;
static uint32_t timer_interrupt_count = 0;
static uint32_t total_interrupt_count = 0;

/* Device handles */
const struct device *gpio_dev;
static struct gpio_callback button_cb_data;
static struct k_timer demo_timer;

/* Timer interrupt handler */
void timer_expiry_function(struct k_timer *timer) {
    timer_interrupt_count++;
    total_interrupt_count++;
    
    printk("Timer interrupt fired! (Count: %u)\n", 
           timer_interrupt_count);
}

/* Setup timer interrupt */
void setup_timer(void) {
    printk("Setting up timer interrupt...\n");
    k_timer_init(&demo_timer, timer_expiry_function, NULL);
    k_timer_start(&demo_timer, K_MSEC(TIMER_INTERVAL_MS), K_MSEC(TIMER_INTERVAL_MS));
    printk("Timer setup complete (interval: %d ms)\n", TIMER_INTERVAL_MS);
}


/* Button interrupt handler */
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    button_interrupt_count++;
    total_interrupt_count++;
    printk("Button interrupt! (Count: %u)\n", button_interrupt_count);
}

/* Setup button interrupt */
void setup_button(void) {
    printk("Setting up button interrupt...\n");
    
    gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    if (!device_is_ready(gpio_dev)) {
        printk("Error: GPIO device not ready\n");
        return;
    }

    /* Configure button pin as input with pull-up */
    int ret = gpio_pin_configure(gpio_dev, BUTTON_PIN, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("Error configuring button pin: %d\n", ret);
        return;
    }

    /* Configure button interrupt on falling edge (button press) */
    ret = gpio_pin_interrupt_configure(gpio_dev, BUTTON_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("Error configuring button interrupt: %d\n", ret);
        return;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(BUTTON_PIN));
    
    ret = gpio_add_callback(gpio_dev, &button_cb_data);
    if (ret < 0) {
        printk("Error adding button callback: %d\n", ret);
        return;
    }

    printk("Button interrupt setup complete (Pin %d)\n", BUTTON_PIN);
}

struct irq_name {
    IRQn_Type irq;
    const char *name;
};

static const struct irq_name irq_names[] = {
    {POWER_CLOCK_IRQn, "POWER_CLOCK"},
    {RADIO_IRQn,       "RADIO"},
    {UARTE0_UART0_IRQn,"UARTE0_UART0"},
    {GPIOTE_IRQn,      "GPIOTE"},
    {TIMER0_IRQn,      "TIMER0"},
    {TIMER1_IRQn,      "TIMER1"},
    {TIMER2_IRQn,      "TIMER2"},
    {RTC0_IRQn,        "RTC0"},
    {TEMP_IRQn,        "TEMP"},
    {RNG_IRQn,         "RNG"},
    {SAADC_IRQn,       "SAADC"},
    {FPU_IRQn,         "FPU"},
};


/* Print interrupt statistics */
void print_interrupt_stats(void) {
    printk("\n=== Interrupt Statistics ===\n");
    printk("Total interrupts fired: %u\n", total_interrupt_count);
    
    if (total_interrupt_count == 0) {
        printk("  No interrupts have fired yet\n");
    } else {
        printk("  Button interrupts: %u\n", button_interrupt_count);
        printk("  Timer interrupts: %u\n", timer_interrupt_count);
    }
    
    uint32_t active_types = 0;
    if (button_interrupt_count > 0) active_types++;
    if (timer_interrupt_count > 0) active_types++;
    
    printk("Active interrupt types: %u\n", active_types);
    printk("Statistics check complete\n");
    printk("About to sleep for %d ms...\n", SLEEP_TIME_MS);
}

/* Check for pending interrupts */
void print_pending_interrupts(void) {
    printk("Pending interrupts:\n");
    bool found_pending = false;
    
    for (int i = 0; i < ARRAY_SIZE(irq_names); i++) {
        IRQn_Type irq = irq_names[i].irq;
        if (NVIC_GetPendingIRQ(irq)) {
            printk("  [%2d] %s is pending\n", irq, irq_names[i].name);
            found_pending = true;
        }
    }
    
    if (!found_pending) {
        printk("  No interrupts pending\n");
    }
}

/* Check for active interrupts */
void print_active_interrupts(void) {
    printk("Active interrupts:\n");
    bool found_active = false;
    
    for (int i = 0; i < ARRAY_SIZE(irq_names); i++) {
        IRQn_Type irq = irq_names[i].irq;
        if (NVIC_GetActive(irq)) {
            printk("  [%2d] %s is active\n", irq, irq_names[i].name);
            found_active = true;
        }
    }
    
    if (!found_active) {
        printk("No interrupts active\n");
    }
}

void print_active_interrupts(void) {
    printk("Active interrupts:\n");
    for (int i = 0; i < ARRAY_SIZE(irq_names); i++) {
        if (NVIC_GetActive(irq_names[i].irq)) {
            printk("  [%2d] %s is active\n", irq_names[i].irq, irq_names[i].name);
        }
    }
}

int main(void) {
    printk("Interrupts on nRF52840-DK\n");
    printk("=====================================\n");

    /* Setup GPIO and interrupts */
    setup_button();
    setup_timer();

    printk("\nDemo Instructions:\n");
    printk("- Press Button 1 (GPIO pin %d) to trigger GPIO interrupts\n", BUTTON_PIN);
    printk("- Timer will automatically fire every %d seconds and toggle LED\n", TIMER_INTERVAL_MS/1000);
    printk("- Statistics will be printed every %d seconds\n\n", SLEEP_TIME_MS/1000);

    /* Main loop */
    while (1) {
        print_interrupt_stats();
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}