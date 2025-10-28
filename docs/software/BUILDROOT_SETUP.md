# Buildroot Setup Guide for E-Reader OS

This document describes how to build the custom Linux OS for the open-source e-reader using Buildroot.

## Overview

We're using **Buildroot 2024.02.7 LTS** to create a minimal, custom Linux distribution optimized for the e-reader hardware.

**Target Hardware:** Raspberry Pi 4B / Pi 5 (for development), Orange Pi Zero LTS (for production)

**Current Status:** Configured for Raspberry Pi 4B 64-bit with SPI and e-ink display support

## Prerequisites

### System Requirements

- **Host OS:** Linux (Ubuntu/Debian-based recommended)
- **Disk Space:** ~10GB free space for build
- **RAM:** 4GB minimum, 8GB recommended
- **Build Time:** 1-2 hours for first build (subsequent builds are much faster)

### Required Packages

On Ubuntu/Pop!_OS 24.04:

```bash
sudo apt-get install -y \
    build-essential \
    git \
    rsync \
    wget \
    unzip \
    libncurses-dev \
    python3 \
    bc \
    file \
    cpio \
    bzip2 \
    gzip
```

All dependencies are typically pre-installed on Pop!_OS 24.04.

## Directory Structure

```
os/buildroot/
├── buildroot-2024.02.7/              # Buildroot source
│   ├── board/raspberrypi/
│   │   ├── config_ereader_64bit.txt  # Custom boot config
│   │   ├── ereader-kernel.fragment   # Kernel config additions
│   │   └── ereader-overlay/          # Root filesystem overlay
│   │       ├── boot/
│   │       └── etc/
│   │           ├── fstab
│   │           ├── hostname
│   │           └── init.d/
│   │               └── S99ereader    # E-reader init script
│   ├── configs/
│   │   └── ereader_rpi4_64_defconfig # Our custom defconfig
│   ├── output/                       # Build output (created during build)
│   └── .config                       # Active configuration
└── buildroot-2024.02.7.tar.gz       # Source tarball
```

## Key Configuration Choices

### System Configuration

| Component | Choice | Reasoning |
|-----------|--------|-----------|
| **Architecture** | ARM 64-bit (aarch64) | Raspberry Pi 4 is 64-bit capable |
| **C Library** | musl | Smaller footprint (~600KB vs 2MB+) |
| **Kernel** | Raspberry Pi custom kernel | Optimized for Pi hardware |
| **Init System** | BusyBox init | Minimal, fast boot |
| **Bootloader** | U-Boot | Standard for embedded Linux |
| **Root FS** | ext4 | Reliable, well-supported |

### Kernel Features Enabled

The kernel is configured with these additional features via `ereader-kernel.fragment`:

- **SPI Support** (`CONFIG_SPI=y`, `CONFIG_SPI_BCM2835=y`, `CONFIG_SPI_SPIDEV=y`)
- **GPIO Support** (`CONFIG_GPIOLIB=y`, `CONFIG_GPIO_SYSFS=y`)
- **Framebuffer Support** (`CONFIG_FB=y` with deferred I/O)
- **I2C Support** (`CONFIG_I2C=y`, `CONFIG_I2C_BCM2835=y`)
- **Input Devices** (`CONFIG_INPUT_KEYBOARD=y`, `CONFIG_KEYBOARD_GPIO=y`)
- **Power Management** (`CONFIG_PM=y`, `CONFIG_PM_SLEEP=y`)
- **Device Tree Overlays** (`CONFIG_OF_OVERLAY=y`)

These enable e-ink display drivers and button input.

### Boot Configuration

The custom `config_ereader_64bit.txt` includes:

- **SPI enabled** (`dtparam=spi=on`)
- **I2C enabled** (`dtparam=i2c_arm=on`)
- **Minimal GPU memory** (16MB) - e-ink doesn't need much
- **WiFi/Bluetooth disabled** (power saving, can re-enable)
- **UART enabled** (for debugging)
- **Custom framebuffer** (800x600, 16-bit depth)

### Packages Included

**Development Tools** (can be removed for production):
- OpenSSH / Dropbear (SSH access)
- Python3 (scripting and testing)
- GDB, strace (debugging)
- Device Tree Compiler (DTC)

**Core System:**
- BusyBox (minimal Unix utilities)
- Linux kernel with custom config
- Raspberry Pi firmware

## Building the OS

### First-Time Build

1. **Navigate to Buildroot directory:**
   ```bash
   cd /home/corey/open-source-ereader/os/buildroot/buildroot-2024.02.7
   ```

2. **Load the e-reader configuration:**
   ```bash
   make ereader_rpi4_64_defconfig
   ```

3. **Start the build:**
   ```bash
   make
   ```

   This will take 1-2 hours. The build downloads source packages, compiles the toolchain, kernel, and all packages.

4. **Monitor build progress:**
   ```bash
   # In another terminal, you can monitor:
   tail -f build.log
   ```

### Subsequent Builds

After the first build, only changed components are rebuilt:

```bash
make
```

Typically takes just a few minutes.

### Clean Builds

```bash
# Clean everything (requires full rebuild)
make clean

# Clean just the build output (keeps downloads)
make clean
```

## Customizing the Configuration

### Interactive Configuration

To modify the Buildroot configuration:

```bash
make menuconfig
```

