#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/version.h>

static int cmd_hello(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "Hello, Zephyr Shell!");
    return 0;
}

static int listall(const struct shell *shell, size_t argc, char **argv) {
    shell_print(shell, "=== Device Information ===");
    
    // System Information
    shell_print(shell, "\n--- System Info ---");
    shell_print(shell, "Board: %s", CONFIG_BOARD);
    shell_print(shell, "SoC: %s", CONFIG_SOC);
    shell_print(shell, "Zephyr Version: %s", KERNEL_VERSION_STRING);
    shell_print(shell, "Build Time: %s %s", __DATE__, __TIME__);
    
    // Runtime Information
    shell_print(shell, "\n--- Runtime Info ---");
    shell_print(shell, "Uptime: %lld ms", k_uptime_get());
    shell_print(shell, "CPU Frequency: %d Hz", sys_clock_hw_cycles_per_sec());

#ifdef CONFIG_NETWORKING
    shell_print(shell, "\n--- Network Info ---");
    shell_print(shell, "Networking: Enabled");
#ifdef CONFIG_MCUMGR_TRANSPORT_UDP
    shell_print(shell, "UDP Transport: Enabled (Port 1337)");
#endif
#else
    shell_print(shell, "\n--- Network Info ---");
    shell_print(shell, "Networking: Disabled");
#endif

    // MCUmgr Information
    shell_print(shell, "\n--- MCUmgr Info ---");
#ifdef CONFIG_MCUMGR_TRANSPORT_UART
    shell_print(shell, "UART Transport: Enabled");
#endif
#ifdef CONFIG_MCUMGR_TRANSPORT_BT
    shell_print(shell, "Bluetooth Transport: Enabled");
#endif

    // Hardware Information
    shell_print(shell, "\n--- Hardware Info ---");
    shell_print(shell, "Board: %s", CONFIG_BOARD);
    
    shell_print(shell, "\n=== End Device Info ===");
    return 0;
}

// Register shell commands
SHELL_CMD_REGISTER(hello, NULL, "Say hello", cmd_hello);
SHELL_CMD_REGISTER(listall, NULL, "List all device information", listall);
