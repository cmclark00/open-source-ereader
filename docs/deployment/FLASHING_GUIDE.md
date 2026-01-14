---
type: reference
title: SD Card Flashing and Deployment Guide
created: 2026-01-13
tags:
  - deployment
  - sd-card
  - flashing
  - raspberry-pi
  - setup
related:
  - "[[BUILD_PROCESS]]"
  - "[[WIRING_GUIDE]]"
  - "[[Phase-01-Foundation-and-Working-Display]]"
---

# SD Card Flashing and Deployment Guide

This guide provides step-by-step instructions for writing the e-reader system image to an SD card, booting the Raspberry Pi Zero W, and verifying that the display is working correctly.

## Prerequisites

### Hardware Required
- Raspberry Pi Zero W
- MicroSD card (8GB minimum, Class 10 or better recommended)
- SD card reader/writer
- Waveshare 4.2" e-paper display (Rev 2.2)
- 5V 2A USB power supply (Micro USB)
- *Optional:* USB-to-serial TTL adapter (3.3V) for serial console debugging

### Software Required
- Built system image: `buildroot/output/images/sdcard.img`
- SD card flashing tool (choose one based on your OS):
  - **Linux/Mac**: `dd` command (built-in) or Etcher
  - **Windows**: Rufus, Win32DiskImager, or Etcher
  - **Cross-platform**: [balenaEtcher](https://www.balena.io/etcher/)

### Wiring Verification
Before powering on the Pi, ensure the e-paper display is correctly wired according to `[[WIRING_GUIDE]]`. Incorrect wiring can damage the display or Pi.

**Critical connections:**
- SPI0 pins: MOSI (GPIO 10), MISO (GPIO 9), SCLK (GPIO 11), CE0 (GPIO 8)
- Control pins: RST (GPIO 17), DC (GPIO 25), BUSY (GPIO 24)
- Power: 3.3V and GND (multiple ground connections recommended)

## Writing the SD Card Image

### Method 1: Using balenaEtcher (Recommended for Beginners)

balenaEtcher is a cross-platform tool with a simple GUI, available for Linux, Mac, and Windows.

1. **Download balenaEtcher**
   - Visit: https://www.balena.io/etcher/
   - Download for your operating system
   - Install and launch the application

2. **Flash the Image**
   - Click "Flash from file"
   - Navigate to `buildroot/output/images/sdcard.img`
   - Click "Select target" and choose your SD card
   - **WARNING**: Double-check you've selected the correct device - flashing will erase all data
   - Click "Flash!" and wait for completion (~2-5 minutes)
   - Etcher will automatically verify the written image

3. **Eject the SD Card**
   - Wait for Etcher to complete and show "Flash Complete!"
   - Safely eject the SD card from your computer

### Method 2: Using `dd` on Linux/Mac

The `dd` command is a powerful low-level tool available on Linux and Mac.

1. **Insert SD Card**
   - Insert the SD card into your card reader
   - Wait a few seconds for the system to detect it

2. **Identify the SD Card Device**
   ```bash
   # On Linux:
   lsblk
   # Look for your SD card (e.g., /dev/sdb, /dev/mmcblk0)
   # Check the size to confirm it's the correct device

   # On Mac:
   diskutil list
   # Look for your SD card (e.g., /dev/disk2)
   ```

   **WARNING**: Using the wrong device will overwrite that disk! Triple-check the device name.

3. **Unmount the SD Card (if auto-mounted)**
   ```bash
   # On Linux:
   sudo umount /dev/sdX*  # Replace sdX with your device

   # On Mac:
   sudo diskutil unmountDisk /dev/diskN  # Replace diskN with your device
   ```

4. **Write the Image**
   ```bash
   # Navigate to project root
   cd /path/to/open-source-ereader

   # On Linux:
   sudo dd if=buildroot/output/images/sdcard.img of=/dev/sdX bs=4M status=progress
   sudo sync

   # On Mac:
   sudo dd if=buildroot/output/images/sdcard.img of=/dev/rdiskN bs=4m
   # Note: Mac uses /dev/rdiskN (raw disk) for faster writes
   ```

   Expected write time: 2-5 minutes depending on SD card speed.

5. **Eject the SD Card**
   ```bash
   # On Linux:
   sudo eject /dev/sdX

   # On Mac:
   sudo diskutil eject /dev/diskN
   ```

### Method 3: Using the Helper Script (Linux/Mac)

For convenience, use the provided `scripts/flash-sdcard.sh` script:

```bash
# Navigate to project root
cd /path/to/open-source-ereader

# Run with device path as argument
sudo ./scripts/flash-sdcard.sh /dev/sdX  # Linux
sudo ./scripts/flash-sdcard.sh /dev/diskN  # Mac

# The script will:
# - Verify the image exists
# - Confirm the target device
# - Unmount any mounted partitions
# - Write the image with progress display
# - Sync and eject
```

### Method 4: Using Win32DiskImager (Windows)

1. **Download Win32DiskImager**
   - Visit: https://sourceforge.net/projects/win32diskimager/
   - Download and install

2. **Flash the Image**
   - Launch Win32DiskImager as Administrator
   - Click the folder icon and select `buildroot/output/images/sdcard.img`
   - Select your SD card drive letter from the "Device" dropdown
   - **WARNING**: Verify the drive letter is correct!
   - Click "Write" and confirm
   - Wait for completion (~5-10 minutes)

3. **Eject the SD Card**
   - Click "Exit" in Win32DiskImager
   - Safely remove the SD card via Windows system tray

### Method 5: Using Rufus (Windows)

1. **Download Rufus**
   - Visit: https://rufus.ie/
   - Download the portable or installer version

2. **Flash the Image**
   - Launch Rufus
   - Select your SD card from the "Device" dropdown
   - Click "SELECT" and choose `buildroot/output/images/sdcard.img`
   - Click "START" and confirm any warnings
   - Wait for completion

3. **Eject the SD Card**
   - Close Rufus
   - Safely remove the SD card

## First Boot

### Boot Sequence

1. **Insert SD Card**
   - Ensure the Pi is powered off
   - Insert the freshly flashed SD card into the Raspberry Pi Zero W
   - Verify the e-paper display is properly connected

2. **Power On**
   - Connect the 5V Micro USB power supply
   - The Pi will begin booting immediately

3. **Expected Boot Timeline**
   - **0-5 seconds**: Pi firmware initializes, green LED may blink
   - **5-15 seconds**: Linux kernel boots, console messages on serial
   - **15-25 seconds**: Init system starts services
   - **25-30 seconds**: Display test application runs
   - **30-35 seconds**: E-paper display shows "Hello E-Reader"

Total expected boot-to-display time: **~30-35 seconds**

### What You Should See

#### On the E-Paper Display
After approximately 30 seconds, the e-paper display should:

1. **Initialization Phase** (5-8 seconds):
   - Display may flash black/white several times
   - This is the UC8176 controller initialization sequence
   - The screen clears to white

2. **Content Rendering** (2-3 seconds):
   - Text appears: "Hello E-Reader"
   - Second line: "Phase 1 Complete"
   - Text should be centered on the screen
   - Font: 8×16 pixel bitmap, black on white

3. **Full Refresh** (~5 seconds):
   - Display performs a full refresh cycle
   - Screen may flash black/white 2-3 times
   - Final image stabilizes

**Success Indicator**: Clear, readable black text on white background with no ghosting or artifacts.

#### Boot LED Indicators
- **PWR LED** (red): Solid red when powered
- **ACT LED** (green): Flashing during SD card activity
  - Rapid flashing during boot
  - Occasional flashing during init
  - Should settle to occasional brief flashes

## Connecting to Serial Console (Optional)

For debugging or monitoring boot messages, connect a USB-to-serial adapter.

### Hardware Setup

1. **Connection Pinout** (on Pi Zero W GPIO header):
   - **Pin 6** (GND) → Serial adapter GND
   - **Pin 8** (GPIO 14 / TXD) → Serial adapter RX
   - **Pin 10** (GPIO 15 / RXD) → Serial adapter TX
   - **WARNING**: Use 3.3V adapter only! 5V will damage the Pi.

2. **Power Separately**
   - Connect serial adapter to computer USB
   - Connect Pi power supply to Pi
   - Do NOT power the Pi from the serial adapter

### Software Setup

#### On Linux/Mac

```bash
# Install screen (if not already installed)
# Ubuntu/Debian:
sudo apt-get install screen

# macOS:
brew install screen

# Connect to serial console
# Linux (adapter appears as /dev/ttyUSB0 or /dev/ttyACM0):
sudo screen /dev/ttyUSB0 115200

# Mac (adapter appears as /dev/tty.usbserial-*):
sudo screen /dev/tty.usbserial-XXXXXXXX 115200

# To exit screen: Ctrl-A, then K, then Y
```

#### On Windows

1. **Install PuTTY**
   - Download from: https://www.putty.org/
   - Install and launch

2. **Configure Connection**
   - Connection type: Serial
   - Serial line: COM3 (or appropriate COM port from Device Manager)
   - Speed: 115200
   - Click "Open"

### Expected Serial Output

```
[    0.000000] Booting Linux on physical CPU 0x0
[    0.000000] Linux version 6.1.xx (buildroot@...)
[    0.000000] CPU: ARMv6-compatible processor [410fb767] revision 7
...
[    2.451234] EXT4-fs (mmcblk0p2): mounted filesystem with ordered data mode
...
Welcome to Buildroot
ereader login: root
Password: ereader

# whoami
root

# ls /usr/bin/display-test
/usr/bin/display-test

# dmesg | grep -i spi
[    1.234567] spi_bcm2835 20204000.spi: bound
```

### Login Credentials

**WARNING**: These are development credentials only. DO NOT use in production!

- **Username**: `root`
- **Password**: `ereader`

## Verification Checklist

After first boot, verify the following:

### Hardware Verification
- [ ] Pi Zero W PWR LED is solid red
- [ ] Pi Zero W ACT LED flashes during boot, then settles
- [ ] E-paper display shows clear text (no artifacts, ghosting, or dead pixels)
- [ ] Display text is readable: "Hello E-Reader" and "Phase 1 Complete"
- [ ] No smoke, unusual smells, or hot components

### System Verification (via serial console)
- [ ] Boot completes without kernel panics
- [ ] Root login works (username: `root`, password: `ereader`)
- [ ] Display test application ran: `grep display-test /var/log/messages`
- [ ] SPI device exists: `ls /dev/spidev0.0`
- [ ] GPIO devices exist: `ls /sys/class/gpio/`

### Display Verification
- [ ] Display initialized (no "SPI open failed" errors in `/var/log/display-test.log`)
- [ ] Full refresh completed (check log timestamps)
- [ ] Text centered correctly on 400×300 screen
- [ ] No communication errors with UC8176 controller

### Manual Test (via serial console)

Run the display test manually to verify it works:

```bash
# Stop the init script version (if running)
/etc/init.d/S99display-test stop

# Run display test manually
/usr/bin/display-test

# Expected output:
# Initializing e-paper display...
# Display initialized successfully
# Drawing text...
# Refreshing display...
# Display test complete!
# Entering sleep mode...

# Check the display - it should show the text
```

## Troubleshooting

### Display Shows Nothing / Stays White

**Possible Causes:**
1. Display not properly connected
2. SPI communication failure
3. Incorrect GPIO pins
4. Display initialization timeout

**Solutions:**
- Verify wiring against `[[WIRING_GUIDE]]`
- Check `/var/log/display-test.log` for errors
- Verify SPI device exists: `ls -l /dev/spidev0.0`
- Check GPIO pins are exported: `ls /sys/class/gpio/`
- Run display test manually with debug output

### Pi Won't Boot / No LED Activity

**Possible Causes:**
1. SD card not properly written
2. Corrupted image
3. Insufficient power supply
4. Faulty SD card

**Solutions:**
- Re-flash the SD card
- Verify image file integrity: `sha256sum buildroot/output/images/sdcard.img`
- Try a different power supply (must be 5V 2A minimum)
- Try a different SD card
- Check for loose connections

### Serial Console Shows Kernel Panic

**Possible Causes:**
1. Root filesystem corruption
2. Incorrect device tree
3. Missing kernel modules

**Solutions:**
- Re-flash SD card
- Rebuild the image: `./scripts/build.sh`
- Check build logs for errors
- Verify defconfig: `make -C buildroot ereader_rpi0w_defconfig`

### Display Test Fails with "SPI open failed"

**Possible Causes:**
1. SPI not enabled in kernel
2. Device tree overlay not loaded
3. Incorrect SPI device path

**Solutions:**
- Check kernel config: `zcat /proc/config.gz | grep CONFIG_SPI`
- Verify overlay loaded: `ls /sys/firmware/devicetree/base/soc/spi@*/spidev@*`
- Check `/boot/config.txt` contains: `dtoverlay=waveshare-epd-4_2`
- Verify device permissions: `ls -l /dev/spidev0.0`

### Display Shows Garbled/Partial Content

**Possible Causes:**
1. SPI speed too high
2. Loose wiring connections
3. Interference or noise
4. Incorrect controller initialization

**Solutions:**
- Lower SPI speed in device tree overlay (default: 4MHz)
- Check all wiring connections, especially ground
- Add short delays to SPI transactions
- Verify initialization sequence matches datasheet
- Check BUSY signal is properly connected

### Display Refresh is Very Slow (>10 seconds)

**Expected Behavior**: Full refresh should take ~5 seconds.

**Possible Causes:**
1. BUSY pin not connected or not working
2. Software polling without hardware signal
3. Incorrect timing in initialization

**Solutions:**
- Verify BUSY pin connection (GPIO 24)
- Check BUSY signal: `cat /sys/class/gpio/gpio24/value` (should change during refresh)
- Review initialization sequence in display-test source

### Boot Time Exceeds 60 Seconds

**Expected Boot Time**: ~30-35 seconds to display.

**Possible Causes:**
1. Slow SD card (Class 4 or lower)
2. Network services attempting to start (DHCP timeout)
3. Unnecessary services enabled

**Solutions:**
- Use Class 10 or UHS-I SD card
- Disable unnecessary services
- Check serial console for delayed services
- Review init script order: `ls -l /etc/init.d/S*`

## Post-Deployment Next Steps

Once the system is booting and the display is working:

1. **Test Multiple Power Cycles**
   - Power off, wait 10 seconds, power on
   - Verify consistent boot behavior
   - Check for SD card corruption (fsck if needed)

2. **Explore the System**
   - Log in via serial console
   - Check running processes: `ps aux`
   - Review logs: `cat /var/log/messages`
   - Test SPI communication: `ls -l /dev/spidev*`

3. **Prepare for Phase 2**
   - Review `[[PHASE_02_PLANNING]]` for button input implementation
   - Test GPIO button connections
   - Plan menu system architecture

4. **Document Your Setup**
   - Note any deviations from standard procedure
   - Record boot time measurements
   - Photograph the working display (for reference)
   - Update `[[PHASE_01_LOG]]` with observations

## Advanced: Manual Boot Partition Access

For debugging or customization, you can mount the SD card on your computer:

### On Linux

```bash
# Insert SD card into computer
lsblk  # Identify device (e.g., /dev/sdb)

# Mount boot partition (FAT32, first partition)
sudo mkdir -p /mnt/ereader-boot
sudo mount /dev/sdb1 /mnt/ereader-boot

# View boot files
ls -l /mnt/ereader-boot/
# - bootcode.bin, start.elf (Pi firmware)
# - zImage (kernel)
# - bcm2708-rpi-zero-w.dtb (device tree)
# - config.txt (Pi config)
# - overlays/ (device tree overlays)

# Edit config.txt to modify boot parameters
sudo nano /mnt/ereader-boot/config.txt

# Unmount when done
sudo umount /mnt/ereader-boot
```

### On Mac

```bash
# Insert SD card
diskutil list  # Identify device (e.g., /dev/disk2)

# Mount boot partition (if not auto-mounted)
sudo diskutil mount /dev/disk2s1

# Access via Finder or command line
cd /Volumes/boot  # (name may vary)
ls -l

# Unmount when done
sudo diskutil unmount /dev/disk2s1
```

### On Windows

Windows should auto-mount the boot partition (FAT32) and assign it a drive letter (e.g., E:).

- Open Explorer and navigate to the drive
- View files: `bootcode.bin`, `zImage`, `config.txt`, etc.
- Edit `config.txt` with Notepad (right-click → Edit)
- Safely eject when done

## Configuration Customization

### Modifying Boot Parameters

Edit `config.txt` on the boot partition:

```ini
# GPU memory (minimal for headless)
gpu_mem=16

# SPI speed customization
dtparam=spi=on
dtoverlay=waveshare-epd-4_2,speed=2000000  # 2MHz (slower, more reliable)

# Disable Bluetooth (power saving)
dtoverlay=disable-bt

# Enable UART (serial console)
enable_uart=1
```

### Disabling Display Test Auto-Run

To prevent the display test from running on boot:

```bash
# Via serial console:
/etc/init.d/S99display-test stop
rm /etc/init.d/S99display-test

# Or rename it:
mv /etc/init.d/S99display-test /etc/init.d/_S99display-test
```

### Changing Root Password

```bash
# Via serial console:
passwd root
# Enter new password twice
```

## Build Metrics and Performance

Expected metrics for reference:

| Metric | Expected Value |
|--------|---------------|
| SD card image size | 150-250 MB |
| Boot time (power-on to login prompt) | 25-30 seconds |
| Boot time (power-on to display test) | 30-35 seconds |
| Display initialization time | 5-8 seconds |
| Full display refresh time | ~5 seconds |
| Total power-on to working display | 35-45 seconds |
| Idle power consumption | ~100-150 mA @ 5V |
| Peak power during display refresh | ~200-300 mA @ 5V |

## Safety and Best Practices

### Electrical Safety
- Always power off before connecting/disconnecting components
- Verify voltage levels (3.3V logic, 5V power)
- Use proper ESD precautions when handling electronics
- Inspect for shorts before applying power

### SD Card Health
- Always use "Safely Remove" / `sync` before removing SD card
- Don't power off during write operations (check ACT LED)
- Use quality SD cards (SanDisk, Samsung, etc.)
- Keep backups of working images

### Development vs Production
- Change default passwords before any deployment
- Disable unnecessary services (serial console, SSH if enabled)
- Remove debug tools from production builds
- Implement proper security measures for network-connected devices

## Resources

- **Buildroot Documentation**: https://buildroot.org/documentation.html
- **Raspberry Pi Documentation**: https://www.raspberrypi.org/documentation/
- **Waveshare Wiki**: https://www.waveshare.com/wiki/4.2inch_e-Paper_Module
- **balenaEtcher**: https://www.balena.io/etcher/
- **Win32DiskImager**: https://sourceforge.net/projects/win32diskimager/
- **Rufus**: https://rufus.ie/

## Related Documentation

- `[[BUILD_PROCESS]]` - How to build the system image
- `[[WIRING_GUIDE]]` - Hardware connections between Pi and display
- `[[DISPLAY_SPECS]]` - Technical specifications of the e-paper display
- `[[DEVICE_TREE]]` - Device tree overlay configuration
- `[[CUSTOM_PACKAGES]]` - How the display-test package is integrated
- `[[PHASE_01_LOG]]` - Phase 1 development progress and decisions

---

**Document Version**: 1.0
**Last Updated**: 2026-01-13
**Tested On**: Raspberry Pi Zero W, Waveshare 4.2" e-paper Rev 2.2
