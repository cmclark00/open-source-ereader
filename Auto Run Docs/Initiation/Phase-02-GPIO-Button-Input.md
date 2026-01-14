# Phase 02: GPIO Button Input System

This phase implements a robust GPIO button input system for navigating the e-reader. By the end of this phase, you'll have physical buttons wired to the Pi Zero W that can be pressed to navigate menus and control the device, with proper debouncing and event handling.

## Tasks

- [ ] Design button layout and GPIO pin assignments:
  - Create `docs/hardware/BUTTON_LAYOUT.md` documenting:
    - Button functions: UP, DOWN, SELECT, BACK, MENU (5 buttons minimum)
    - GPIO pin assignments for each button (avoid pins used by SPI display)
    - Pull-up vs pull-down resistor strategy
    - Physical layout suggestions (placement around display)
  - Create ASCII art or simple diagram showing button positions
  - Document which Pi Zero W GPIO pins are available (avoid GPIO 7-11 used by SPI)

- [ ] Update wiring guide with button connections:
  - Extend `docs/hardware/WIRING_GUIDE.md` with button wiring section:
    - Schematic showing buttons connected between GPIO and GND with pull-up resistors
    - Step-by-step wiring instructions for each button
    - Testing procedure: use `gpio` command or Python to verify button presses
  - Update `docs/hardware/BOM.md` with tactile button part numbers and resistor values

- [ ] Update kernel configuration for GPIO input:
  - Extend `configs/linux_ereader.fragment` with GPIO options:
    - Enable GPIO character device support (CONFIG_GPIO_CDEV=y)
    - Enable GPIO sysfs interface (CONFIG_GPIO_SYSFS=y) for debugging
    - Enable software debouncing if available
  - Document configuration choices in `docs/progress/PHASE_02_LOG.md`

- [ ] Create device tree overlay for GPIO buttons:
  - Create `board/ereader/overlays/ereader-buttons.dts`:
    - Define gpio-keys device with all 5 buttons
    - Map buttons to Linux input event codes (KEY_UP, KEY_DOWN, KEY_ENTER, KEY_ESC, KEY_MENU)
    - Configure debounce intervals (typically 20-50ms)
    - Set active-low property for pull-up configuration
  - Update `board/ereader/post-build.sh` to compile both overlays
  - Document button event codes in `docs/hardware/BUTTON_LAYOUT.md`

- [ ] Write button test application:
  - Create `src/button-test/` directory
  - Write `src/button-test/button_input.c` with:
    - Function to open /dev/input/event* devices
    - Event loop reading input_event structures
    - Button press/release detection with timestamp
    - Debouncing logic in software (belt-and-suspenders approach)
  - Write `src/button-test/button_input.h` with button enum and callback types
  - Write `src/button-test/main.c` that prints button events to console
  - Create `src/button-test/Makefile` for cross-compilation

- [ ] Integrate button test into Buildroot:
  - Create `package/button-test/button-test.mk` package definition
  - Create `package/button-test/Config.in` for menuconfig
  - Add to Buildroot package list
  - Document package in `docs/buildroot/CUSTOM_PACKAGES.md`

- [ ] Build and create testing documentation:
  - Rebuild Buildroot with button support enabled
  - Create `docs/testing/BUTTON_TESTING.md` with:
    - How to run button-test application
    - Expected output when buttons are pressed
    - Troubleshooting guide for common issues (no events, multiple events, wrong pins)
    - How to verify button functionality before proceeding to Phase 3
  - Update `docs/progress/PHASE_02_LOG.md` with completion notes

- [ ] Design input abstraction layer for e-reader application:
  - Create `docs/architecture/INPUT_SYSTEM.md` documenting:
    - High-level input event handling architecture
    - How button events will translate to e-reader actions (page turn, menu navigation)
    - State machine for different input modes (reading mode vs menu mode)
    - Plan for future input methods (touchscreen, keyboard)
  - This is planning documentation for Phase 3 implementation
