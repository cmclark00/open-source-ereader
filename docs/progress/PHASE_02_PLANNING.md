---
type: planning
title: Phase 02 Planning - Button Input and Menu System
created: 2026-01-13
tags:
  - planning
  - phase-02
  - buttons
  - input
  - menu-system
  - gpio
related:
  - "[[PHASE_01_LOG]]"
  - "[[Phase-01-Foundation-and-Working-Display]]"
  - "[[WIRING_GUIDE]]"
---

# Phase 02 Planning: Button Input and Menu System

This document outlines the design and implementation plan for Phase 2 of the e-reader project: adding physical button input and a basic menu system for navigation.

## Phase Goal

Create a functional button input system using GPIO pins and implement a simple text-based menu interface on the e-paper display, enabling user navigation and interaction with the e-reader system.

## Prerequisites

Phase 02 assumes successful completion of Phase 01:
- ✓ Buildroot system builds and boots
- ✓ E-paper display working (shows "Hello E-Reader")
- ✓ SPI and GPIO subsystems functional
- ✓ Serial console access for debugging

## Overview

Phase 02 adds interactive capability to the e-reader by:

1. **Hardware:** Adding 5 physical push buttons connected to GPIO pins
2. **Software:** Implementing button debouncing and event handling
3. **UI:** Creating a simple menu system rendered on the e-paper display
4. **Optimization:** Using partial refresh for faster menu navigation

## Button Design

### Hardware Configuration

**Recommended Button Layout:**

```
┌─────────────────────────┐
│  E-Paper Display        │
│  (400×300)              │
│                         │
│                         │
└─────────────────────────┘

        [UP]
         ↑
    [← BACK] [SELECT →]
         ↓
       [DOWN]

       [MENU]
```

**Button Functions:**
- **UP:** Navigate up in menus / Previous page in book
- **DOWN:** Navigate down in menus / Next page in book
- **SELECT:** Confirm selection / Open item
- **BACK:** Go back / Cancel / Exit menu
- **MENU:** Open main menu from anywhere

### GPIO Pin Selection

**Recommended GPIO Assignments:**

| Button | GPIO Pin | Physical Pin | Rationale |
|--------|----------|--------------|-----------|
| UP     | GPIO 5   | Pin 29       | Available, no conflicts |
| DOWN   | GPIO 6   | Pin 31       | Available, no conflicts |
| SELECT | GPIO 13  | Pin 33       | Available, no conflicts |
| BACK   | GPIO 19  | Pin 35       | Available, no conflicts |
| MENU   | GPIO 26  | Pin 37       | Available, no conflicts |

**Pin Selection Criteria:**
- Avoid pins used by display (GPIO 8, 10, 11, 17, 24, 25)
- Avoid special-purpose pins (I2C, UART, etc.)
- Choose pins without pull-up/pull-down on boot (safe boot state)
- Select physically clustered pins for easier wiring

**Alternative Pins (if needed):**
- GPIO 12 (Pin 32)
- GPIO 16 (Pin 36)
- GPIO 20 (Pin 38)
- GPIO 21 (Pin 40)

### Circuit Design

**Simple Pull-up Configuration:**

```
3.3V ──┬── 10kΩ ──┬── GPIO Pin
       │          │
      ─┴─        ─┴─
      Button     Pi Zero W
      ─┬─
       │
      GND
```

Each button connects between GPIO and GND, with internal or external 10kΩ pull-up to 3.3V.

**Configuration:**
- GPIO configured as input with internal pull-up
- Button press pulls GPIO to GND (logic LOW = pressed)
- Button release returns GPIO to 3.3V (logic HIGH = released)
- No external resistors needed if using internal pull-ups

**Debouncing Strategy:**
- Software debouncing (no additional hardware required)
- Typical debounce time: 50ms
- Implementation in driver layer

## Software Architecture

### Input Subsystem Integration

**Approach:** Use Linux input subsystem for button events

**Architecture:**

