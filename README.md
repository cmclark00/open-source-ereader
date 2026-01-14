# Open Source E-Reader

A minimal, open-source e-reader built on Raspberry Pi Zero W with a Waveshare 4.2" e-paper display. This project provides a complete, customizable e-reading experience using only open-source components and software.

## Project Overview

This project creates a functional e-reader device using:
- **Raspberry Pi Zero W** - Low-power ARM-based single-board computer (512MB RAM, 700MHz ARM CPU)
- **Waveshare 4.2" e-paper display (rev 2.2)** - 400×300 pixel black and white display
- **Custom Buildroot-based Linux OS** - Minimal, fast-booting embedded Linux (kernel 6.1 LTS)
- **Physical buttons** for navigation (5 GPIO buttons, no touchscreen required)

The goal is to build a minimal, open-source e-reader that boots directly to a reading interface, with long battery life and a distraction-free reading experience.

## Current Status

**Phase 3: Basic E-Reader Application** - ✅ Complete

The e-reader now has a fully functional reading application! You can:
- ✅ Boot directly to a book library menu
- ✅ Browse and select books from `/books/` directory
- ✅ Read plain text (`.txt`) files with automatic pagination
- ✅ Navigate pages with physical buttons (UP/DOWN)
- ✅ Automatic bookmark saving and resumption
- ✅ Display formatted text on 400×300 e-paper display

**What's Working:**
- Complete Buildroot system that boots in ~10-30 seconds
- E-paper display driver with SPI communication
- GPIO button input with debouncing and event detection
- Book library management (scanning, loading, sorting)
- Text rendering engine with word wrapping and pagination
- Menu system with scrolling and selection
- Reading view with page navigation
- Bookmark persistence across reboots

**Current Limitations:**
- Only supports plain text (`.txt`) files (EPUB/PDF planned for Phase 4)
- Maximum book size: 10 MB
- Fixed font (8×16 bitmap font)
- No WiFi features yet (planned for Phase 6)

