---
type: reference
title: Device Tree Overlay Documentation for E-Paper Display
created: 2026-01-13
tags:
  - hardware
  - device-tree
  - spi
  - gpio
  - kernel
related:
  - "[[DISPLAY_SPECS]]"
  - "[[WIRING_GUIDE]]"
  - "[[PHASE_01_LOG]]"
---

# Device Tree Overlay for Waveshare 4.2" E-Paper Display

This document explains the device tree overlay configuration for the Waveshare 4.2" e-paper display module connected to the Raspberry Pi Zero W.

## Overview

Device tree overlays provide a way to describe hardware configuration to the Linux kernel without modifying the base device tree or recompiling the kernel. The overlay for the e-paper display configures:

- **SPI0 interface** - Communication bus for display data
- **GPIO pins** - Control signals (DC, RST, BUSY)
- **SPI device parameters** - Speed, mode, chip select

## Overlay File Location

**Source file:** `board/ereader/overlays/waveshare-epd-4_2.dts`
**Compiled blob:** `output/images/rpi-firmware/overlays/waveshare-epd-4_2.dtbo`
**Boot location:** `/boot/overlays/waveshare-epd-4_2.dtbo` (on SD card)

## Hardware Pin Configuration

The overlay defines the following hardware connections:

| Signal | GPIO | Physical Pin | Direction | Description |
|--------|------|--------------|-----------|-------------|
| MOSI (DIN) | GPIO 10 | Pin 19 | Output | SPI Master Out Slave In (data to display) |
| SCLK (CLK) | GPIO 11 | Pin 23 | Output | SPI clock signal |
| CE0 (CS) | GPIO 8 | Pin 24 | Output | SPI chip select (active low) |
| DC | GPIO 25 | Pin 22 | Output | Data/Command control (0=command, 1=data) |
| RST | GPIO 17 | Pin 11 | Output | Reset signal (active low) |
| BUSY | GPIO 24 | Pin 18 | Input | Display busy status (1=busy, 0=ready) |

For complete wiring details, see [[WIRING_GUIDE]].

## Device Tree Structure

### Fragment 0: SPI Device Configuration

```dts
fragment@0 {
    target = <&spi0>;
    __overlay__ {
        status = "okay";
        epd_spidev: spidev@0 {
            compatible = "spidev";
            reg = <0>;
            spi-max-frequency = <4000000>;
            spi-cpol = <0>;
            spi-cpha = <0>;
        };
    };
};
```

**Purpose:** Configures the SPI0 bus and creates a userspace SPI device.

**Key parameters:**
- `compatible = "spidev"` - Creates `/dev/spidev0.0` for userspace access
- `reg = <0>` - Uses chip select 0 (CE0 / GPIO 8)
- `spi-max-frequency = <4000000>` - 4 MHz SPI clock (conservative for reliability)
- `spi-cpol = <0>`, `spi-cpha = <0>` - SPI Mode 0 (clock idles low, data sampled on rising edge)

### Fragment 1: GPIO Pin Configuration

```dts
fragment@1 {
    target = <&gpio>;
    __overlay__ {
        epd_pins: epd_pins {
            brcm,pins = <17 24 25>;  /* RST, BUSY, DC */
            brcm,function = <1 0 1>; /* OUT, IN, OUT */
            brcm,pull = <0 2 0>;     /* NONE, UP, NONE */
        };
    };
};
```

**Purpose:** Configures pin functions and pull resistors.

**Pin functions:**
- `<1>` = Output (RST and DC)
- `<0>` = Input (BUSY)

**Pull resistor configuration:**
- `<0>` = None (RST and DC)
- `<2>` = Pull-up (BUSY)

### Fragment 2: GPIO Labels for Control Signals

```dts
fragment@2 {
    target-path = "/";
    __overlay__ {
        epd_gpios {
            compatible = "gpio-leds";
            pinctrl-names = "default";
            pinctrl-0 = <&epd_pins>;

            dc {
                label = "epd_dc";
                gpios = <&gpio 25 0>;
                default-state = "off";
            };

            rst {
                label = "epd_rst";
                gpios = <&gpio 17 0>;
                default-state = "on";
            };
        };
    };
};
```