```
┌─────────────────────────────────────────┐
│  User Application (Menu System)        │
│  - Reads from /dev/input/event0        │
│  - Processes KEY_UP, KEY_DOWN, etc.    │
└─────────────────┬───────────────────────┘
                  │ Input Events
┌─────────────────▼───────────────────────┐
│  Linux Input Subsystem                  │
│  - /dev/input/event0                    │
│  - Event types: EV_KEY                  │
│  - Key codes: KEY_UP, KEY_DOWN, etc.    │
└─────────────────┬───────────────────────┘
                  │ GPIO IRQ
┌─────────────────▼───────────────────────┐
│  GPIO Keys Driver                       │
│  - Device tree: gpio-keys               │
│  - Handles interrupts, debouncing       │
│  - Generates input events               │
└─────────────────┬───────────────────────┘
                  │ GPIO Level
┌─────────────────▼───────────────────────┐
│  BCM2835 GPIO Hardware                  │
│  - GPIO 5, 6, 13, 19, 26                │
│  - Pull-ups enabled                     │
└─────────────────────────────────────────┘
```

**Benefits:**
- Standard Linux input API
- Automatic debouncing in kernel driver
- Event-driven (no polling needed)
- Works with existing libraries (libinput, evdev)

### Device Tree Configuration

Create `board/ereader/overlays/gpio-buttons.dts`:

```dts
/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2835";

    fragment@0 {
        target-path = "/";
        __overlay__ {
            gpio_keys: gpio_keys {
                compatible = "gpio-keys";
                #address-cells = <1>;
                #size-cells = <0>;
                status = "okay";

                button_up: button@0 {
                    label = "UP";
                    linux,code = <103>; /* KEY_UP */
                    gpios = <&gpio 5 1>; /* GPIO5, active low */
                    debounce-interval = <50>;
                };

                button_down: button@1 {
                    label = "DOWN";
                    linux,code = <108>; /* KEY_DOWN */
                    gpios = <&gpio 6 1>; /* GPIO6, active low */
                    debounce-interval = <50>;
                };

                button_select: button@2 {
                    label = "SELECT";
                    linux,code = <28>; /* KEY_ENTER */
                    gpios = <&gpio 13 1>; /* GPIO13, active low */
                    debounce-interval = <50>;
                };

                button_back: button@3 {
                    label = "BACK";
                    linux,code = <1>; /* KEY_ESC */
                    gpios = <&gpio 19 1>; /* GPIO19, active low */
                    debounce-interval = <50>;
                };

                button_menu: button@4 {
                    label = "MENU";
                    linux,code = <139>; /* KEY_MENU */
                    gpios = <&gpio 26 1>; /* GPIO26, active low */
                    debounce-interval = <50>;
                };
            };
        };
    };
};
```

**Device Tree Details:**
- `compatible = "gpio-keys"`: Uses standard gpio-keys driver
- `gpios = <&gpio N 1>`: GPIO number, active low (1 = inverted, 0 when pressed)
- `linux,code`: Standard Linux key codes from `input-event-codes.h`
- `debounce-interval`: Software debounce time in milliseconds

### Kernel Configuration

Already enabled in Phase 01 (configs/linux_ereader.fragment):
- `CONFIG_INPUT_EVDEV=y` - Input event interface
- `CONFIG_KEYBOARD_GPIO=y` - GPIO keyboard driver

**Verification after boot:**
```bash
# Check input device exists
ls /dev/input/event*

# View input device info
cat /sys/class/input/event0/device/name
# Should show: gpio_keys

# Monitor button events (real-time)
evtest /dev/input/event0
```

## Menu System Design

### Menu Structure

**Main Menu:**

```
┌─────────────────────────┐
│   E-Reader v1.0         │
│                         │
│   > Library             │
│     Settings            │
│     About               │
│                         │
│   UP/DOWN: Navigate     │
│   SELECT: Open          │
└─────────────────────────┘
```

**Library Menu (Future):**
```
┌─────────────────────────┐
│   Library               │
│                         │
│   > Book 1.epub         │
│     Book 2.epub         │
│     Book 3.epub         │
│                         │
│   BACK: Main Menu       │
└─────────────────────────┘
```

**Settings Menu:**
```
┌─────────────────────────┐
│   Settings              │
│                         │
│   > Display Test        │
│     System Info         │
│     Reboot              │
│     Shutdown            │
│                         │
│   BACK: Main Menu       │
└─────────────────────────┘
```

**System Info:**
```
┌─────────────────────────┐
│   System Information    │
│                         │
│   Kernel: 6.1.xx        │
│   Uptime: 0d 0h 5m      │
│   Memory: 45MB / 512MB  │
│   Storage: 120MB free   │
│                         │
│   BACK: Settings        │
└─────────────────────────┘
```

### Menu Rendering