See [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for complete usage instructions.

## Hardware Requirements

### Core Components
- **Raspberry Pi Zero W** (or Pi Zero 2 W for better performance)
  - BCM2835 SoC with ARM1176JZF-S CPU (700MHz)
  - 512MB RAM
  - Built-in WiFi and Bluetooth (currently unused)
  - 40-pin GPIO header
- **Waveshare 4.2" E-Paper Display Module (Rev 2.2)**
  - Resolution: 400×300 pixels
  - Interface: SPI
  - Color: Black/White (1-bit)
  - Controller: IL0398 or compatible
  - Ultra-low power consumption
- **GPIO Buttons** (5x tactile push buttons)
  - UP, DOWN, SELECT, BACK, MENU
  - Connected to GPIO pins 17, 27, 22, 23, 24
- **MicroSD Card** (8GB minimum, Class 10 recommended)
- **Power Supply**: 5V 2A micro-USB

### Detailed Information
- [Bill of Materials](docs/hardware/BOM.md) - Complete parts list with suppliers
- [Wiring Guide](docs/hardware/WIRING_GUIDE.md) - Pin connections and assembly
- [Display Specifications](docs/hardware/DISPLAY_SPECS.md) - Technical details
- [Button Layout](docs/hardware/BUTTON_LAYOUT.md) - Button wiring and GPIO mappings

## Quick Start Guide

### For Users (Pre-Built Image)

1. **Download Image**: [Pre-built images coming soon]
2. **Flash to SD Card**: Use `dd`, Win32DiskImager, or balenaEtcher
3. **Add Books**: Copy `.txt` files to `/books/` directory on the SD card
4. **Connect Hardware**: Wire display and buttons (see [WIRING_GUIDE.md](docs/hardware/WIRING_GUIDE.md))
5. **Boot**: Insert SD card, power on, wait 10-30 seconds for library menu

See [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for complete usage instructions.

### For Developers (Build from Source)

```bash
# 1. Clone repository
git clone <repository-url>
cd open-source-ereader

# 2. Configure Buildroot
cd buildroot
make ereader_rpi0w_defconfig

# 3. Build (1-2 hours first time)
make

# 4. Flash to SD card
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress && sync
```

See [docs/buildroot/BUILD_PROCESS.md](docs/buildroot/BUILD_PROCESS.md) for detailed build instructions.

### Serial Console (Optional, for Debugging)

Connect USB-to-TTL adapter: TX→GPIO 14, RX→GPIO 15, GND→GND. Use 115200 baud.

```bash
screen /dev/ttyUSB0 115200
# Login: root / Password: ereader
```

## Software Architecture

- **Operating System:** Custom Buildroot-based minimal Linux (kernel 6.1 LTS)
- **Display Driver:** Custom SPI-based e-paper driver with direct framebuffer access
- **Input System:** GPIO button driver with debouncing and event detection
- **Application:** C-based e-reader (v0.3.0) with:
  - Text rendering engine (8×16 bitmap font, word wrapping, pagination)
  - Book manager (scan, load, bookmark persistence)
  - Menu system (library browser with scrolling)
  - Reading view (page navigation, status bar)
- **Boot Time:** ~10-30 seconds from power-on to library menu
- **Architecture:** See [docs/architecture/EREADER_DESIGN.md](docs/architecture/EREADER_DESIGN.md)

## Documentation

### User Documentation
- **[User Guide](docs/USER_GUIDE.md)** - Complete usage instructions, button controls, adding books
- **[Flashing Guide](docs/deployment/FLASHING_GUIDE.md)** - How to flash SD card images

### Hardware Documentation
- **[Wiring Guide](docs/hardware/WIRING_GUIDE.md)** - Display and button connections
- **[Bill of Materials](docs/hardware/BOM.md)** - Parts list with suppliers
- **[Display Specs](docs/hardware/DISPLAY_SPECS.md)** - Technical display details
- **[Button Layout](docs/hardware/BUTTON_LAYOUT.md)** - GPIO button mappings

### Developer Documentation
- **[Build Process](docs/buildroot/BUILD_PROCESS.md)** - Building from source
- **[Custom Packages](docs/buildroot/CUSTOM_PACKAGES.md)** - Buildroot package creation
- **[Architecture Design](docs/architecture/EREADER_DESIGN.md)** - Software architecture
- **[Input System](docs/architecture/INPUT_SYSTEM.md)** - Button input design
- **[Progress Logs](docs/progress/)** - Phase completion logs

### Project Planning
- **Phase Roadmaps:** See `Auto Run Docs/Initiation/` for detailed task lists

## Development Phases

1. **Phase 01:** Foundation and Working Display ✅ *(Complete)*
   - Buildroot-based Linux system
   - E-paper display driver and SPI communication
   - Basic "Hello E-Reader" test application

2. **Phase 02:** GPIO Button Input ✅ *(Complete)*
   - GPIO button driver with debouncing
   - Event-based input system
   - Button test application

3. **Phase 03:** Basic E-Reader Application ✅ *(Complete)*
   - Text rendering engine with pagination
   - Book library management
   - Menu system and reading view
   - Bookmark persistence
   - Boot-to-reader experience

4. **Phase 04:** EPUB and PDF Support *(Next)*
   - EPUB parser and renderer
   - PDF text extraction
   - Enhanced formatting support

5. **Phase 05:** Polish and Advanced Features
   - Font size adjustment
   - Reading statistics
   - Dictionary lookup
   - Enhanced UI/UX

6. **Phase 06:** WiFi and Network Features
   - WiFi configuration
   - Online book downloads
   - Cloud sync

7. **Phase 07:** Release Preparation
   - Final testing and optimization
   - Pre-built images
   - Complete documentation

## License

This project is open source. License information to be added.

## Contributing

This is an open-source educational project. Contributions, bug reports, and suggestions are welcome.

## Support

For issues and questions:
- Check the progress logs in `docs/progress/`
- Review the phase documentation in `Auto Run Docs/Initiation/`
- File an issue on GitHub

## Acknowledgments

- Waveshare for e-paper display modules and documentation
- Buildroot project for embedded Linux build system
- Raspberry Pi Foundation for affordable hardware platform