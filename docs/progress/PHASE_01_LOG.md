---
type: note
title: Phase 01 Progress Log - Foundation and Working Display
created: 2026-01-13
tags:
  - progress
  - phase-01
  - log
related:
  - "[[Phase-01-Foundation-and-Working-Display]]"
---

# Phase 01 Progress Log

This log tracks progress, decisions, and lessons learned during Phase 01: Foundation and Working Display.

## Phase Goal

Create a Buildroot-based minimal Linux system that boots on the Raspberry Pi Zero W and successfully displays "Hello E-Reader" on the Waveshare 4.2" e-paper display.

## Current Status

**Phase:** In Progress
**Started:** 2026-01-13
**Target Completion:** TBD

### Completed Tasks

- [x] 2026-01-13: Created project directory structure
  - Created all necessary folders: docs/, src/, configs/, board/, scripts/, package/

- [x] 2026-01-13: Created initial documentation
  - README.md with project overview and quick start guide
  - docs/hardware/WIRING_GUIDE.md with detailed pin connections
  - docs/hardware/BOM.md with complete parts list and pricing
  - docs/progress/PHASE_01_LOG.md (this file)

- [x] 2026-01-13: Researched Waveshare 4.2" display technical requirements
  - Created docs/hardware/DISPLAY_SPECS.md with comprehensive specifications
  - Documented UC8176/IL0398 controller, 400×300 resolution, 4 grayscale levels
  - Determined userspace SPI driver approach (no suitable kernel driver)
  - Documented initialization sequences and official resources

- [x] 2026-01-13: Set up Buildroot configuration for Raspberry Pi Zero W
  - Downloaded Buildroot 2024.02.2 (latest stable in 2024.02.x series)
  - Created configs/ereader_rpi0w_defconfig with custom configuration:
    * Target: ARM1176JZF-S (Pi Zero W CPU)
    * Kernel: 6.1 LTS series
    * Root filesystem: ext4 (256MB)
    * Hostname: "ereader", root password: "ereader" (dev only)
    * Serial console on ttyAMA0 (115200 baud)
    * Debug tools: strace, ltrace, gdb, gdbserver
  - Created board/ereader/ directory with board-specific configuration files:
    * config_ereader.txt (boot config with SPI enabled, minimal GPU memory)
    * post-build.sh (creates directories, sets hostname, adds welcome message)
    * post-image.sh (generates SD card image with genimage)
  - Created Makefile wrapper for common build commands (config, build, menuconfig, etc.)

- [x] 2026-01-13: Configure kernel for SPI and framebuffer support
  - Created configs/linux_ereader.fragment with kernel configuration options
  - Enabled SPI support (CONFIG_SPI, CONFIG_SPI_BCM2835, CONFIG_SPI_SPIDEV)
  - Enabled GPIO support (CONFIG_GPIO_CDEV, CONFIG_GPIO_SYSFS)
  - Enabled framebuffer support (CONFIG_FB, CONFIG_FB_DEFERRED_IO)
  - Enabled input event support (CONFIG_INPUT_EVDEV, CONFIG_KEYBOARD_GPIO)
  - Updated ereader_rpi0w_defconfig to reference the kernel fragment
  - Documented all kernel configuration choices and rationale in decision log

### In Progress

- [ ] Create device tree overlay for e-paper display
- [ ] Write minimal C test application for display verification
- [ ] Integrate test application into Buildroot
- [ ] Build complete system image
- [ ] Create SD card flashing documentation

### Not Started

- [ ] Hardware testing (requires physical hardware)
- [ ] Performance benchmarking
- [ ] Phase 01 completion documentation

## Technical Decisions

### Decision Log

#### 2026-01-13: Project Structure

**Decision:** Use standard Buildroot external tree structure

**Rationale:**
- Keep custom configuration separate from Buildroot source
- Use `configs/` for defconfigs
- Use `board/ereader/` for board-specific files and overlays
- Use `package/` for custom application packages

**Alternatives Considered:**
- In-tree modification of Buildroot (rejected: harder to maintain and update)
- Yocto/OpenEmbedded (rejected: steeper learning curve, overkill for this project)

#### 2026-01-13: Documentation Format

**Decision:** Use structured Markdown with YAML front matter and wiki-links

**Rationale:**
- Enables better organization and cross-referencing
- Compatible with Obsidian and other knowledge base tools
- Makes documentation more discoverable
- Supports future documentation graph visualization