**Text-Based UI Framework:**

Create `src/menu-system/ui_framework.h` and `ui_framework.c`:

**Features:**
- Screen layout manager (title, menu items, footer)
- Text rendering using 8×16 font from Phase 01
- Cursor/selection indicator (">")
- Scrolling for long lists
- Partial refresh optimization

**Layout Structure:**

```
┌─────────────────────────┐  Line 0-1: Title (centered)
│   Title Text            │
│                         │  Line 2: Separator/blank
├─────────────────────────┤
│   > Item 1              │  Lines 3-15: Menu items (13 items max visible)
│     Item 2              │  ">" indicates selection
│     Item 3              │
│     ...                 │
│                         │
├─────────────────────────┤  Line 16: Separator/blank
│   Help Text             │  Lines 17-18: Footer/help
└─────────────────────────┘
```

**Dimensions:**
- Display: 400×300 pixels
- Font: 8×16 pixels per character
- Characters per line: 400/8 = 50 characters
- Lines available: 300/16 = 18.75 lines (~18 lines)
- Title: 2 lines
- Footer: 2 lines
- Menu area: ~14 lines (11-13 visible items)

### Partial Refresh Optimization

**Goal:** Reduce refresh time from 5 seconds to <1 second for menu navigation

**Strategy:**

1. **Full Refresh Triggers:**
   - Menu screen change (new menu)
   - After 10 partial refreshes (prevent ghosting)
   - User-initiated (settings option)

2. **Partial Refresh Use:**
   - Cursor movement (item selection)
   - Small text updates (time, status)
   - Single item changes

3. **Implementation:**
   - Add partial refresh function to epd_driver.c
   - Track refresh counter (force full after N partial)
   - Define update regions (e.g., menu items only, not title/footer)

**UC8176 Partial Refresh Sequence:**
- Use "partial update" mode in controller
- Update only changed regions
- Faster refresh (~300-500ms vs 5000ms)
- Trade-off: slight ghosting after multiple partials

### Input Event Handling

**Event Loop Architecture:**

```c
// Pseudo-code for menu event loop

int input_fd = open("/dev/input/event0", O_RDONLY);
struct input_event ev;

while (1) {
    // Read input event (blocking)
    read(input_fd, &ev, sizeof(ev));

    // Filter for key press events (not release)
    if (ev.type == EV_KEY && ev.value == 1) {
        switch (ev.code) {
            case KEY_UP:
                menu_move_cursor_up();
                ui_render_menu(PARTIAL_REFRESH);
                break;
            case KEY_DOWN:
                menu_move_cursor_down();
                ui_render_menu(PARTIAL_REFRESH);
                break;
            case KEY_ENTER:
                menu_select_item();
                ui_render_menu(FULL_REFRESH);
                break;
            case KEY_ESC:
                menu_go_back();
                ui_render_menu(FULL_REFRESH);
                break;
            case KEY_MENU:
                menu_show_main();
                ui_render_menu(FULL_REFRESH);
                break;
        }
    }
}
```

**Features:**
- Blocking read (no CPU polling)
- Key press detection (value == 1, ignore release value == 0)
- Partial refresh for cursor movement
- Full refresh for screen changes
- Responsive UI (<100ms input latency)

## Implementation Plan

### Task Breakdown

**Phase 2 Tasks:**

1. **Hardware Setup:**
   - Document button wiring in WIRING_GUIDE.md
   - Add button components to BOM.md
   - Create button assembly instructions

2. **Device Tree Configuration:**
   - Create gpio-buttons.dts overlay
   - Update post-build.sh to compile new overlay
   - Update config.txt to load gpio-buttons overlay
   - Test overlay loads correctly

3. **Button Test Application:**
   - Create src/button-test/ directory
   - Write simple button test (prints button presses)
   - Integrate into Buildroot as package
   - Verify all 5 buttons work correctly

4. **Menu System Framework:**
   - Create src/menu-system/ directory
   - Implement UI framework (layout, rendering)
   - Port 8×16 font from display-test
   - Add partial refresh support to epd_driver

5. **Menu Implementation:**
   - Implement menu data structures (linked list or array)
   - Create main menu, settings menu, system info
   - Add navigation logic (up/down/select/back)
   - Implement menu state machine

6. **Integration:**
   - Replace display-test with menu-system as init application
   - Test on hardware
   - Optimize refresh times
   - Debug any input/display issues

