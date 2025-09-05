#include <zephyr/kernel.h>
#include <zephyr/stats/stats.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>

//Zephyr’s Statistics Management (Stats subsystem) is a lightweight way to gather runtime counters, 
//which is especially useful in embedded systems for diagnostics and performance monitoring.

//You can make a shell-accessible version or to integrate it with "mcumgr".

/* --- NETWORK STATS --- */
STATS_SECT_START(net_stats)
    STATS_SECT_ENTRY(tx_packets)
    STATS_SECT_ENTRY(rx_packets)
STATS_SECT_END;

STATS_SECT_DECL(net_stats) net_stats;

/* --- POWER STATS --- */
STATS_SECT_START(pwr_stats)
    STATS_SECT_ENTRY(power_cycles)
    STATS_SECT_ENTRY(voltage_drops)
STATS_SECT_END;

STATS_SECT_DECL(pwr_stats) pwr_stats;

int main(void)
{
    int rc;

    /* Initialize & register net_stats */
    rc = STATS_INIT_AND_REG(net_stats, STATS_SIZE_32, "net_stats");
    __ASSERT(rc == 0, "Failed to register net_stats");

    /* Initialize & register pwr_stats */
    rc = STATS_INIT_AND_REG(pwr_stats, STATS_SIZE_32, "pwr_stats");
    __ASSERT(rc == 0, "Failed to register pwr_stats");

    /* Use the stat counters */
    STATS_INCN(net_stats, tx_packets, 10);
    STATS_INCN(net_stats, rx_packets, 5);

    STATS_INC(pwr_stats, power_cycles);
    STATS_INCN(pwr_stats, voltage_drops, 2);

    /* Print the current values */
    printk("=== Network Stats ===\n");
    printk("TX packets: %u\n", net_stats.tx_packets);
    printk("RX packets: %u\n", net_stats.rx_packets);

    printk("=== Power Stats ===\n");
    printk("Power cycles: %u\n", pwr_stats.power_cycles);
    printk("Voltage drops: %u\n", pwr_stats.voltage_drops);

    k_sleep(K_SECONDS(2));

    STATS_INCN(pwr_stats, voltage_drops, 2);

    /* Print the current values */
    printk("=== Network Stats ===\n");
    printk("TX packets: %u\n", net_stats.tx_packets);
    printk("RX packets: %u\n", net_stats.rx_packets);

    printk("=== Power Stats ===\n");
    printk("Power cycles: %u\n", pwr_stats.power_cycles);
    printk("Voltage drops: %u\n", pwr_stats.voltage_drops);
}