**Format:**
```yaml
---
type: research | note | report | reference
title: Document Title
created: YYYY-MM-DD
tags:
  - relevant-tags
related:
  - "[[Other-Document]]"
---
```

#### 2026-01-13: Buildroot Version Selection

**Decision:** Use Buildroot 2024.02.2 (latest stable in 2024.02.x series)

**Rationale:**
- Latest stable release with security fixes and bug fixes
- Released May 7, 2024 - mature and well-tested
- LTS kernel 6.1.x support built-in
- Well-documented and widely used
- Active community support

**Alternatives Considered:**
- Buildroot 2024.11.x (rejected: too new, prefer proven stable release)
- Buildroot 2023.x (rejected: older, missing recent improvements)

**Configuration Choices:**
- **Init System:** BusyBox init (minimal, fast boot, low overhead)
- **Root Filesystem:** ext4 (journaling, reliability, 256MB size)
- **GPU Memory:** 16MB (minimal allocation, maximizes RAM for applications)
- **Debug Tools:** Included strace, ltrace, gdb for development phase

#### 2026-01-13: Pin Mapping for E-Paper Display

**Decision:** Use standard SPI0 interface with GPIOs 17, 24, 25 for control signals

**Pin Assignments:**
- VCC: Pin 1 (3.3V)
- GND: Pin 6 (GND)
- DIN (MOSI): Pin 19 (GPIO 10)
- CLK (SCLK): Pin 23 (GPIO 11)
- CS: Pin 24 (GPIO 8 / CE0)
- DC: Pin 22 (GPIO 25)
- RST: Pin 11 (GPIO 17)
- BUSY: Pin 18 (GPIO 24)

**Rationale:**
- Uses hardware SPI interface for best performance
- Control pins chosen to avoid conflicts with other common peripherals
- GPIO 17 and 25 are commonly used in Waveshare examples
- GPIO 24 is available and suitable for BUSY input

**Trade-offs:**
- Locks in SPI0 (SPI1 still available for expansion)
- GPIOs 17, 24, 25 unavailable for other uses

#### 2026-01-13: Kernel Configuration for E-Paper Display Support

**Decision:** Enable comprehensive SPI, GPIO, and framebuffer kernel support via configuration fragment

**Configuration File:** configs/linux_ereader.fragment

**Kernel Options Enabled and Rationale:**

**SPI Support (Required):**
- `CONFIG_SPI=y` - Core SPI subsystem support (foundation for all SPI functionality)
- `CONFIG_SPI_MASTER=y` - Pi Zero W acts as SPI master, e-paper display as slave
- `CONFIG_SPI_BCM2835=y` - BCM2835 SPI controller driver (hardware SPI on GPIO 10/11)
- `CONFIG_SPI_BCM2835_AUX=y` - Auxiliary SPI controller support
- `CONFIG_SPI_SPIDEV=y` - Userspace SPI device driver (creates /dev/spidev0.0)

**Rationale:** E-paper display communicates via 4-wire SPI. The userspace driver approach requires direct SPI access via /dev/spidev, which is provided by CONFIG_SPI_SPIDEV. Hardware SPI (BCM2835) is essential for reliable, fast communication at up to 10 MHz.

**GPIO Support (Required):**
- `CONFIG_GPIOLIB=y` - Base GPIO library infrastructure
- `CONFIG_GPIO_BCM2835=y` - Raspberry Pi Zero W GPIO controller driver
- `CONFIG_GPIO_CDEV=y` - Modern GPIO character device interface (/dev/gpiochip0)
- `CONFIG_GPIO_SYSFS=y` - Legacy GPIO sysfs interface (/sys/class/gpio)

**Rationale:** E-paper display requires 3 control GPIOs beyond SPI:
- DC (Data/Command select) - GPIO 25
- RST (Reset) - GPIO 17
- BUSY (Status output) - GPIO 24

Both character device (modern) and sysfs (legacy) interfaces are enabled for maximum compatibility with different libraries and debugging tools.

**Framebuffer Support (Future-proofing):**
- `CONFIG_FB=y` - Framebuffer device support
- `CONFIG_FB_DEFERRED_IO=y` - Deferred I/O for slow-refresh displays
- `CONFIG_FRAMEBUFFER_CONSOLE=y` - Text console on framebuffer
- `CONFIG_FB_SIMPLE=y` - Simple framebuffer support

**Rationale:** While Phase 1 uses userspace SPI driver, framebuffer support enables:
- Future kernel-space display driver integration
- Deferred I/O batching for efficient e-paper updates (avoid excessive refreshes)
- Boot console messages on e-paper display (nice-to-have)
- Standard Linux framebuffer API compatibility

