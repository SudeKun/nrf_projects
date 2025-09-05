#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>


#define WDT_FEED_TRIES 8
#define WDT_MAX_WINDOW  1000U
#define WDT_MIN_WINDOW  0U
#define WDG_FEED_INTERVAL 500U
#define WDT_OPT WDT_OPT_PAUSE_HALTED_BY_DBG
#define WDT_ALLOW_CALLBACK 0


int main(void)
{
	int err;
	int wdt_channel_id;
	const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));

	printk("Watchdog sample application\n");

	if (!device_is_ready(wdt)) {
		printk("%s: device not ready.\n", wdt->name);
		return 0;
	}

	struct wdt_timeout_cfg wdt_config = {
		/* Reset SoC when watchdog timer expires. */
		.flags = WDT_FLAG_RESET_SOC,

		/* Expire watchdog after max window */
		.window.min = WDT_MIN_WINDOW,
		.window.max = WDT_MAX_WINDOW,
	};

        wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
	if (wdt_channel_id == -ENOTSUP) {
		/* IWDG driver for STM32 doesn't support callback */
		printk("Callback support rejected, continuing anyway\n");
		wdt_config.callback = NULL;
		wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
	}
	if (wdt_channel_id < 0) {
		printk("Watchdog install error\n");
		return 0;
	}

	err = wdt_setup(wdt, WDT_OPT);
	if (err < 0) {
		printk("Watchdog setup error\n");
		return 0;
	}

        #if WDT_MIN_WINDOW != 0
	/* Wait opening window. */
	k_msleep(WDT_MIN_WINDOW);
        #endif
	/* Feeding watchdog. */
	printk("Feeding watchdog %d times\n", WDT_FEED_TRIES);
	for (int i = 0; i < WDT_FEED_TRIES; ++i) {
		printk("Feeding watchdog...\n");
		wdt_feed(wdt, wdt_channel_id);
		k_sleep(K_MSEC(WDG_FEED_INTERVAL));
	}

	/* Waiting for the SoC reset. */
	printk("Waiting for reset...\n");
	while (1) {
		k_yield();
	}
	return 0;
}
