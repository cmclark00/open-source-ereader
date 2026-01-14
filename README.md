# Open Source E-Reader

A minimal, open-source e-reader built on Raspberry Pi Zero W with a Waveshare 4.2" e-paper display. This project provides a complete, customizable e-reading experience using only open-source components and software.

## Project Overview

This project creates a functional e-reader device using:
- **Raspberry Pi Zero W** - Low-power ARM-based single-board computer
- **Waveshare 4.2" e-paper display (rev 2.2)** - 400x300 pixel black and white display
- **Custom Buildroot-based Linux OS** - Minimal, fast-booting embedded Linux
- **Physical buttons** for navigation (no touchscreen required)

The goal is to build a minimal, open-source e-reader that can read EPUB and PDF files, with long battery life and a distraction-free reading experience.

## Hardware Requirements

### Core Components
- **Raspberry Pi Zero W** (or Pi Zero 2 W for better performance)
- **Waveshare 4.2" E-Paper Display Module (Rev 2.2)**
  - Resolution: 400x300 pixels
  - Interface: SPI
  - Supports partial refresh
- **MicroSD Card** (8GB minimum, Class 10 recommended)
- **GPIO Buttons** (6x tactile buttons for Phase 2+)
- **Power Supply**: 5V micro-USB (2A minimum recommended)

## Current Status

**Phase 1: Foundation and Working Display** - In Progress

This project is in early development. Currently working on establishing the Buildroot-based minimal Linux system that boots on the Raspberry Pi Zero W and displays text on the Waveshare 4.2" e-paper display.

## Quick Start Guide

> **Note:** This project is under active development. The quick start guide will be updated as features are implemented.

### Prerequisites

- Raspberry Pi Zero W (or Pi Zero 2 W)
- Waveshare 4.2" e-paper display (Rev 2.2)
- MicroSD card (8GB minimum, Class 10 recommended)
- Power supply (5V, 2A minimum)
- Basic soldering skills for GPIO connections
- Linux development machine for building the image (or WSL2 on Windows)

### Hardware Setup

1. Wire the Waveshare 4.2" e-paper display to the Raspberry Pi Zero W according to the wiring guide in `docs/hardware/WIRING_GUIDE.md`
2. Insert the microSD card (4GB minimum recommended) into the Pi Zero W
3. Connect power supply (5V, 2.5A recommended for stable operation)

### Quick Start

1. **Download or Build the Image**
   - Pre-built images: [Coming soon]
   - Or build from source (see Building from Source below)

2. **Flash to SD Card**
   - Linux/Mac: `dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress`
   - Windows: Use Win32DiskImager or Rufus
   - See `docs/deployment/FLASHING_GUIDE.md` for detailed instructions

3. **Boot the Device**
   - Insert SD card into Raspberry Pi Zero W
   - Connect the Waveshare 4.2" e-paper display via SPI
   - Connect USB power (5V 2A recommended)
   - The display should show "Hello E-Reader" after ~30-60 seconds

4. **Serial Console Access** (optional, for debugging)
   - Connect USB-to-TTL adapter to GPIO pins (see WIRING_GUIDE.md)
   - Baudrate: 115200
   - Login: root / ereader

## Hardware Requirements

- **Raspberry Pi Zero W** (v1.1)
- **Waveshare 4.2" E-Paper Display Module (rev 2.2)**
  - Resolution: 400x300 pixels
  - Grayscale: 2-bit (4 colors: black, dark gray, light gray, white)
  - Interface: SPI
- **MicroSD card** (8GB minimum, Class 10 recommended)
- **Power supply**: 5V 2A USB micro power adapter
- **Optional**: USB-to-TTL serial adapter for debugging

## Quick Start

1. **Flash SD Card**: Write the pre-built image to a microSD card (see `docs/deployment/FLASHING_GUIDE.md`)
2. **Wire Display**: Connect Waveshare 4.2" e-paper display to Pi Zero W following `docs/hardware/WIRING_GUIDE.md`
3. **Boot**: Insert SD card, connect power (5V/1A minimum), wait ~30 seconds for "Hello E-Reader" to appear
4. **Optional**: Connect serial console to ttyAMA0 for debugging (see deployment docs)

## Current Status

This project is in **Phase 01: Foundation and Working Display**. The current phase focuses on:
- Buildroot-based minimal Linux system for Raspberry Pi Zero W
- Basic e-paper display driver and SPI communication
- Verification that hardware components work together

## Hardware Requirements

### Required Components
- **Raspberry Pi Zero W** (or Pi Zero 2 W for better performance)
- **Waveshare 4.2" E-Paper Display (rev 2.2)** - Black/White, 400x300 resolution
- MicroSD card (8GB minimum, Class 10 recommended)
- 5V 2A Micro-USB power supply
- Micro USB cable (for power and/or serial console)
- USB-to-TTL serial adapter (optional, for debugging via UART)

### Display Specifications
- Model: Waveshare 4.2" E-Paper Display Module (rev 2.2)
- Resolution: 400x300 pixels
- Color: Black and White (1-bit)
- Interface: SPI
- Controller: IL0398 (or compatible)

## Quick Start Guide