**Purpose:** Provides labeled GPIO outputs for DC and RST signals.

**Note:** Using `gpio-leds` subsystem is a common technique to export GPIOs with meaningful labels, even though these aren't LEDs. This allows for easier identification in `/sys/class/leds/`.

### Fragment 3: BUSY Pin Input Configuration

```dts
fragment@3 {
    target = <&gpio>;
    __overlay__ {
        epd_busy_pin: epd_busy_pin {
            brcm,pins = <24>;
            brcm,function = <0>;  /* Input */
            brcm,pull = <1>;      /* Pull-down */
        };
    };
};
```

**Purpose:** Configures GPIO 24 as input with pull-down resistor for BUSY signal monitoring.

**Why pull-down?** The BUSY signal is driven high by the display when busy, and we use a pull-down to ensure a defined low state when the display is idle or disconnected.

## Runtime Customization

The overlay supports runtime parameter overrides via `/boot/config.txt`:

### Override Examples

```bash
# Change SPI speed to 2 MHz (more conservative)
dtoverlay=waveshare-epd-4_2,speed=2000000

# Use different GPIO pins (advanced users only)
dtoverlay=waveshare-epd-4_2,dc_pin=22,rst_pin=27,busy_pin=23
```

### Available Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `speed` | 4000000 | SPI clock frequency in Hz (max 10000000) |
| `dc_pin` | 25 | GPIO number for Data/Command signal |
| `rst_pin` | 17 | GPIO number for Reset signal |
| `busy_pin` | 24 | GPIO number for Busy status input |

**Warning:** Changing pin assignments requires updating the wiring and may conflict with other peripherals. Only modify if you understand the implications.

## Kernel Requirements

The device tree overlay requires the following kernel configuration options (already enabled in `configs/linux_ereader.fragment`):

```
CONFIG_SPI=y                   # Core SPI support
CONFIG_SPI_BCM2835=y           # BCM2835 SPI controller
CONFIG_SPI_SPIDEV=y            # Userspace SPI device driver
CONFIG_GPIOLIB=y               # GPIO subsystem
CONFIG_GPIO_BCM2835=y          # BCM2835 GPIO driver
CONFIG_GPIO_CDEV=y             # GPIO character device interface
CONFIG_GPIO_SYSFS=y            # GPIO sysfs interface (legacy)
CONFIG_OF_OVERLAY=y            # Device tree overlay support
```

For details on kernel configuration, see [[PHASE_01_LOG]].

## Compilation Process

### Manual Compilation

Device tree overlays are compiled using the device tree compiler (`dtc`):

```bash
dtc -@ -I dts -O dtb -o waveshare-epd-4_2.dtbo waveshare-epd-4_2.dts
```

**Flags:**
- `-@` - Generate symbols for overlay resolution
- `-I dts` - Input format: Device Tree Source
- `-O dtb` - Output format: Device Tree Blob
- `-o <file>` - Output file name (.dtbo extension by convention)

### Automatic Compilation (Buildroot)

The `board/ereader/post-build.sh` script automatically compiles all `.dts` files in `board/ereader/overlays/` during the Buildroot build process:

1. Post-build script runs after root filesystem is built
2. Finds all `.dts` files in overlay directory
3. Compiles each with `dtc` from host tools
4. Places compiled `.dtbo` files in `output/images/rpi-firmware/overlays/`
5. Overlays are included in boot partition by `post-image.sh`

## Loading the Overlay

### Boot-Time Loading

Add the following line to `/boot/config.txt`:

```bash
dtoverlay=waveshare-epd-4_2
```

The overlay is loaded during boot before the kernel starts, ensuring SPI and GPIO are configured before any userspace code runs.

### Runtime Loading (Advanced)

Overlays can be loaded at runtime using configfs (requires `CONFIG_OF_CONFIGFS=y`):

```bash
# Mount configfs if not already mounted
mount -t configfs none /sys/kernel/config

# Load overlay
mkdir /sys/kernel/config/device-tree/overlays/epd
cat /boot/overlays/waveshare-epd-4_2.dtbo > \
    /sys/kernel/config/device-tree/overlays/epd/dtbo
```

