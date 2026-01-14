# E-Paper Display Test Application

Simple test application to verify the Waveshare 4.2" e-paper display is working correctly with the Raspberry Pi Zero W.

## Overview

This application demonstrates basic e-paper display functionality:
- SPI communication with the UC8176/IL0398 controller
- GPIO control for RST, DC, and BUSY pins
- Display initialization sequence
- Framebuffer management
- Text rendering with 8x16 bitmap font
- Full refresh operation

## Files

- `main.c` - Test application entry point, displays "Hello E-Reader"
- `epd_driver.c` - E-paper display driver implementation
- `epd_driver.h` - Driver header with function prototypes and pin definitions
- `font.h` - 8x16 bitmap font for text rendering
- `Makefile` - Build configuration for cross-compilation

## Building

### Cross-compilation with Buildroot Toolchain

When integrated as a Buildroot package (see Phase 1 next task), the package system handles compilation automatically.

### Manual Cross-compilation

```bash
# Set up Buildroot toolchain path
export PATH=/path/to/buildroot/output/host/bin:$PATH

# Build with cross-compiler
make CROSS_COMPILE=arm-linux-gnueabihf-

# Install to target filesystem
make install DESTDIR=/path/to/buildroot/output/target
```

### Native Build (for development/testing on x86)

```bash
# Build with host compiler (won't work on x86, but useful for syntax checking)
make

# Debug build
make BUILD_TYPE=debug
```

## Hardware Requirements

- Raspberry Pi Zero W
- Waveshare 4.2" e-paper display (Rev 2.2)
- Proper SPI and GPIO connections (see `docs/hardware/WIRING_GUIDE.md`)

## Running on Target

1. Copy the `display-test` binary to the Raspberry Pi:
   ```bash
   scp display-test root@ereader:/usr/bin/
   ```

2. SSH into the device:
   ```bash
   ssh root@ereader
   ```

3. Run the test:
   ```bash
   display-test
   ```

4. Expected output:
   - Console messages showing initialization progress
   - "Hello E-Reader" displayed on the e-paper screen
   - "Phase 1 Complete" subtitle
   - ~5 second refresh time

## Permissions

The application requires:
- Access to `/dev/spidev0.0` (SPI device)
- Access to GPIO pins via sysfs (`/sys/class/gpio`)

Run as root or ensure proper permissions are set:
```bash
# Add user to gpio and spi groups (if they exist)
usermod -a -G gpio,spi username

# Or run as root
sudo display-test
```

## Troubleshooting

### "Failed to open SPI device"
- Ensure SPI is enabled in device tree
- Check `/dev/spidev0.0` exists
- Verify device tree overlay is loaded: `dtoverlay -l`

### "Failed to export GPIO pins"
- GPIO pins may already be exported (normal if running multiple times)
- Check `/sys/class/gpio/` directory
- Ensure GPIO permissions are correct

### Display shows nothing
- Verify physical connections (see wiring guide)
- Check power supply (5V 2A recommended)
- Ensure BUSY pin is connected and readable
- Try manual reset by power cycling the display

### Display shows corruption
- Reduce SPI speed in `epd_driver.h` (try 2MHz instead of 4MHz)
- Check for loose connections on SPI bus
- Ensure proper grounding

## Expected Behavior

1. **Initialization**: ~1 second
2. **Clear to white**: ~5 seconds (full refresh)
3. **Draw text**: <1 second (framebuffer only)
4. **Refresh display**: ~5 seconds (full refresh)
5. **Sleep mode**: <1 second

Total runtime: ~12-15 seconds

## Technical Details

### Display Specifications
- Resolution: 400×300 pixels
- Controller: UC8176/IL0398
- Interface: 4-wire SPI
- Colors: Black/White (1-bit per pixel)
- Refresh time: ~5 seconds (full refresh)

### GPIO Pin Mapping
- RST: GPIO 17 (Pin 11)
- DC: GPIO 25 (Pin 22)
- BUSY: GPIO 24 (Pin 18)
- CS: GPIO 8 (Pin 24 - CE0)
- MOSI: GPIO 10 (Pin 19)
- SCLK: GPIO 11 (Pin 23)

### SPI Configuration
- Device: `/dev/spidev0.0`
- Speed: 4 MHz
- Mode: 0 (CPOL=0, CPHA=0)
- Bits per word: 8

## Next Steps

After verifying this test works:
1. Integrate as Buildroot package (next task in Phase 1)
2. Configure init system to run on boot
3. Build complete system image
4. Flash to SD card and test on hardware

## References

- [Waveshare 4.2" Display Documentation](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- [UC8176 Datasheet](https://cursedhardware.github.io/epd-driver-ic/UC8176c.pdf)
- Project documentation: `docs/hardware/DISPLAY_SPECS.md`
