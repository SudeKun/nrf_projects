#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

int main(void)
{
    const struct device *reg = DEVICE_DT_GET(DT_NODELABEL(led_reg));
    int ret;
    LOG_INF("Starting LED power regulator demo");

    // Check if regulator device is ready
    if (!device_is_ready(reg)) {
        LOG_ERR("Regulator device not ready");
        return -1;
    }

    while (1) {
        // Enable power regulator (turns on LED)
        LOG_INF("Enabling LED power regulator");
        ret = regulator_enable(reg);
        if (ret != 0) {
            LOG_ERR("Failed to enable regulator: %d", ret);
        } else {
            LOG_INF("LED power regulator enabled - LED should be ON");
            
            // Keep LED on for 2 seconds
            k_sleep(K_SECONDS(2));
        }
        
        // Disable power regulator (turns off LED)
        LOG_INF("Disabling LED power regulator");
        ret = regulator_disable(reg);
        if (ret != 0) {
            LOG_ERR("Failed to disable regulator: %d", ret);
        } else {
            LOG_INF("LED power regulator disabled - LED should be OFF");
        }
        
        // Wait 2 seconds before next cycle
        k_sleep(K_SECONDS(2));
    }
}