Navigate with arrow keys, space to select, '/' to search.

**Important sections:**
- `Target options` - Architecture settings
- `Toolchain` - C library, compiler options
- `System configuration` - Hostname, init system
- `Kernel` - Linux kernel configuration
- `Target packages` - Software packages to include

After making changes:

```bash
make savedefconfig BR2_DEFCONFIG=configs/ereader_rpi4_64_defconfig
```

### Modifying Kernel Configuration

To change kernel options:

1. **Edit the kernel fragment:**
   ```bash
   nano board/raspberrypi/ereader-kernel.fragment
   ```

2. **Or use kernel menuconfig:**
   ```bash
   make linux-menuconfig
   ```

   Then save the changes:
   ```bash
   make linux-update-defconfig
   ```

### Modifying Root Filesystem

Add or modify files in the overlay directory:

```bash
board/raspberrypi/ereader-overlay/
```

Any files here will be copied to the root filesystem during build.

**Common modifications:**
- `etc/init.d/` - Startup scripts
- `etc/fstab` - Filesystem mount points
- `etc/network/interfaces` - Network configuration
- `home/` - User files

## Build Output

After a successful build, the output is in:

```
output/images/
├── bcm2711-rpi-4-b.dtb          # Device tree blob
├── Image                         # Linux kernel
├── rootfs.ext2                  # Root filesystem image
├── rootfs.tar                   # Root filesystem tarball
├── rpi-firmware/                # Raspberry Pi firmware files
├── sdcard.img                   # Complete SD card image (ready to flash!)
└── u-boot.bin                   # U-Boot bootloader
```

**The main output you need:** `sdcard.img` - This is a complete, bootable SD card image.

## Flashing to SD Card

### Linux

1. **Identify your SD card device:**
   ```bash
   lsblk
   ```

   Look for your SD card (usually `/dev/sdX` or `/dev/mmcblkX`)

2. **Flash the image:**
   ```bash
   sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
   sudo sync
   ```

   **WARNING:** Double-check the device name! `dd` will overwrite whatever device you specify.

### macOS

```bash
diskutil list
sudo dd if=output/images/sdcard.img of=/dev/rdiskX bs=4m
```

### Windows

Use **Rufus** or **Win32DiskImager** to write `sdcard.img` to the SD card.

Or use `dd` via WSL (same as Linux instructions).

## First Boot

1. **Insert SD card** into Raspberry Pi 4
2. **Connect UART cable** (optional, for debugging):
   - GND → Pin 6
   - TX → Pin 8 (GPIO 14)
   - RX → Pin 10 (GPIO 15)
3. **Power on** the Pi
4. **Connect via serial console:**
   ```bash
   screen /dev/ttyUSB0 115200
   # or
   minicom -D /dev/ttyUSB0 -b 115200
   ```

5. **Default credentials:**
   - Username: `root`
   - Password: (none - just press Enter)

## Testing SPI

Once booted, verify SPI is available:

```bash
# Check SPI devices
ls -l /dev/spi*

# Should see:
# /dev/spidev0.0
# /dev/spidev0.1

# Check kernel modules
lsmod | grep spi

# Check device tree
ls /proc/device-tree/soc/spi*
```

## Troubleshooting

### Build Failures

**Issue:** Package download fails
```bash
# Clear downloads and retry
make clean
rm -rf dl/
make
```

**Issue:** Kernel build fails
```bash
# Rebuild just the kernel
make linux-rebuild
```

**Issue:** Out of disk space
```bash
# Check usage
df -h

# Clean build artifacts
make clean
```

### Boot Failures

**Issue:** Pi doesn't boot
- Check SD card is properly flashed
- Verify power supply (Pi 4 needs 5V 3A)
- Check serial console for error messages

**Issue:** Can't see SPI devices
- Check `config.txt` has `dtparam=spi=on`
- Verify kernel config includes `CONFIG_SPI_BCM2835=y`

**Issue:** No network
- Network is not configured by default in this minimal build
- Add network configuration to overlay if needed

## Next Steps

1. **Test e-ink display** when hardware arrives:
   - Connect display to SPI0
   - Load appropriate driver (Waveshare, etc.)
   - Test basic display functions

2. **Add Qt and MuPDF** for the reader application:
   ```bash
   make menuconfig
   # Enable BR2_PACKAGE_QT5
   # Enable BR2_PACKAGE_MUPDF (if available, or build manually)
   ```

3. **Port to Orange Pi Zero LTS:**
   - Create new defconfig based on Allwinner H3
   - Adjust kernel and device tree for H3 SoC
   - Test on Orange Pi hardware

## Build Performance

Typical build times on different systems:

| System | CPU | RAM | First Build | Incremental |
|--------|-----|-----|-------------|-------------|
| Desktop | i7-8700K | 16GB | ~45 min | 2-5 min |
| Laptop | i5-1135G7 | 8GB | ~80 min | 3-7 min |
| VM | 4 cores | 8GB | ~120 min | 5-10 min |

## Additional Resources

- [Buildroot User Manual](https://buildroot.org/downloads/manual/manual.html)
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [Linux Kernel Configuration](https://www.kernel.org/doc/html/latest/)
- [Device Tree Documentation](https://www.kernel.org/doc/Documentation/devicetree/)

## Version History

- **2024-10-23**: Initial Buildroot setup for Raspberry Pi 4 with SPI support
