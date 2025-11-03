# Building E-Reader OS for Raspberry Pi Zero W

This guide explains how to build the custom E-Reader operating system for Raspberry Pi Zero W using the converted Buildroot configuration.

## What Changed from Pi 4 to Pi Zero W?

### Hardware Differences
| Component | Pi 4 64-bit | Pi Zero W | Impact |
|-----------|-------------|-----------|--------|
| CPU | Cortex-A72 (quad-core 1.5GHz) | ARM1176JZF-S (single-core 1GHz) | Slower, but adequate for e-reading |
| Architecture | ARMv8 (64-bit) | ARMv6 (32-bit) | Different kernel and firmware |
| RAM | 1-8GB | 512MB | Requires careful memory management |
| WiFi | BCM43455 | BCM43438 | Same driver, different chip |
| Bluetooth | 5.0 | 4.1 | Disabled to save power |
| USB | USB 3.0 + 2.0 | 1x USB 2.0 (OTG) | Limited peripheral support |

### Configuration Changes Made

#### 1. **Buildroot Defconfig** (`ereader_rpizerow_defconfig`)
- **Architecture**: `BR2_arm=y` instead of `BR2_aarch64=y`
- **CPU**: `BR2_arm1176jzf_s=y` instead of `BR2_cortex_a72=y`
- **Kernel defconfig**: `bcmrpi` instead of `bcm2711`
- **Device Tree**: `bcm2708-rpi-zero-w` instead of `bcm2711-rpi-4-b`
- **Firmware**: `BR2_PACKAGE_RPI_FIRMWARE_VARIANT_PI=y` instead of `VARIANT_PI4`
- **Bootcode**: Added `BR2_PACKAGE_RPI_FIRMWARE_BOOTCODE_BIN=y` (Pi Zero W needs this, Pi 4 doesn't)
- **Network**: Changed DHCP from `eth0` to `wlan0` (Pi Zero W has WiFi only)

#### 2. **Boot Configuration** (`config_ereader_zerow.txt`)
- **Kernel**: `kernel=zImage` (32-bit) instead of `kernel=Image` (64-bit)
- **GPU Memory**: `gpu_mem=16` instead of gpu_mem_XXX variants
- **CPU Frequency**: Adjusted for Pi Zero W's 1GHz CPU (700MHz min, 1000MHz max)
- **Removed**: 64-bit flag (`arm_64bit=1`)
- **Added**: Boot optimization flags (`disable_splash=1`, `boot_delay=0`)

#### 3. **Kernel Fragment** (`ereader-kernel.fragment`)
- **No changes needed!** The kernel fragment is hardware-agnostic
- WiFi drivers work for both BCM43455 (Pi 4) and BCM43438 (Pi Zero W)

#### 4. **Filesystem Overlay** (`board/raspberrypi/ereader-overlay`)
- **Shared between Pi 4 and Pi Zero W** - no changes needed
- All scripts and configuration files work on both platforms

## Prerequisites

### System Requirements
- **OS**: Linux (Ubuntu 20.04+ recommended, or WSL2 on Windows)
- **Disk Space**: 15GB free space
- **RAM**: 4GB minimum, 8GB recommended
- **Time**: 1-3 hours for first build (depends on CPU speed)

### Install Dependencies

**Ubuntu/Debian**:
```bash
sudo apt update
sudo apt install -y \
    git build-essential \
    libncurses5-dev bc wget cpio unzip rsync \
    gcc-arm-linux-gnueabihf \
    device-tree-compiler u-boot-tools \
    dosfstools mtools
```

**Fedora/RHEL**:
```bash
sudo dnf install -y \
    git make gcc ncurses-devel bc wget cpio unzip rsync \
    gcc-arm-linux-gnu \
    dtc uboot-tools \
    dosfstools mtools
```

**Arch Linux**:
```bash
sudo pacman -S \
    git base-devel ncurses bc wget cpio unzip rsync \
    arm-none-eabi-gcc \
    dtc uboot-tools \
    dosfstools mtools
```

## Building the Image

### Step 1: Navigate to Buildroot Directory

```bash
cd ~/Documents/open-source-ereader/os/buildroot/buildroot-2024.02.7
```

### Step 2: Load E-Reader Pi Zero W Configuration

```bash
make ereader_rpizerow_defconfig
```

This loads the custom configuration optimized for Pi Zero W e-reader use.

### Step 3: (Optional) Customize Configuration

If you want to modify packages or settings:

```bash
make menuconfig
```

**Common customizations**:
- Add packages under "Target packages"
- Adjust filesystem size under "Filesystem images"
- Modify kernel config: `make linux-menuconfig`

**Save your changes**: Changes are saved automatically in `.config`

### Step 4: Build the System

```bash
# Build with all CPU cores (faster)
make -j$(nproc)

# Or specify number of cores manually
# make -j4
```

**Build time**:
- First build: 1-3 hours (downloads and compiles everything)
- Subsequent builds: 5-30 minutes (only rebuilds changed components)

**Output location**: `output/images/`

### Step 5: Verify Build Output

Check that these files were created:

```bash
ls -lh output/images/
```

Expected files:
- `sdcard.img` - Complete SD card image (ready to flash)
- `zImage` - Linux kernel (32-bit ARM)
- `bcm2708-rpi-zero-w.dtb` - Device tree blob
- `rootfs.ext4` - Root filesystem
- `boot.vfat` - Boot partition

## Flashing to SD Card

### Linux

```bash
# Find your SD card device (CAREFUL!)
lsblk

# Flash the image (replace /dev/sdX with your SD card)
# WARNING: This will erase all data on the SD card!
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress conv=fsync

# Sync to ensure all writes complete
sync
```

### macOS

```bash
# Find your SD card
diskutil list

# Unmount the SD card (replace diskX)
diskutil unmountDisk /dev/diskX

# Flash the image
sudo dd if=output/images/sdcard.img of=/dev/rdiskX bs=4m

# Eject
diskutil eject /dev/diskX
```

### Windows (WSL2)

**Option 1**: Use [Raspberry Pi Imager](https://www.raspberrypi.org/software/)
1. Select "Use custom" and choose `sdcard.img`
2. Select your SD card
3. Click "Write"

**Option 2**: Use [balenaEtcher](https://www.balena.io/etcher/)
1. Select `sdcard.img`
2. Select SD card
3. Flash!

**Option 3**: Use Win32DiskImager (traditional method)

## First Boot

### 1. Insert SD Card

Insert the flashed SD card into your Raspberry Pi Zero W.

### 2. Connect Serial Console (Optional but Recommended)

**Hardware needed**:
- USB-to-TTL serial adapter (3.3V!)
- 3 jumper wires

**Connections**:
| Pi Zero W Pin | Serial Adapter |
|---------------|----------------|
| Pin 6 (GND) | GND |
| Pin 8 (TX) | RX |
| Pin 10 (RX) | TX |

**Connect**:
```bash
# Linux
screen /dev/ttyUSB0 115200

# or
minicom -D /dev/ttyUSB0 -b 115200

# macOS
screen /dev/cu.usbserial-XXXX 115200
```

### 3. Power On

Connect power via micro USB port. The Pi Zero W needs:
- **Voltage**: 5V
- **Current**: 1A minimum (2A recommended)

**Boot sequence** (watch serial console):
1. U-Boot loads (~2 seconds)
2. Linux kernel boots (~3 seconds)
3. Init scripts run (~1 second)
4. Login prompt appears

**Total boot time**: ~5-8 seconds

### 4. Login

**Default credentials**:
- **Username**: `root`
- **Password**: (none - just press Enter)

## Connecting to WiFi

### Method 1: Using Setup Script (Easiest)

```bash
# Run the interactive WiFi setup
setup-wifi
```

Follow the prompts to enter your WiFi SSID and password.

### Method 2: Manual Configuration

Edit `/etc/wpa_supplicant/wpa_supplicant.conf`:

```bash
nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Add your network:

```
network={
    ssid="YourWiFiName"
    psk="YourPassword"
}
```

Restart WiFi:
```bash
/etc/init.d/S40network restart
```

### Method 3: Pre-configure Before First Boot

1. After flashing SD card, mount the boot partition on your PC
2. Find `/etc/wpa_supplicant/wpa_supplicant.conf` in the rootfs partition
3. Edit it to add your WiFi credentials
4. Safely eject and boot

### Verify WiFi Connection

```bash
# Check interface status
ifconfig wlan0

# Check IP address
ip addr show wlan0

# Test connectivity
ping -c 3 8.8.8.8
```

## SSH Access

Once WiFi is connected:

```bash
# Find Pi's IP address (on the Pi)
hostname -I

# From your computer
ssh root@192.168.1.XXX
```

**Note**: Default password is blank, just press Enter.

## Testing the Display

### Run the Framebuffer Test

The system includes a simple framebuffer test script:

```bash
# This should display patterns on the framebuffer
python3 /usr/bin/framebuffer_test.py
```

If you have an e-ink display connected via SPI, you'll need to:
1. Load the display driver
2. Configure the SPI pins
3. Run display-specific test code

(Detailed e-ink setup will be covered in a separate guide)

## Rebuilding After Changes

### Rebuild Specific Package

```bash
# Example: Rebuild BusyBox
make busybox-rebuild

# Rebuild Python
make python3-rebuild

# Rebuild kernel
make linux-rebuild
```

### Rebuild Everything

```bash
make clean
make -j$(nproc)
```

### Save Your Configuration

```bash
# Save current configuration as defconfig
make savedefconfig BR2_DEFCONFIG=configs/ereader_rpizerow_defconfig
```

This updates `ereader_rpizerow_defconfig` with your changes.

## Troubleshooting

### Build Fails

**Problem**: "No rule to make target..."
```bash
# Solution: Clean and rebuild
make clean
make ereader_rpizerow_defconfig
make -j$(nproc)
```

**Problem**: Download failures
```bash
# Solution: Delete download cache and retry
rm -rf dl/
make -j$(nproc)
```

### Boot Issues

**Problem**: Pi doesn't boot (no serial output)
- Check SD card was flashed correctly
- Verify power supply (needs 5V 1A minimum)
- Try different SD card (some cheap cards don't work)

**Problem**: Kernel panic or boot loops
- Check serial console output for errors
- Verify correct defconfig was used (`ereader_rpizerow_defconfig`)
- Try reflashing SD card

### WiFi Not Working

**Problem**: `wlan0` interface doesn't appear
```bash
# Check if firmware loaded
dmesg | grep -i firmware

# Check if module loaded
lsmod | grep brcmfmac

# Manually load module
modprobe brcmfmac
```

**Problem**: Can't connect to WiFi
```bash
# Check wpa_supplicant status
ps aux | grep wpa_supplicant

# Restart networking
/etc/init.d/S40network restart

# Check for errors
dmesg | grep -i wlan
```

### Display Not Working

**Problem**: Framebuffer test shows nothing
- Verify SPI is enabled: `ls /dev/spi*` should show `/dev/spidev0.0` and `/dev/spidev0.1`
- Check GPIO: `ls /sys/class/gpio/`
- Review kernel messages: `dmesg | grep -i spi`

## Performance Optimization

### Reduce Boot Time

1. **Disable UART** (saves ~0.5s):
   Edit `board/raspberrypi/config_ereader_zerow.txt`:
   ```
   enable_uart=0
   ```

2. **Disable unnecessary services**:
   ```bash
   # Remove from /etc/init.d/ or make non-executable
   chmod -x /etc/init.d/S50dropbear  # Disable SSH server
   ```

3. **Optimize kernel**:
   ```bash
   make linux-menuconfig
   # Disable unused drivers and features
   ```

### Reduce Memory Usage

Check memory usage:
```bash
free -h
```

Optimize:
- Remove unused packages in `make menuconfig`
- Reduce GPU memory to 16MB (already done)
- Use musl instead of glibc (already done)

### Improve Battery Life

1. **Disable WiFi when not needed**:
   ```bash
   ifconfig wlan0 down
   ```

2. **Reduce CPU frequency**:
   Edit `config_ereader_zerow.txt`:
   ```
   arm_freq=700
   ```

3. **Disable HDMI**:
   ```bash
   /usr/bin/tvservice -o
   ```

4. **Enable power management in kernel**:
   Already configured in `ereader-kernel.fragment`

## Differences from Pi 4 Version

### What Works Exactly the Same
✅ Filesystem overlay and scripts  
✅ Python packages and reader app  
✅ Network configuration  
✅ WiFi drivers and setup  
✅ SPI, I2C, GPIO configuration  
✅ Power management features  

### What's Different
🔧 **Architecture**: 32-bit vs 64-bit  
🔧 **Boot files**: `zImage` vs `Image`, different firmware  
🔧 **Performance**: 2-3x slower (but still good enough for reading)  
🔧 **Network**: WiFi only (no Ethernet like Pi 4)  
🔧 **USB**: Single USB port (OTG only)  

### What Doesn't Work
❌ **64-bit applications**: Must use 32-bit ARM binaries  
❌ **Some Python packages**: Some wheels only available for ARMv7, not ARMv6  

## Next Steps

1. **Connect E-Ink Display**: See `docs/hardware/DISPLAY_CONNECTION.md` (coming soon)
2. **Install Reader Application**: See `docs/software/READER_APP_SETUP.md` (coming soon)
3. **Customize UI**: See `docs/software/UI_CUSTOMIZATION.md` (coming soon)
4. **Battery Setup**: See `docs/hardware/BATTERY_SETUP.md` (coming soon)

## Additional Resources

- [Buildroot Manual](https://buildroot.org/downloads/manual/manual.html)
- [Raspberry Pi Zero W Documentation](https://www.raspberrypi.org/documentation/)
- [Pi Zero W Pinout](https://pinout.xyz/)
- [E-Reader Project Main Docs](../../README.md)

## Summary

You now have a complete Buildroot configuration that builds a custom Linux OS for Raspberry Pi Zero W, optimized for e-reader use. The configuration is based on the Pi 4 version but adapted for the Pi Zero W's hardware.

**Key files created**:
- `configs/ereader_rpizerow_defconfig` - Main Buildroot configuration
- `board/raspberrypi/config_ereader_zerow.txt` - Boot configuration
- (Kernel fragment shared with Pi 4 version)

**Build command**:
```bash
cd os/buildroot/buildroot-2024.02.7
make ereader_rpizerow_defconfig
make -j$(nproc)
```

Happy building! 🚀