**Input Device Support (Phase 2 Preparation):**
- `CONFIG_INPUT_EVDEV=y` - Input event interface (creates /dev/input/eventX)
- `CONFIG_KEYBOARD_GPIO=y` - GPIO-based button/key support
- `CONFIG_KEYBOARD_GPIO_POLLED=y` - Polling mode for buttons without interrupts

**Rationale:** Phase 2 will add physical GPIO buttons for navigation (next, previous, select, menu). Enabling now avoids kernel rebuild later. Buttons defined in device tree will appear as standard Linux input events, allowing use of standard input libraries.

**Device Tree Support:**
- `CONFIG_OF_OVERLAY=y` - Device tree overlay support (runtime configuration)
- `CONFIG_OF_CONFIGFS=y` - Apply overlays via configfs

**Rationale:** Device tree overlays allow hardware configuration without kernel recompilation. Essential for defining SPI device, GPIO pins, and future peripherals.

**Additional Features (Nice-to-have):**
- `CONFIG_I2C=y`, `CONFIG_I2C_BCM2835=y` - I2C support for future sensors/RTC
- `CONFIG_PWM=y`, `CONFIG_PWM_BCM2835=y` - PWM for future backlight or peripherals
- `CONFIG_WATCHDOG=y`, `CONFIG_BCM2835_WDT=y` - System reliability (auto-reboot on hang)
- `CONFIG_HW_RANDOM=y`, `CONFIG_HW_RANDOM_BCM2835=y` - Hardware RNG for entropy
- `CONFIG_DYNAMIC_DEBUG=y` - Enable debug messages at runtime (development)

**Implementation Approach:**
Kernel fragment file applied via BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES in Buildroot. This approach:
- Keeps custom options separate from base bcmrpi_defconfig
- Makes it easy to track e-reader-specific requirements
- Allows updating base kernel config independently
- Documents each option with inline comments

**Alternatives Considered:**
- Full custom kernel config (rejected: harder to maintain, loses Pi-specific optimizations)
- Compile drivers as modules (rejected: adds boot complexity, minimal size benefit)
- Skip framebuffer support (rejected: needed for future driver integration)

## Research Notes

### Display Technology

E-paper (electrophoretic) displays work by:
1. Using charged black and white particles suspended in oil
2. Applying electric field moves particles to create image
3. Image persists without power (bistable display)
4. Refresh required to change image (~2 seconds for full refresh)

**Key Characteristics:**
- Ultra-low power consumption (no power needed to maintain image)
- Excellent outdoor readability (reflective, not backlit)
- Slow refresh rate (not suitable for video)
- Limited grayscale levels (typically 2-4 levels)
- Wide viewing angle (>170°)

### Waveshare 4.2" Display Notes

**Model:** 4.2inch e-Paper Module (Rev 2.2)
**Controller IC:** IL0398 (or compatible)

**Technical Specs:**
- Resolution: 400x300 pixels (4:3 aspect ratio)
- Active Area: 84.8mm × 63.6mm
- Pixel Size: 0.212mm × 0.212mm
- Color: Black and White (1-bit per pixel)
- Interface: 4-wire SPI + 3 control GPIOs
- Operating Voltage: 3.3V (5V tolerant on VCC)
- Refresh Time: ~2 seconds (full), ~0.3 seconds (partial)
- Power Consumption: 26mA (refresh), <0.1mA (standby)

