---
type: report
title: Phase 02 Progress Log - GPIO Button Input System
created: 2026-01-13
tags:
  - progress
  - phase-02
  - buttons
  - gpio
  - input
related:
  - "[[PHASE_01_LOG]]"
  - "[[BUTTON_LAYOUT]]"
  - "[[WIRING_GUIDE]]"
---

# Phase 02 Progress Log: GPIO Button Input System

This log tracks progress on implementing the GPIO button input system for e-reader navigation.

## Phase Overview

**Objective:** Implement a robust GPIO button input system with 5 physical buttons for navigating the e-reader interface.

**Start Date:** 2026-01-13

**Status:** In Progress

## Completed Tasks

### 2026-01-13: Hardware Documentation and Planning

**Task 1: Button Layout and GPIO Pin Assignments**
- ✅ Created `docs/hardware/BUTTON_LAYOUT.md`
- Designed 5-button layout: UP, DOWN, SELECT, BACK, MENU
- Assigned GPIO pins: GPIO 5, 6, 13, 19, 26
- Documented active-low configuration with internal pull-ups
- Created physical layout diagrams and suggestions
- Mapped Linux input event codes (KEY_UP, KEY_DOWN, KEY_ENTER, KEY_ESC, KEY_MENU)
- Documented debouncing strategy (hardware + software)

**Rationale for GPIO Pin Selection:**
- Avoided SPI pins used by display (GPIO 7-11, 17, 24, 25)
- Avoided UART pins for serial console (GPIO 14-15)
- Reserved I2C pins for future sensors (GPIO 2-3)
- Chose physically adjacent pins (Pin 29-37) for easier wiring
- All selected pins are general-purpose with no special conflicts

**Task 2: Wiring Guide Update**
- ✅ Extended `docs/hardware/WIRING_GUIDE.md` with button section
- Added button pin connection table
- Created button wiring schematic (active-low with internal pull-up)
- Documented step-by-step button wiring instructions
- Added combined display + buttons wiring table
- Included testing procedures using GPIO sysfs
- Added comprehensive troubleshooting section
- Documented optional hardware debouncing with capacitors

**Task 3: Bill of Materials Update**
- ✅ Updated `docs/hardware/BOM.md` with button components
- Corrected pull-up resistor requirement (not needed - using internal)
- Added detailed tactile button specifications
- Included specific part numbers: Omron B3F-1000/B3F-4055, E-Switch TL1105, C&K PTS645
- Added optional 0.1µF capacitors for hardware debouncing
- Documented 6x6mm vs 12x12mm button trade-offs

**Task 4: Kernel Configuration Review**
- ✅ Reviewed `configs/linux_ereader.fragment`
- Verified GPIO support already configured:
  - `CONFIG_GPIO_CDEV=y` - Character device interface
  - `CONFIG_GPIO_SYSFS=y` - Sysfs interface for debugging
  - `CONFIG_INPUT_EVDEV=y` - Input event interface
  - `CONFIG_KEYBOARD_GPIO=y` - GPIO-based button support
  - `CONFIG_KEYBOARD_GPIO_POLLED=y` - Polled GPIO keys
  - `CONFIG_OF_OVERLAY=y` - Device tree overlay support
- **Result:** Kernel configuration already complete for GPIO buttons (done in Phase 01)
- No additional kernel config changes required

**Configuration Rationale:**
- `CONFIG_GPIO_CDEV` provides modern /dev/gpiochip0 interface
- `CONFIG_GPIO_SYSFS` retained for easy debugging and testing
- `CONFIG_KEYBOARD_GPIO` enables gpio-keys device tree binding
- `CONFIG_INPUT_EVDEV` creates /dev/input/eventX devices for userspace access
- All necessary options were proactively configured in Phase 01

## In Progress Tasks

### Device Tree Overlay Creation

**Status:** Ready to implement

**Next Steps:**
1. Create `board/ereader/overlays/ereader-buttons.dts`
2. Define gpio-keys device with all 5 buttons
3. Map button GPIOs to Linux input event codes
4. Configure debounce intervals (20-50ms)
5. Set GPIO_ACTIVE_LOW property for each button
6. Update `board/ereader/post-build.sh` to compile button overlay
7. Test overlay loading and button events

**Technical Details:**
- Device tree source will define gpio-keys compatible device
- Each button configured as separate key within gpio-keys node
- GPIOs referenced using phandle to bcm2835 GPIO controller
- Debounce interval set at 20ms (hardware) + software filtering
- Active-low configuration enables internal pull-ups

## Pending Tasks

### Button Test Application

**Requirements:**
- C application to read /dev/input/eventX devices
- Event loop parsing input_event structures
- Button press/release detection with timestamps
- Additional software debouncing (50ms window)
- Console output showing button events
- Modular design with button_input.c/h library