7. **Documentation:**
   - Update PHASE_02_LOG.md with progress
   - Document button API and menu framework
   - Create user guide for menu navigation

### Timeline Estimates

**Development Time (not calendar time, effort required):**

| Task | Estimated Effort | Dependencies |
|------|------------------|--------------|
| Button hardware documentation | 1 hour | Phase 01 complete |
| Device tree overlay | 2 hours | Kernel config ready |
| Button test application | 3 hours | Overlay working |
| UI framework | 6 hours | Display driver working |
| Partial refresh implementation | 4 hours | UI framework |
| Menu data structures | 2 hours | UI framework |
| Menu navigation logic | 4 hours | Menu structures |
| Integration and testing | 6 hours | All components ready |
| Documentation | 2 hours | Testing complete |
| **Total** | **~30 hours** | |

**Critical Path:**
Device tree → Button test → UI framework → Partial refresh → Menu implementation → Integration

## Technical Challenges

### Challenge 1: Button Debouncing

**Problem:** Mechanical buttons generate multiple events on single press (contact bounce)

**Solution:**
- Use kernel's built-in debouncing (debounce-interval in device tree)
- Typical value: 50ms
- If needed, add software debouncing in application (ignore events <50ms apart)

**Testing:**
- Monitor raw events with `evtest`
- Verify single event per press
- Adjust debounce-interval if needed

### Challenge 2: Partial Refresh Ghosting

**Problem:** Partial refreshes cause ghosting (previous image faintly visible)

**Solution:**
- Force full refresh after N partial refreshes (e.g., N=10)
- Offer manual full refresh (button combo or menu option)
- Use partial refresh only for cursor movement, not content changes

**Testing:**
- Navigate menu extensively
- Check for ghosting buildup
- Tune full refresh frequency

### Challenge 3: Input Latency

**Problem:** E-paper refresh time creates perceived input lag

**Solution:**
- Use partial refresh for immediate feedback (<500ms)
- Provide audio/haptic feedback (future: piezo buzzer)
- Visual feedback before refresh (change cursor immediately, then refresh)

**Testing:**
- Measure time from button press to screen update
- Target: <1 second for partial refresh

### Challenge 4: Power Consumption

**Problem:** Continuous input polling wastes power

**Solution:**
- Use interrupt-driven input (gpio-keys driver provides this)
- No polling loop needed
- CPU sleeps until button press
- Display controller in sleep mode when idle

**Testing:**
- Measure idle current (should be similar to Phase 01)
- Verify CPU not busy-waiting
- Check `top` - no high CPU processes

## Success Criteria

Phase 02 is successful when:

✓ **All 5 buttons functional:**
- Each button generates correct input event
- Events appear in /dev/input/event0
- No false triggers or missed presses
- Debouncing works correctly

✓ **Menu system operational:**
- Main menu renders correctly
- Cursor navigation works (UP/DOWN buttons)
- Item selection works (SELECT button)
- Back navigation works (BACK button)
- Menu button returns to main menu from anywhere

✓ **Display optimization:**
- Partial refresh implemented and working
- Cursor movement uses partial refresh (<1 second)
- Menu changes use full refresh
- No excessive ghosting

✓ **System integration:**
- Menu system launches on boot
- Can navigate to Settings → Display Test
- Display test still works
- System info shows correct data
- Shutdown/reboot commands work

✓ **Documentation complete:**
- Button wiring guide updated
- Menu system architecture documented
- User guide for menu navigation
- Phase 02 progress log updated

## Testing Plan

### Unit Testing

1. **Button Hardware:**
   - Test each button individually with multimeter
   - Verify GPIO voltage: HIGH (~3.3V) when released, LOW (~0V) when pressed
   - Check for shorts or open circuits

2. **Button Events:**
   - Use `evtest /dev/input/event0`
   - Press each button, verify correct key code
   - Test rapid presses (debouncing)
   - Test simultaneous presses (if any combinations needed)

3. **UI Rendering:**
   - Test title rendering (centered text)
   - Test menu items (left-aligned, cursor)
   - Test footer (help text)
   - Test scrolling (>13 items in list)

4. **Partial Refresh:**
   - Measure refresh time (should be <1s)
   - Test ghosting after 5, 10, 20 partial refreshes
   - Verify full refresh clears ghosting

### Integration Testing

