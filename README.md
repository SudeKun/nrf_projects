# nRF Connect SDK Projects

This repository contains a comprehensive collection of nRF Connect SDK demonstrations and learning projects, covering various aspects of Nordic Semiconductor nRF series development.

## Overview

This workspace is organized into three main categories:
- **demos1/**: Basic Zephyr RTOS feature demonstrations
- **demos2/**: Advanced features and protocols

## Project Structure

### 📁 demos1/ - Basic Zephyr Features

| Project | Description | Key Features |
|---------|-------------|--------------|
| `button_controller/` | Button input handling | GPIO input, interrupt handling |
| `led_PWM/` | PWM LED control | PWM API, LED brightness control |
| `littlefs/` | LittleFS file system | File operations, persistent storage |
| `nvs/` | Non-Volatile Storage | Key-value storage, configuration persistence |
| `pwr_reg/` | Power Regulator Control | Power management, regulator on/off with LED signaling |

### 📁 demos2/ - Advanced Features

| Project | Description | Key Features |
|---------|-------------|--------------|
| `cdc_acm_demo/` | USB CDC ACM Communication | USB CDC, serial communication |
| `deep_sleep_demo/` | Deep Sleep Implementation | Power optimization, sleep modes |
| `interrupts/` | Interrupt Handling | GPIO interrupts, interrupt service routines |
| `MCUboot/` | MCUboot Bootloader | Secure bootloader, firmware updates |
| `rtc_demo/` | Real-Time Clock | RTC API, time keeping |
| `settings_demo/` | Settings Management | Configuration storage and retrieval |
| `smp_server_demo/` | SMP Server Protocol | Device management, remote configuration |
| `state_demo/` | State Machine | State management patterns |
| `statistics_mng_demo/` | Statistics Management | System monitoring, data collection |
| `watchdog/` | Watchdog Timer | System reliability, watchdog implementation |

## Prerequisites

- **nRF Connect SDK**: Latest version
- **nRF Connect for Desktop**: For device programming and debugging
- **Hardware**: nRF52840 Development Kit (or compatible nRF series board)
- **VS Code**: With nRF Connect extension pack

## Getting Started

1. **Setup Development Environment**
   ```bash
   # Install nRF Connect SDK following Nordic's official guide
   # https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation.html
   ```

2. **Clone Repository**
   ```bash
   git clone <repository-url>
   cd nrf_projects
   ```

3. **Build a Project**
   ```bash
   # Navigate to any project directory
   cd demos1/button_controller
   
   # Build the project
   west build -b nrf52840dk_nrf52840
   
   # Flash to device
   west flash
   ```

## Configuration Files

Each project contains standard Zephyr configuration files:

- **`prj.conf`**: Main application configuration
- **`CMakeLists.txt`**: Build system configuration
- **`Kconfig`**: Custom configuration options (where applicable)
- **`boards/`**: Hardware-specific configurations
- **`*.overlay`**: Device tree overlays for hardware customization

## Key Learning Topics

### Basic Concepts
- ✅ GPIO Control (buttons, LEDs)
- ✅ PWM (Pulse Width Modulation)
- ✅ File Systems (LittleFS)
- ✅ Non-Volatile Storage (NVS)
- ✅ Power Management

### Advanced Topics
- ✅ USB Communication (CDC ACM)
- ✅ Bluetooth Low Energy
- ✅ MCUboot Secure Bootloader
- ✅ Sleep Modes and Power Optimization
- ✅ Real-Time Clock
- ✅ System Management Protocol (SMP)
- ✅ Interrupt Handling
- ✅ Watchdog Timer
- ✅ State Machines
- ✅ Statistics Management

## Hardware Requirements

Projects are designed for the **nRF52840 Development Kit**

## Project Notes

Refer to `notes.txt` for detailed learning notes and project progression. Key insights include:

- Configuration file hierarchy and usage
- Hardware abstraction layer concepts
- Driver implementation patterns
- Power management strategies

## Building and Flashing

### Standard Build Process
```bash
# Build for nRF52840 DK
west build -b nrf52840dk_nrf52840

# Clean build
west build -b nrf52840dk_nrf52840 --pristine

# Flash to device
west flash

# Monitor serial output
west serial-term
```

### Multi-Image Builds
Some projects (like MCUboot) require multi-image builds:
```bash
west build -b nrf52840dk_nrf52840 --sysbuild
```

## Troubleshooting

### Common Issues
1. **Build Errors**: Ensure nRF Connect SDK is properly installed and `west` is in PATH
2. **Flash Errors**: Check device connection and drivers
3. **Configuration Issues**: Verify board-specific overlay files

### Useful Commands
```bash
# Check west installation
west --version

# List available boards
west boards

# Clean workspace
west clean

# Update dependencies
west update
```

## Contributing

When adding new projects:
1. Follow the existing directory structure
2. Include proper configuration files
3. Add documentation for complex features
4. Test on target hardware

## Resources

- [nRF Connect SDK Documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/)
- [Zephyr RTOS Documentation](https://docs.zephyrproject.org/)
- [Nordic DevZone](https://devzone.nordicsemi.com/)
- [nRF Connect for Desktop](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop)

## License

This project is for educational and development purposes. Please refer to Nordic Semiconductor's licensing terms for commercial use.

---

*This repository represents hands-on learning with nRF Connect SDK, covering fundamental to advanced embedded systems concepts.*