### Buildroot Integration

**Requirements:**
- Create `package/button-test/` directory structure
- Write button-test.mk package definition
- Create Config.in for menuconfig integration
- Update Buildroot package configuration
- Document in `docs/buildroot/CUSTOM_PACKAGES.md`

### Testing Documentation

**Requirements:**
- Create `docs/testing/BUTTON_TESTING.md`
- Document how to run button-test application
- Provide expected output examples
- Troubleshooting guide for common issues
- Verification procedure before Phase 3

### Input Architecture Planning

**Requirements:**
- Create `docs/architecture/INPUT_SYSTEM.md`
- Design high-level event handling architecture
- Plan button-to-action mapping system
- Design state machine for input modes (reading vs menu)
- Consider future input methods (touchscreen, keyboard)

## Technical Decisions

### Button Configuration Choice: Internal Pull-ups

**Decision:** Use Raspberry Pi's internal pull-up resistors instead of external resistors

**Rationale:**
- Reduces component count and cost (no external resistors needed)
- Simplifies wiring (fewer connections)
- Standard practice for Raspberry Pi GPIO buttons
- BCM2835 internal pull-ups (~50kΩ) are well-suited for button debouncing
- Device tree `GPIO_ACTIVE_LOW` flag automatically enables internal pull-up

**Trade-offs:**
- Internal pull-ups are weaker (~50kΩ vs typical 10kΩ external)
- Slightly more susceptible to electrical noise (mitigated by software debouncing)
- Less control over pull-up strength (not adjustable)
- **Conclusion:** Acceptable trade-off for button input application

### Debouncing Strategy: Multi-layer Approach

**Decision:** Implement three-layer debouncing system

**Layers:**
1. **Optional hardware debouncing:** 0.1µF capacitor per button (optional)
2. **Device tree debouncing:** 20ms debounce-interval property
3. **Software debouncing:** 50ms filter in application code

**Rationale:**
- Hardware capacitor is optional but improves reliability with noisy switches
- Device tree debouncing handled by Linux GPIO subsystem (efficient, kernel-level)
- Application-level debouncing provides final safety net
- Belt-and-suspenders approach ensures robust button handling
- Each layer catches different types of bounce/noise

### GPIO Pin Selection Rationale

**Decision:** Use GPIO 5, 6, 13, 19, 26 for buttons

**Considered Alternatives:**
- Lower numbered GPIOs (2-4): Reserved for I2C and system functions
- GPIO 14-15: Used for UART serial console
- GPIO 16, 20, 21: Reserved for future expansion (6th button, LEDs, etc.)

**Final Choice Benefits:**
- Physically grouped together (pins 29-37 on header)
- Easy to wire with minimal jumper wire crossing
- No conflicts with existing hardware (SPI display, UART)
- Leaves expansion pins available

## Challenges and Solutions

### Challenge: Pin Mapping Confusion

**Problem:** GPIO numbers vs physical pin numbers differ on Raspberry Pi

**Solution:**
- Created comprehensive tables in [[BUTTON_LAYOUT]]
- Documented both GPIO and physical pin numbers
- Added visual diagram of GPIO header section
- Included testing procedure to verify correct wiring

### Challenge: Pull-up/Pull-down Configuration

**Problem:** Confusion about when to use pull-up vs pull-down and internal vs external

**Solution:**
- Clearly documented active-low configuration with internal pull-ups
- Explained why this is standard for Raspberry Pi buttons
- Provided electrical schematics showing current flow
- Noted that `GPIO_ACTIVE_LOW` device tree flag handles configuration

## Next Milestone

**Target:** Complete device tree overlay and button test application

**Success Criteria:**
- Device tree overlay compiles without errors
- Buttons appear as /dev/input/eventX devices
- button-test application shows correct button press/release events
- All 5 buttons respond reliably with proper debouncing
- Documentation complete for testing procedures

## Resources and References

- [Linux gpio-keys Device Tree Binding](https://www.kernel.org/doc/Documentation/devicetree/bindings/input/gpio-keys.txt)
- [Raspberry Pi GPIO Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [Linux Input Subsystem](https://www.kernel.org/doc/html/latest/input/input.html)
- [[BUTTON_LAYOUT]] - Hardware button specifications
- [[WIRING_GUIDE]] - Physical wiring instructions

## Notes

- Phase 01 proactively configured kernel for GPIO button support
- Hardware documentation is complete and ready for physical assembly
- Next focus: Device tree overlay and test application development
- Consider ordering button components early (2-4 week lead time for some suppliers)

## Revision History

- 2026-01-13: Phase 02 progress log created, documented hardware planning completion