### Prerequisites
- Raspberry Pi Zero W
- Waveshare 4.2" E-Paper Display (rev 2.2)
- 8GB+ microSD card
- USB serial adapter (for debugging, optional)
- Linux development machine for building (or WSL2 on Windows)

### Hardware Setup

1. Connect the Waveshare 4.2" e-paper display to the Raspberry Pi Zero W via SPI
   - Refer to `docs/hardware/WIRING_GUIDE.md` for detailed pin connections
2. Insert SD card into your computer for flashing

### Software Build

1. Clone this repository:
   ```bash
   git clone <repository-url>
   cd open-source-ereader
   ```

2. Build the system image (requires Linux with >4GB RAM, 30GB+ disk space):
   ```bash
   cd buildroot
   make ereader_rpi0w_defconfig
   make
   ```

   First build takes 1-2 hours depending on your system.

3. Flash the SD card:
   ```bash
   sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
   ```
   Replace `/dev/sdX` with your SD card device.

## Current Status

This is an early-stage project. Phase 1 is currently in development.

**Working:**
- Initial project structure
- Documentation framework

**In Progress:**
- Buildroot configuration
- E-paper display driver integration
- Basic display test application

**Not Yet Implemented:**
- Button input system
- E-reader application
- EPUB/PDF support
- WiFi connectivity

## Hardware Requirements

### Core Components

- **Raspberry Pi Zero W** (or Zero 2 W for better performance)
  - BCM2835 SoC with ARM1176JZF-S CPU (700MHz)
  - 512MB RAM
  - Built-in WiFi and Bluetooth
  - 40-pin GPIO header

- **Waveshare 4.2" E-Paper Display Module (Rev 2.2)**
  - Resolution: 400x300 pixels
  - Interface: SPI
  - Color: Black/White (or Red/Black/White for color version)
  - Partial refresh support
  - Ultra-low power consumption
  - Viewing angle: >170°

- **MicroSD Card** (8GB minimum, Class 10 recommended)

- **Power Supply** (5V 2A micro-USB)

- **GPIO Buttons** (4-6 tactile push buttons for navigation)

## Quick Start Guide

### Prerequisites

- Linux development environment (Ubuntu 20.04+ recommended)
- 8GB+ free disk space for Buildroot
- SD card reader
- Basic knowledge of embedded Linux

### Building the System

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/open-source-ereader.git
   cd open-source-ereader
   ```

2. **Download and configure Buildroot:**
   ```bash
   cd buildroot
   make ereader_rpi0w_defconfig
   ```

3. **Build the system image:**
   ```bash
   make
   ```
   This will take 1-2 hours on the first build.

4. **Flash to SD card:**
   ```bash
   sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
   sync
   ```
   Replace `/dev/sdX` with your SD card device.

5. **Boot and Test:**
   - Insert SD card into Pi Zero W
   - Connect power
   - Display should show "Hello E-Reader" after boot

### Serial Console Access

For debugging, connect a USB-to-serial adapter to the Pi Zero W GPIO pins:
- TX (GPIO 14, pin 8)
- RX (GPIO 15, pin 10)
- GND (pin 6)

Use 115200 baud rate:
```bash
screen /dev/ttyUSB0 115200
```

## Project Status

**Current Phase:** Phase 01 - Foundation and Working Display

See [Phase 01 Progress Log](docs/progress/PHASE_01_LOG.md) for detailed status.

## Hardware Requirements

### Core Components

- **Raspberry Pi Zero W** (ARM1176JZF-S, 512MB RAM, WiFi/BT)
- **Waveshare 4.2" E-Paper Display Module (Rev 2.2)**
  - Resolution: 400x300 pixels
  - Interface: SPI
  - Partial refresh support
  - 4-level grayscale

### Detailed Information

- [Bill of Materials](docs/hardware/BOM.md) - Complete parts list with suppliers
- [Wiring Guide](docs/hardware/WIRING_GUIDE.md) - Pin connections and assembly
- [Display Specifications](docs/hardware/DISPLAY_SPECS.md) - Technical details

## Software Architecture

- **Operating System:** Custom Buildroot-based minimal Linux
- **Kernel:** Linux 6.1 LTS with SPI and framebuffer support
- **Display Driver:** Custom SPI-based e-paper driver
- **Application:** C-based e-reader with framebuffer rendering

## Documentation

- **Phase Documentation:** See `Auto Run Docs/Initiation/` for development roadmap
- **Build Guide:** [docs/buildroot/CUSTOM_PACKAGES.md](docs/buildroot/CUSTOM_PACKAGES.md)
- **Deployment:** [docs/deployment/FLASHING_GUIDE.md](docs/deployment/FLASHING_GUIDE.md)
- **Progress Logs:** [docs/progress/](docs/progress/)

## Development Phases

1. **Phase 01:** Foundation and Working Display *(Current)*
2. **Phase 02:** GPIO Button Input
3. **Phase 03:** Basic E-Reader Application
4. **Phase 04:** EPUB and PDF Support
5. **Phase 05:** Polish and Advanced Features
6. **Phase 06:** WiFi and Network Features
7. **Phase 07:** Release Preparation

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