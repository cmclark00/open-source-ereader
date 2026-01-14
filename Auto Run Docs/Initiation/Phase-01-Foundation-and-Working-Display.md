# Phase 01: Foundation and Working Display

This phase establishes the project foundation and delivers a working prototype: a Buildroot-based minimal Linux system that boots on the Raspberry Pi Zero W and successfully displays "Hello E-Reader" on the Waveshare 4.2" e-paper display. By the end of this phase, you'll have verified hardware functionality and a proven software stack to build upon.

## Tasks

- [x] Create project structure and initial documentation:
  - `README.md` with project overview, hardware requirements (Pi Zero W, Waveshare 4.2" e-paper rev 2.2), and quick start guide
  - `docs/hardware/WIRING_GUIDE.md` documenting SPI pin connections between Pi Zero W and Waveshare display
  - `docs/hardware/BOM.md` (Bill of Materials) listing all required components with part numbers
  - `docs/progress/PHASE_01_LOG.md` for tracking progress and decisions
  - `.gitignore` excluding build artifacts, downloads, and temporary files

- [ ] Research and document Waveshare 4.2" e-paper display technical requirements:
  - Create `docs/hardware/DISPLAY_SPECS.md` with front matter (type: research, tags: [hardware, display, waveshare])
  - Document display resolution, color depth, refresh time, and SPI communication protocol
  - Research and note the correct Linux kernel driver (IT8951, EPD driver, or fb_ili9341 equivalent)
  - Include links to Waveshare official documentation and example code repositories
  - Document power requirements and any special initialization sequences

- [ ] Set up Buildroot configuration for Raspberry Pi Zero W:
  - Download Buildroot 2024.02.x (latest stable) to `buildroot/` directory
  - Create custom defconfig at `configs/ereader_rpi0w_defconfig` with:
    - Target architecture: ARM, ARM1176JZF-S (Pi Zero W CPU)
    - Kernel version: Latest LTS (6.1.x or 6.6.x)
    - Enable kernel headers, loadable module support
    - Root filesystem: ext4
    - System hostname: "ereader", root password: "ereader" (dev only)
    - Enable getty on ttyAMA0 (serial console for debugging)
  - Create `buildroot/Makefile` wrapper for common build commands

- [ ] Configure kernel for SPI and framebuffer support:
  - Create `configs/linux_ereader.fragment` with kernel config fragments:
    - Enable SPI support (CONFIG_SPI=y, CONFIG_SPI_BCM2835=y)
    - Enable framebuffer support (CONFIG_FB=y, CONFIG_FB_DEFERRED_IO=y)
    - Enable generic SPI device drivers
    - Enable input event interface (CONFIG_INPUT_EVDEV=y for future GPIO buttons)
  - Document why each option is needed in `docs/progress/PHASE_01_LOG.md`

- [ ] Create device tree overlay for e-paper display:
  - Research the correct device tree bindings for Waveshare 4.2" display
  - Create `board/ereader/overlays/waveshare-epd-4_2.dts` defining:
    - SPI bus configuration (speed, mode, CS pin)
    - GPIO pins for DC, RST, BUSY signals
    - Framebuffer device registration
  - Add compilation instructions in `board/ereader/post-build.sh`
  - Document the overlay in `docs/hardware/DEVICE_TREE.md`

- [ ] Write minimal C test application for display verification:
  - Create `src/display-test/` directory structure
  - Write `src/display-test/epd_driver.c` with basic functions:
    - SPI communication wrapper (init, send command, send data)
    - Display initialization sequence from Waveshare datasheet
    - Simple framebuffer clear and text drawing (using basic bitmap font)
    - Display refresh/update function
  - Write `src/display-test/epd_driver.h` with function prototypes and pin definitions
  - Write `src/display-test/main.c` that initializes display and shows "Hello E-Reader"
  - Create `src/display-test/Makefile` for cross-compilation with Buildroot toolchain

- [ ] Integrate test application into Buildroot:
  - Create `package/display-test/display-test.mk` Buildroot package definition
  - Create `package/display-test/Config.in` for menuconfig integration
  - Add display-test to Buildroot package list
  - Configure init system to auto-run display-test on boot
  - Document the package creation process in `docs/buildroot/CUSTOM_PACKAGES.md`

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
