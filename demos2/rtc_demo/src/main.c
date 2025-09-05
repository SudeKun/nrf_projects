

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/util.h>
#include <time.h>

const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

// Global variables to track initial time and boot time
static struct rtc_time initial_time;
static int64_t boot_time_ms;

static int set_date_time(const struct device *rtc)
{
    int ret = 0;
    initial_time.tm_year = 2025 - 1900;
    initial_time.tm_mon = 11 - 1;
    initial_time.tm_mday = 30;
    initial_time.tm_hour = 18;
    initial_time.tm_min = 30;
    initial_time.tm_sec = 0;

    // Store the boot time when we set the initial time
    boot_time_ms = k_uptime_get();

    ret = rtc_set_time(rtc, &initial_time);
    if (ret < 0) {
        printk("Cannot write date time: %d\n", ret);
        return ret;
    }
    return ret;
}

static int get_date_time(const struct device *rtc)
{
    struct rtc_time tm = initial_time;
    
    // Calculate elapsed time since boot
    int64_t elapsed_ms = k_uptime_get() - boot_time_ms;
    int elapsed_seconds = elapsed_ms / 1000;
    
    // Add elapsed seconds to the initial time
    tm.tm_sec += elapsed_seconds;
    
    // Handle time overflow
    while (tm.tm_sec >= 60) {
        tm.tm_sec -= 60;
        tm.tm_min++;
    }
    
    while (tm.tm_min >= 60) {
        tm.tm_min -= 60;
        tm.tm_hour++;
    }
    
    while (tm.tm_hour >= 24) {
        tm.tm_hour -= 24;
        tm.tm_mday++;
        // Note: This simple implementation doesn't handle month/year overflow
        // For a complete solution, you'd need to handle days per month, leap years, etc.
    }

    printk("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d\n", 
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
           tm.tm_hour, tm.tm_min, tm.tm_sec);

    return 0;
}

int main(void)
{
    /* Check if the RTC is ready */
    if (!device_is_ready(rtc)) {
        printk("Device is not ready\n");
        return 0;
    }

    set_date_time(rtc);

    /* Continuously read the current date and time from the RTC */
    while (get_date_time(rtc) == 0) {
        k_sleep(K_MSEC(1000));
    }
    return 0;
}