**Note:** Runtime loading is complex and not recommended for production. Boot-time loading via `config.txt` is simpler and more reliable.

## Verification

After booting with the overlay loaded, verify correct configuration:

### Check SPI Device

```bash
# SPI device should exist
ls -l /dev/spidev0.0

# Should show:
# crw------- 1 root root 153, 0 Jan 13 12:00 /dev/spidev0.0
```

### Check GPIO Availability

```bash
# Check GPIO chip exists
ls -l /dev/gpiochip0

# List all available GPIO lines
gpioinfo gpiochip0 | grep -E '(17|24|25)'

# Should show something like:
# line  17:      "GPIO17"       unused   input  active-high
# line  24:      "GPIO24"       unused   input  active-high
# line  25:      "GPIO25"       unused   input  active-high
```

### Check Device Tree

```bash
# View loaded device tree in human-readable format
dtc -I fs /proc/device-tree | grep -A 10 spidev

# Check for overlay in kernel log
dmesg | grep -i spi
# Expected output includes:
# [    1.234567] spi-bcm2835 20204000.spi: master is unqueued, this is deprecated
# [    1.234567] spi spi0.0: spidev@0 registered
```

## Userspace Access

### Accessing SPI (/dev/spidev0.0)

The SPI device can be accessed using:
- **Linux SPI API** - `ioctl()` calls with `<linux/spi/spidev.h>`
- **Libraries** - WiringPi, bcm2835 library, lgpio
- **Python** - spidev module

Example (C):
```c
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int fd = open("/dev/spidev0.0", O_RDWR);
uint32_t speed = 4000000;
uint8_t mode = SPI_MODE_0;

ioctl(fd, SPI_IOC_WR_MODE, &mode);
ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
```

### Accessing GPIO

Modern approach using libgpiod:
```c
#include <gpiod.h>

struct gpiod_chip *chip = gpiod_chip_open("/dev/gpiochip0");
struct gpiod_line *dc_line = gpiod_chip_get_line(chip, 25);
gpiod_line_request_output(dc_line, "epd_dc", 0);
gpiod_line_set_value(dc_line, 1);  // Set DC high
```

Legacy sysfs approach (deprecated but widely supported):
```bash
# Export GPIO
echo 25 > /sys/class/gpio/export

# Set direction
echo out > /sys/class/gpio/gpio25/direction

# Set value
echo 1 > /sys/class/gpio/gpio25/value
```

## Troubleshooting

### SPI Device Not Present

**Symptoms:** `/dev/spidev0.0` does not exist

**Solutions:**
1. Check overlay is loaded in `/boot/config.txt`:
   ```bash
   grep dtoverlay /boot/config.txt | grep epd
   ```
2. Verify overlay file exists:
   ```bash
   ls -l /boot/overlays/waveshare-epd-4_2.dtbo
   ```
3. Check kernel has SPI support:
   ```bash
   zcat /proc/config.gz | grep -E 'CONFIG_SPI|CONFIG_SPIDEV'
   ```
4. Look for errors in kernel log:
   ```bash
   dmesg | grep -i spi
   ```

### GPIO Not Accessible

**Symptoms:** Cannot access GPIO pins

**Solutions:**
1. Check GPIO device exists:
   ```bash
   ls -l /dev/gpiochip0
   ```
2. Verify GPIO support in kernel:
   ```bash
   zcat /proc/config.gz | grep CONFIG_GPIO
   ```
3. Check pin is not already claimed:
   ```bash
   gpioinfo gpiochip0 | grep -E '(17|24|25)'
   ```
4. Check device tree loaded correctly:
   ```bash
   ls /proc/device-tree/soc/gpio@7e200000/
   ```

### SPI Communication Errors

**Symptoms:** Data corruption, timeouts, or no response

**Solutions:**
1. Reduce SPI speed:
   ```bash
   # In /boot/config.txt:
   dtoverlay=waveshare-epd-4_2,speed=2000000
   ```