1. **Navigation Flow:**
   - Start at main menu
   - Navigate to Settings
   - Navigate to System Info
   - Go back to Settings
   - Go back to Main Menu
   - Press Menu button from System Info (should go to Main Menu)

2. **Edge Cases:**
   - UP at top of menu (should wrap or do nothing)
   - DOWN at bottom of menu (should wrap or do nothing)
   - SELECT on item with no action (should ignore or show message)
   - BACK at main menu (should do nothing or show exit confirmation)

3. **Stress Testing:**
   - Rapid button presses (10 presses/second)
   - Navigate through all menus multiple times
   - Leave system idle for 1+ hours, verify still responsive
   - Test after 100+ partial refreshes

### Performance Testing

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Button response time | <50ms | evtest timestamp |
| Partial refresh time | <1 second | Stopwatch, code timing |
| Full refresh time | <5 seconds | Stopwatch, code timing |
| CPU usage (idle) | <5% | top command |
| Memory usage | <120MB | free command |
| Boot time | <40 seconds | Serial console, stopwatch |

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| GPIO pins conflict with display | Low | High | Careful pin selection, testing |
| Debouncing insufficient | Medium | Medium | Adjustable debounce-interval in DT |
| Partial refresh too slow | Medium | Medium | Optimize controller settings, fallback to full |
| Ghosting unacceptable | Low | Medium | Force full refresh more frequently |
| Input latency too high | Low | High | Partial refresh, optimize rendering |
| Memory exhausted by menu data | Very Low | High | Keep menus simple, limit item count |
| Buttons unreliable (hardware) | Medium | High | Quality components, proper wiring |

## Future Enhancements (Post-Phase 2)

### Phase 2.5 (Optional Improvements)

1. **Audio Feedback:**
   - Add piezo buzzer on PWM pin
   - Click sound on button press
   - Different tones for different actions

2. **Status Bar:**
   - Top status bar with time, battery (if added), WiFi
   - Always visible across all screens
   - Updates every minute

3. **Power Management:**
   - Blank display after 5 minutes idle
   - Deep sleep mode
   - Wake on button press

4. **Configuration Persistence:**
   - Save user settings to /etc/ereader.conf
   - Remember last menu position
   - Restore state on boot

### Phase 3 Preview

Once Phase 2 is complete and stable, Phase 3 will add:

1. **Book Library:**
   - Scan /books/ directory for EPUB files
   - Display list in Library menu
   - Basic metadata (title, author from EPUB)

2. **EPUB Reader:**
   - Parse EPUB files (use libxml2, libzip)
   - Extract text content
   - Render pages with reflowing text
   - Page navigation (UP/DOWN for pages)

3. **Reading State:**
   - Save current book and page
   - Resume reading on boot
   - Bookmarks functionality

## References

### Hardware

- [GPIO Usage Guide - Raspberry Pi](https://www.raspberrypi.com/documentation/computers/os.html#gpio-and-the-40-pin-header)
- [Button Debouncing Techniques](https://www.ganssle.com/debouncing.htm)
- [Push Button Wiring Best Practices](https://learn.adafruit.com/make-it-switch/pull-up-and-pull-down-resistors)

### Software

- [Linux Input Subsystem Documentation](https://www.kernel.org/doc/html/latest/input/input.html)
- [gpio-keys Device Tree Binding](https://www.kernel.org/doc/Documentation/devicetree/bindings/input/gpio-keys.txt)
- [evdev (Event Device) API](https://www.freedesktop.org/software/libevdev/doc/latest/)
- [Input Event Codes](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h)

### E-Paper Display

- [UC8176 Controller Partial Refresh](https://github.com/waveshare/e-Paper/tree/master/RaspberryPi_JetsonNano/c)
- [E-Paper Refresh Modes Explained](https://www.good-display.com/companyfile/32.html)

## Related Documentation

- `[[PHASE_01_LOG]]` - Phase 1 completion summary
- `[[WIRING_GUIDE]]` - Hardware wiring (to be updated with buttons)
- `[[BOM]]` - Bill of materials (to be updated with buttons)
- `[[DISPLAY_SPECS]]` - E-paper display technical details
- `[[CUSTOM_PACKAGES]]` - How to create Buildroot packages

---

**Document Version**: 1.0
**Created**: 2026-01-13
**Status**: Planning - Ready for implementation
**Estimated Effort**: ~30 hours development time
