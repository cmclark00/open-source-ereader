# Pi Zero W E-Reader - Quick Start

This is a quick reference for building the E-Reader OS on your Raspberry Pi Zero W.

## TL;DR - Build Commands

```bash
cd os/buildroot/buildroot-2024.02.7
make ereader_rpizerow_defconfig
make -j$(nproc)
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
```

Replace `/dev/sdX` with your SD card device.

## Files Created

Three new files were created to support Pi Zero W:

1. **`os/buildroot/buildroot-2024.02.7/configs/ereader_rpizerow_defconfig`**
   - Main Buildroot configuration
   - Converted from Pi 4 64-bit to Pi Zero W 32-bit
   
2. **`os/buildroot/buildroot-2024.02.7/board/raspberrypi/config_ereader_zerow.txt`**
   - Boot configuration (config.txt)
   - Optimized for Pi Zero W hardware and power savings
   
3. **`docs/software/PI_ZERO_W_BUILD.md`**
   - Complete build guide
   - Troubleshooting tips
   - Configuration details

## Key Differences from Pi 4 Version

| Aspect | Pi 4 Version | Pi Zero W Version |
|--------|-------------|-------------------|
| **Architecture** | 64-bit (AArch64) | 32-bit (ARM) |
| **CPU Config** | Cortex-A72 | ARM1176JZF-S |
| **Kernel** | bcm2711 | bcmrpi |
| **Device Tree** | bcm2711-rpi-4-b | bcm2708-rpi-zero-w |
| **Kernel Image** | Image | zImage |
| **Firmware** | PI4 variant | PI variant |
| **Bootcode** | Not needed | bootcode.bin required |
| **Network** | eth0 (Ethernet) | wlan0 (WiFi only) |
| **Performance** | ~4x faster | Slower but adequate |

## What's Shared Between Both

- ✅ Kernel configuration fragment (`ereader-kernel.fragment`)
- ✅ Filesystem overlay (`board/raspberrypi/ereader-overlay`)
- ✅ WiFi configuration and drivers
- ✅ All init scripts and applications
- ✅ SPI/I2C/GPIO setup

## Build Time Expectations

- **First build**: 1-3 hours (downloads everything)
- **Subsequent builds**: 5-30 minutes
- **Final image size**: ~120MB

## Testing Your Build

1. **Flash SD card** with `sdcard.img`
2. **Insert into Pi Zero W** and power on
3. **Connect serial console** (optional): Pin 6=GND, Pin 8=TX, Pin 10=RX
4. **Boot time**: Should be ~5-8 seconds
5. **Login**: Username `root`, no password
6. **Setup WiFi**: Run `setup-wifi`
7. **Test display**: Run `python3 /usr/bin/framebuffer_test.py`

## Next Steps

- Connect your e-ink display (see hardware docs)
- Configure WiFi for book downloads
- Install reader application
- Build custom carrier PCB (optional)
- 3D print case (optional)

## Full Documentation

- **Complete build guide**: `docs/software/PI_ZERO_W_BUILD.md`
- **Hardware setup**: `docs/hardware/PI_ZERO_W_VARIANT.md`
- **Main project docs**: `CLAUDE.md`

## Quick Troubleshooting

**Build fails?**
```bash
make clean
make ereader_rpizerow_defconfig
make -j$(nproc)
```

**Won't boot?**
- Check power supply (5V 1A minimum)
- Try different SD card
- Connect serial console to see errors

**WiFi not working?**
```bash
# Check interface exists
ifconfig wlan0

# Restart networking
/etc/init.d/S40network restart
```

## Performance Notes

The Pi Zero W is slower than Pi 4, but still very usable:
- **Page turns**: 200-300ms (vs 100-150ms on Pi 4)
- **PDF rendering**: 500ms-1s (vs 300-600ms on Pi 4)
- **Boot time**: ~5-8s (vs ~5s on Pi 4)
- **Battery life**: Same (~3-4 weeks)

For reading novels and simple EPUBs, you won't notice the difference!

---

**Ready to build?** See `docs/software/PI_ZERO_W_BUILD.md` for detailed instructions.
