#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    printk("\n========================================\n");
    printk("=== MCUboot -> Application Transition ===\n");
    printk("*** Application started ***\n");
    printk("Current uptime: %lld ms\n", k_uptime_get());
    printk("MCUboot successfully handed over control to application\n");
    printk("If uptime is very small, MCUboot completed very quickly\n");
    printk("Expected: Should see 5+ second delay if MCUboot config worked\n");
    printk("========================================\n\n");
    
    LOG_INF("Application main() started at uptime: %lld ms", k_uptime_get());
    LOG_INF("MCUboot -> Application transition complete");
    
    while (1) {
        LOG_INF("Application running... uptime: %lld ms", k_uptime_get());
        k_sleep(K_SECONDS(5));
    }
    
    return 0;
}