2. Check wiring connections (especially GND, CLK, MOSI)
3. Verify SPI mode matches display requirements (Mode 0)
4. Check power supply is stable (measure VCC during SPI transfers)
5. Use oscilloscope to verify signal integrity

### Display Not Responding

**Symptoms:** Display stays blank or shows garbage

**Solutions:**
1. Verify all connections per [[WIRING_GUIDE]]
2. Check BUSY signal is readable:
   ```bash
   gpioget gpiochip0 24
   ```
3. Ensure proper reset sequence (10ms low pulse on RST)
4. Check DC signal toggles correctly during communication
5. Verify correct initialization sequence from [[DISPLAY_SPECS]]

## Design Rationale

### Why Userspace SPI (spidev)?

**Advantages:**
- Faster development and debugging
- Full control over timing and initialization
- No kernel module required
- Easier to port example code from Waveshare

**Disadvantages:**
- Higher latency than kernel driver
- No Linux framebuffer integration
- Requires root or specific permissions

**Alternative:** A kernel framebuffer driver could be developed in Phase 6+ if needed for better system integration.

### Why These GPIO Pins?

The selected GPIOs (17, 24, 25) were chosen because:
- They are available on Pi Zero W 40-pin header
- They don't conflict with:
  - SPI0 pins (8, 9, 10, 11)
  - I2C pins (2, 3) - reserved for future sensors
  - UART pins (14, 15) - used for serial console
  - Audio pins (18, 19, 20, 21) - may be used for audio out
- They match Waveshare's reference implementation
- They are grouped physically for easier wiring

### SPI Mode 0 Selection

The UC8176/IL0398 controller datasheet specifies SPI Mode 0:
- **CPOL=0**: Clock idles at LOW
- **CPHA=0**: Data sampled on rising edge, changed on falling edge

Using the correct SPI mode is critical for reliable communication.

### 4 MHz SPI Speed

**Display capability:** Up to 10 MHz
**Selected speed:** 4 MHz

**Rationale:**
- Provides reliable operation with typical jumper wire lengths (10-20cm)
- Reduces EMI and signal integrity issues
- Still fast enough for full refresh in <1 second
- Can be increased to 10 MHz with proper PCB layout

## References

### Device Tree Documentation
- [Linux Kernel Device Tree Usage](https://www.kernel.org/doc/Documentation/devicetree/usage-model.txt)
- [Device Tree Overlay Notes](https://www.kernel.org/doc/Documentation/devicetree/overlay-notes.txt)
- [Raspberry Pi Device Tree Documentation](https://www.raspberrypi.com/documentation/computers/configuration.html#part3)

### SPI and GPIO Bindings
- [Linux SPI Bus Device Tree Bindings](https://www.kernel.org/doc/Documentation/devicetree/bindings/spi/spi-controller.yaml)
- [BCM2835 GPIO Device Tree Bindings](https://www.kernel.org/doc/Documentation/devicetree/bindings/gpio/brcm,bcm2835-gpio.txt)

### Tutorials and Examples
- [Mastering Device Tree Overlays in Raspberry Pi](https://hubtronics.in/learn/mastering-device-tree-overlays-rpi)
- [Enabling new hardware on Raspberry Pi with Device Tree Overlays – Bootlin](https://bootlin.com/blog/enabling-new-hardware-on-raspberry-pi-with-device-tree-overlays/)
- [How to add or change SPI chip select pins on Raspberry Pi with device tree overlay](https://gist.github.com/mcbridejc/d060602e892f6879e7bc8b93aa3f85be)

### Related Documentation
- [[DISPLAY_SPECS]] - Technical specifications for the Waveshare 4.2" display
- [[WIRING_GUIDE]] - Physical wiring instructions
- [[PHASE_01_LOG]] - Kernel configuration decisions

## Revision History

- **2026-01-13:** Initial documentation created
  - Documented device tree overlay structure
  - Explained SPI and GPIO configuration
  - Added verification and troubleshooting sections
  - Included design rationale and references

---

**Document Status:** Complete - Phase 01 Foundation
**Last Updated:** 2026-01-13
**Next Review:** After first hardware test (Phase 01, Task 6+)