**References:**
- [Waveshare Wiki](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- IL0398 Datasheet (to be added)

### Raspberry Pi Zero W Specifications

**CPU:** BCM2835 (ARM1176JZF-S @ 1GHz, single-core)
**RAM:** 512MB LPDDR2
**Storage:** microSD card
**Interfaces:**
- 1× mini HDMI
- 1× micro-USB OTG
- 1× micro-USB power
- 40-pin GPIO header (unpopulated on some models)
- CSI camera connector

**Power Consumption:**
- Idle: ~100-120mA @ 5V
- CPU load: ~150-200mA @ 5V
- WiFi active: +50-100mA
- Total system: ~400-500mA with display (during refresh)

**Boot Time:**
- Standard Raspbian: 20-40 seconds
- Minimal Buildroot: 5-10 seconds (target)

## Challenges and Solutions

### Challenge: Display Driver Selection

**Issue:** Multiple e-paper driver options exist in Linux kernel

**Options:**
1. **IT8951 driver** - For IT8951 controller (different hardware)
2. **fb_ili9341** - For ILI9341 TFT displays (not compatible)
3. **Custom SPI driver** - Write our own driver
4. **Userspace SPI** - Use /dev/spidev from userspace

**Decision:** Start with userspace SPI approach (option 4)

**Rationale:**
- Fastest path to working display
- Easier debugging and iteration
- Reference code available from Waveshare
- Can migrate to kernel driver later if needed

**Trade-offs:**
- Userspace has higher latency
- No integration with Linux framebuffer console
- Need manual framebuffer management

### Challenge: Cross-Compilation Toolchain

**Issue:** Need to compile C code for ARM on x86 development machine

**Solution:** Use Buildroot's built-in toolchain

**Rationale:**
- Buildroot generates compatible cross-compiler automatically
- Ensures ABI compatibility with target system
- Includes all necessary libraries

**Alternative:** Install separate ARM toolchain (rejected: extra complexity)

## Lessons Learned

### Documentation Best Practices

1. **Start with documentation:** Creating docs first helps clarify requirements
2. **Use structured format:** YAML frontmatter makes docs more organized
3. **Cross-reference everything:** Wiki-links create knowledge graph
4. **Be specific:** Include part numbers, pin numbers, exact versions

### Project Organization

1. **Separate concerns:** Keep Buildroot, custom packages, and docs in separate directories
2. **Plan for testing:** Create test infrastructure early (display-test package)
3. **Version everything:** Use git from the start

## Performance Targets

### Boot Time Goals

- **Power-on to kernel boot:** <5 seconds
- **Kernel boot to init:** <2 seconds
- **Init to display test:** <3 seconds
- **Total boot time:** <10 seconds (target)

Comparison:
- Raspbian: 30-40 seconds
- Buildroot minimal: 5-10 seconds (expected)

### Display Refresh Targets

- **Full refresh:** 2 seconds (hardware limit)
- **Partial refresh:** 0.3-0.5 seconds (hardware limit)
- **Framebuffer update:** <100ms (software target)

### Memory Usage Targets

- **Kernel:** <50MB
- **Userspace:** <30MB
- **Total system:** <100MB RAM usage
- **Available for apps:** 400+ MB

## Next Steps

### Immediate (Next Session)

1. Research Waveshare 4.2" display driver requirements
   - Find Linux kernel driver options
   - Review Waveshare example code
   - Document initialization sequence

2. Download and configure Buildroot
   - Get Buildroot 2024.02.x
   - Create custom defconfig for Pi Zero W
   - Configure kernel options

### Near-Term (This Week)

1. Create device tree overlay for display
2. Write basic display test application
3. Build and test on hardware

### Future (Phase 2+)

1. Add button input support
2. Implement e-reader application
3. Add EPUB/PDF parsing

## Open Questions

1. **Q:** Should we use full or partial refresh for page turns?
   - **A:** TBD - test both and measure impact on battery life and user experience

2. **Q:** What Linux kernel version should we target?
   - **A:** Latest LTS (6.1.x or 6.6.x as of 2026-01-13)

3. **Q:** Should display driver be kernel module or userspace?
   - **A:** Starting with userspace for rapid development

4. **Q:** What init system? (systemd, sysvinit, BusyBox init)
   - **A:** BusyBox init (minimal, fast boot)

## Reference Links

### Hardware

- [Waveshare 4.2" E-Paper Wiki](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- [Raspberry Pi Zero W Product Page](https://www.raspberrypi.com/products/raspberry-pi-zero-w/)
- [RPi GPIO Pinout](https://pinout.xyz/)

### Software

- [Buildroot User Manual](https://buildroot.org/downloads/manual/manual.html)
- [Raspberry Pi Linux Kernel](https://github.com/raspberrypi/linux)
- [Device Tree Documentation](https://www.kernel.org/doc/Documentation/devicetree/)

### Example Code

- [Waveshare E-Paper Examples](https://github.com/waveshare/e-Paper)
- [BCM2835 Library](http://www.airspayce.com/mikem/bcm2835/)
- [Linux SPI Documentation](https://www.kernel.org/doc/Documentation/spi/)

## Revision History

- 2026-01-13: Initial log created
- 2026-01-13: Added project structure and documentation completion
- 2026-01-13: Documented technical decisions (project structure, pin mapping, documentation format)
- 2026-01-13: Completed Buildroot setup - downloaded 2024.02.2, created custom defconfig and board files
- 2026-01-13: Added Buildroot version selection decision and configuration details
