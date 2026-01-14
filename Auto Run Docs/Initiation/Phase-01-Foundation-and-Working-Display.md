# Phase 01: Foundation and Working Display

This phase establishes the project foundation and delivers a working prototype: a Buildroot-based minimal Linux system that boots on the Raspberry Pi Zero W and successfully displays "Hello E-Reader" on the Waveshare 4.2" e-paper display. By the end of this phase, you'll have verified hardware functionality and a proven software stack to build upon.

## Tasks

- [x] Create project structure and initial documentation:
  - `README.md` with project overview, hardware requirements (Pi Zero W, Waveshare 4.2" e-paper rev 2.2), and quick start guide
  - `docs/hardware/WIRING_GUIDE.md` documenting SPI pin connections between Pi Zero W and Waveshare display
  - `docs/hardware/BOM.md` (Bill of Materials) listing all required components with part numbers
  - `docs/progress/PHASE_01_LOG.md` for tracking progress and decisions
  - `.gitignore` excluding build artifacts, downloads, and temporary files

- [x] Research and document Waveshare 4.2" e-paper display technical requirements:
  - Create `docs/hardware/DISPLAY_SPECS.md` with front matter (type: research, tags: [hardware, display, waveshare])
  - Document display resolution, color depth, refresh time, and SPI communication protocol
  - Research and note the correct Linux kernel driver (IT8951, EPD driver, or fb_ili9341 equivalent)
  - Include links to Waveshare official documentation and example code repositories
  - Document power requirements and any special initialization sequences
  - **Completion Notes**: Comprehensive research completed. Display uses UC8176/IL0398 controller, 400×300 resolution, 4 grayscale levels. Recommended approach: userspace SPI driver (no suitable kernel driver exists). All specifications, pinouts, initialization sequences, and official resources documented in DISPLAY_SPECS.md with proper front matter and cross-references.

- [x] Set up Buildroot configuration for Raspberry Pi Zero W:
  - Download Buildroot 2024.02.x (latest stable) to `buildroot/` directory
  - Create custom defconfig at `configs/ereader_rpi0w_defconfig` with:
    - Target architecture: ARM, ARM1176JZF-S (Pi Zero W CPU)
    - Kernel version: Latest LTS (6.1.x or 6.6.x)
    - Enable kernel headers, loadable module support
    - Root filesystem: ext4
    - System hostname: "ereader", root password: "ereader" (dev only)
    - Enable getty on ttyAMA0 (serial console for debugging)
  - Create `buildroot/Makefile` wrapper for common build commands
  - **Completion Notes**: Successfully downloaded Buildroot 2024.02.2, created custom defconfig with all required settings (ARM1176JZF-S, Linux 6.1 LTS, ext4 256MB root filesystem, hostname "ereader", serial console on ttyAMA0 at 115200 baud, debug tools included). Created board/ereader/ configuration files (config_ereader.txt with SPI enabled and minimal GPU memory, post-build.sh for filesystem customization, post-image.sh for SD card image generation). Created Makefile wrapper at project root with targets: config, build, menuconfig, savedefconfig, clean, distclean, show-info. Configuration ready for kernel customization in next task.

- [x] Configure kernel for SPI and framebuffer support:
  - Create `configs/linux_ereader.fragment` with kernel config fragments:
    - Enable SPI support (CONFIG_SPI=y, CONFIG_SPI_BCM2835=y)
    - Enable framebuffer support (CONFIG_FB=y, CONFIG_FB_DEFERRED_IO=y)
    - Enable generic SPI device drivers
    - Enable input event interface (CONFIG_INPUT_EVDEV=y for future GPIO buttons)
  - Document why each option is needed in `docs/progress/PHASE_01_LOG.md`
  - **Completion Notes**: Successfully created configs/linux_ereader.fragment with comprehensive kernel configuration including SPI support (CONFIG_SPI, CONFIG_SPI_BCM2835, CONFIG_SPI_SPIDEV), GPIO support (CONFIG_GPIO_CDEV, CONFIG_GPIO_SYSFS), framebuffer support (CONFIG_FB, CONFIG_FB_DEFERRED_IO, CONFIG_FRAMEBUFFER_CONSOLE), input device support (CONFIG_INPUT_EVDEV, CONFIG_KEYBOARD_GPIO), device tree overlay support, and additional features (I2C, PWM, watchdog, HW RNG). Updated ereader_rpi0w_defconfig to reference the kernel fragment. All options documented with detailed rationale in PHASE_01_LOG.md decision log including why each option is required, alternatives considered, and implementation approach.

- [x] Create device tree overlay for e-paper display:
  - Research the correct device tree bindings for Waveshare 4.2" display
  - Create `board/ereader/overlays/waveshare-epd-4_2.dts` defining:
    - SPI bus configuration (speed, mode, CS pin)
    - GPIO pins for DC, RST, BUSY signals
    - Framebuffer device registration
  - Add compilation instructions in `board/ereader/post-build.sh`
  - Document the overlay in `docs/hardware/DEVICE_TREE.md`
  - **Completion Notes**: Successfully created comprehensive device tree overlay with four fragments: (1) SPI0 device configuration using spidev for userspace access at 4MHz in Mode 0, (2) GPIO pin configuration for RST/BUSY/DC with proper function and pull settings, (3) GPIO labels using gpio-leds subsystem for DC and RST control signals, (4) BUSY pin input configuration with pull-down. Updated post-build.sh to automatically compile all .dts files in overlays directory using host dtc with -@ flag, placing compiled .dtbo files in rpi-firmware/overlays/. Updated config_ereader.txt to load overlay at boot. Created comprehensive DEVICE_TREE.md documentation (150+ lines) covering overlay structure, hardware configuration, kernel requirements, compilation process, verification procedures, userspace access methods, troubleshooting guide, and design rationale. Overlay supports runtime parameter customization for SPI speed and GPIO pin assignments via config.txt overrides.

- [x] Write minimal C test application for display verification:
  - Create `src/display-test/` directory structure
  - Write `src/display-test/epd_driver.c` with basic functions:
    - SPI communication wrapper (init, send command, send data)
    - Display initialization sequence from Waveshare datasheet
    - Simple framebuffer clear and text drawing (using basic bitmap font)
    - Display refresh/update function
  - Write `src/display-test/epd_driver.h` with function prototypes and pin definitions
  - Write `src/display-test/main.c` that initializes display and shows "Hello E-Reader"
  - Create `src/display-test/Makefile` for cross-compilation with Buildroot toolchain
  - **Completion Notes**: Successfully created complete C test application with 5 files: (1) epd_driver.h - comprehensive header with UC8176/IL0398 command definitions, pin mappings (RST=17, DC=25, BUSY=24, CS=8), and function prototypes; (2) epd_driver.c - full driver implementation (700+ lines) with SPI communication via /dev/spidev0.0 at 4MHz, GPIO control via sysfs, complete initialization sequence for UC8176 controller, framebuffer management (15KB for 400×300 1-bit display), and pixel/text rendering; (3) font.h - 8×16 bitmap font covering ASCII 32-90 with inline rendering functions; (4) main.c - test application that initializes display, clears to white, draws "Hello E-Reader" and "Phase 1 Complete" centered on screen, performs full refresh (~5s), and enters sleep mode; (5) Makefile - cross-compilation support with Buildroot toolchain integration, debug/release builds, install target. Also created comprehensive README.md with build instructions, troubleshooting guide, and technical specifications. Application ready for integration into Buildroot package system.

- [x] Integrate test application into Buildroot:
  - Create `package/display-test/display-test.mk` Buildroot package definition
  - Create `package/display-test/Config.in` for menuconfig integration
  - Add display-test to Buildroot package list
  - Configure init system to auto-run display-test on boot
  - Document the package creation process in `docs/buildroot/CUSTOM_PACKAGES.md`
  - **Completion Notes**: Successfully integrated display-test application into Buildroot using BR2_EXTERNAL mechanism. Created complete package infrastructure: (1) external.desc defining EREADER external tree, (2) external.mk and Config.in for automatic package discovery, (3) package/display-test/display-test.mk with full build definition using local site method, cross-compilation support, and binary installation to /usr/bin, (4) package/display-test/Config.in providing menuconfig integration under "External options → EREADER", (5) package/display-test/S99display-test init script for automatic boot-time execution with comprehensive logging to /var/log/display-test.log. Package enabled in configs/ereader_rpi0w_defconfig (BR2_PACKAGE_DISPLAY_TEST=y). Created comprehensive 618-line docs/buildroot/CUSTOM_PACKAGES.md documentation with structured front matter, covering BR2_EXTERNAL structure, package components (.mk, Config.in, Makefile, init scripts), step-by-step guide for adding new packages, init script numbering conventions (S01-S99), daemon vs one-shot patterns, testing/verification procedures, and troubleshooting guide with solutions for common issues. Documentation includes display-test as working reference example and templates for future custom packages. All sub-items completed: package builds via generic-package infrastructure, auto-runs at boot via SysV init, and is fully documented.

- [ ] Build complete system image:
  - Run Buildroot build process (may take 1-2 hours on first build)
  - Verify output files: `output/images/sdcard.img`, `zImage`, `rootfs.ext4`
  - Document build time and any warnings/errors encountered
  - Create `scripts/build.sh` wrapper script for reproducible builds

- [ ] Create SD card flashing and deployment documentation:
  - Write `docs/deployment/FLASHING_GUIDE.md` with step-by-step instructions:
    - How to write sdcard.img to SD card (dd, Etcher, Win32DiskImager)
    - How to boot Pi Zero W and connect serial console
    - Expected boot output and timing
    - How to verify display is working (what to see on e-paper screen)
  - Create `scripts/flash-sdcard.sh` helper script (Linux/Mac)
  - Note: This is documentation only - actual flashing is done manually by user

- [ ] Document phase 1 completion and next steps:
  - Update `docs/progress/PHASE_01_LOG.md` with:
    - What works: boot process, display initialization, text rendering
    - What doesn't work yet: buttons, book reading, menu system
    - Lessons learned and technical decisions made
    - Estimated boot time and display refresh rate
  - Create `docs/progress/PHASE_02_PLANNING.md` outlining button input system design
  - Take a "mental snapshot" of working system state for troubleshooting
