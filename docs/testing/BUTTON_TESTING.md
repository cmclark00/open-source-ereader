---
type: reference
title: Button Testing Guide
created: 2026-01-13
tags:
  - testing
  - buttons
  - gpio
  - verification
related:
  - "[[BUTTON_LAYOUT]]"
  - "[[WIRING_GUIDE]]"
  - "[[PHASE_02_LOG]]"
---

# Button Testing Guide

This document provides comprehensive instructions for testing the GPIO button input system on the E-Reader. Follow these procedures to verify that buttons are wired correctly and functioning properly before proceeding to Phase 3.

## Prerequisites

Before testing buttons, ensure:

1. **Hardware is assembled:**
   - Raspberry Pi Zero W with GPIO header soldered
   - 5 tactile buttons wired according to [[WIRING_GUIDE]]
   - Buttons connected to GPIO 5, 6, 13, 19, 26
   - Common ground connection for all buttons
   - E-paper display connected (not required for button testing, but should be present)

2. **Software is flashed:**
   - SD card flashed with Phase 2 image (includes button-test application)
   - ereader-buttons device tree overlay enabled
   - Serial console access configured (optional but recommended)

3. **System is booted:**
   - Raspberry Pi powered on and booted
   - Serial console connected (or SSH enabled)
   - Root login successful

## Testing Procedure

### Level 1: GPIO Sysfs Testing (Low-Level Verification)

This test verifies that buttons are physically wired correctly using the Linux GPIO sysfs interface.

#### Test Each Button Individually

```bash
# Test UP button (GPIO 5)
echo 5 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio5/direction

# Read value - should be 1 (not pressed)
cat /sys/class/gpio/gpio5/value

# Press and hold the UP button, then read again
cat /sys/class/gpio/gpio5/value
# Should now show 0 (pressed)

# Release button and read again
cat /sys/class/gpio/gpio5/value
# Should return to 1 (released)

# Clean up
echo 5 > /sys/class/gpio/unexport
```

#### Repeat for All Buttons

Test each button GPIO using the same procedure:

- **UP:** GPIO 5
- **DOWN:** GPIO 6
- **SELECT:** GPIO 13
- **BACK:** GPIO 19
- **MENU:** GPIO 26

#### Expected Results

| Button | GPIO | Not Pressed | Pressed |
|--------|------|-------------|---------|
| UP | 5 | 1 | 0 |
| DOWN | 6 | 1 | 0 |
| SELECT | 13 | 1 | 0 |
| BACK | 19 | 1 | 0 |
| MENU | 26 | 1 | 0 |

**If any button fails this test, check wiring before proceeding.**

### Level 2: Input Event Device Verification

Verify that the gpio-keys device tree overlay has created an input event device.

#### Check for Input Devices

```bash
# List all input devices
ls -la /dev/input/

# Should see:
# event0 (or event1, event2, etc.) - gpio-keys device
```

#### Identify gpio-keys Device

```bash
# Check device name
for dev in /dev/input/event*; do
  echo "Device: $dev"
  cat /sys/class/input/$(basename $dev)/device/name
  echo ""
done
```

Look for output containing:
- `gpio-keys`
- `gpio_keys`
- `button`

#### Check Device Capabilities

```bash
# If evtest is available (optional):
evtest /dev/input/event0

# Should show:
# Input device name: "gpio-keys"
# Supported events:
#   Event type 0 (EV_SYN)
#   Event type 1 (EV_KEY)
#     Event code 1 (KEY_ESC)
#     Event code 28 (KEY_ENTER)
#     Event code 103 (KEY_UP)
#     Event code 108 (KEY_DOWN)
#     Event code 139 (KEY_MENU)
```

If `evtest` is not available, proceed to the button-test application.

### Level 3: button-test Application

The primary testing method using the custom button-test application.

#### Run button-test Application

```bash
# Run the button test application
button-test
```

#### Expected Output

```
===========================================
  E-Reader Button Test Application
===========================================

This application tests the GPIO button input system.

Button Layout:
  UP     - GPIO 5  (Pin 29) - Navigate up
  DOWN   - GPIO 6  (Pin 31) - Navigate down
  SELECT - GPIO 13 (Pin 33) - Confirm/Enter
  BACK   - GPIO 19 (Pin 35) - Go back/Cancel
  MENU   - GPIO 26 (Pin 37) - Open menu

Press any button to see events.
Press CTRL+C to exit.

Event Legend:
  Green  - Button pressed
  Red    - Button released
  Yellow - Button held (auto-repeat)

-------------------------------------------

Initializing button input system...
Found gpio-keys device: /dev/input/event0
Button input system initialized successfully.
Waiting for button events...

[12:34:56.123] UP       | PRESS      | value=1
[12:34:56.234] UP       | RELEASE    | value=0
[12:34:57.456] SELECT   | PRESS      | value=1
[12:34:57.567] SELECT   | RELEASE    | value=0
```

#### Interactive Testing Checklist

Press each button and verify console output:

- [ ] **UP button:** Green "UP | PRESS", then red "UP | RELEASE"
- [ ] **DOWN button:** Green "DOWN | PRESS", then red "DOWN | RELEASE"
- [ ] **SELECT button:** Green "SELECT | PRESS", then red "SELECT | RELEASE"
- [ ] **BACK button:** Green "BACK | PRESS", then red "BACK | RELEASE"
- [ ] **MENU button:** Green "MENU | PRESS", then red "MENU | RELEASE"

#### Test Button Combinations

Press multiple buttons simultaneously to verify independence:

- [ ] Press UP + DOWN together → Should see both events
- [ ] Press SELECT + BACK together → Should see both events
- [ ] Press all 5 buttons together → Should see all 5 events

#### Test Button Hold (Auto-Repeat)

Press and hold the SELECT button for 2+ seconds:

- [ ] Initial: Green "SELECT | PRESS"
- [ ] After ~500ms: Yellow "SELECT | REPEAT" (may appear multiple times)
- [ ] On release: Red "SELECT | RELEASE"

**Note:** Auto-repeat is a Linux input subsystem feature and may not be enabled by default. If you don't see REPEAT events, this is normal.

#### Exit the Application

Press CTRL+C to cleanly exit:

```
^C
Shutting down...
Button test application terminated.
```

## Verification Criteria

### Success Criteria

All of the following must be true:

✅ All 5 buttons respond when pressed (Level 1 test)
✅ GPIO values toggle correctly (1 → 0 → 1)
✅ Input event device exists (/dev/input/eventX)
✅ button-test application initializes without errors
✅ All button presses/releases show in button-test output
✅ Button events have correct labels (UP, DOWN, SELECT, BACK, MENU)
✅ No duplicate/extra events from single button press
✅ Multiple buttons can be pressed simultaneously
✅ Console output is color-coded (green, red, yellow)

### Pass/Fail Decision

**PASS:** All success criteria met → Proceed to Phase 3
**FAIL:** Any criteria not met → Troubleshoot before continuing

## Troubleshooting

### Button Not Responding (Level 1 Test)

**Symptom:** GPIO value always shows 1, even when button is pressed

**Possible Causes:**
1. Button not connected to correct GPIO pin
2. Button wired to wrong physical pin
3. Loose jumper wire connection
4. Defective button (no contact when pressed)
5. GND connection missing

**Solutions:**
- Verify wiring against [[BUTTON_LAYOUT]] pin assignment table
- Check physical pin number (not GPIO number)
- Reseat jumper wires
- Test button continuity with multimeter
- Verify common ground connection

### Button Always Shows Pressed (Level 1 Test)

**Symptom:** GPIO value always shows 0

**Possible Causes:**
1. GPIO pin shorted to ground
2. Button stuck in pressed state
3. Solder bridge on GPIO header

**Solutions:**
- Disconnect button and re-test GPIO
- Inspect button for mechanical damage
- Check for solder bridges with magnifying glass
- Try different GPIO pin temporarily

### Wrong Button Detected

**Symptom:** Pressing UP button triggers DOWN event (or similar)

**Possible Causes:**
1. Buttons wired to incorrect GPIO pins
2. Device tree overlay GPIO assignments incorrect

**Solutions:**
- Double-check wiring against [[BUTTON_LAYOUT]]
- Verify device tree overlay compilation succeeded
- Check boot log: `dmesg | grep gpio-keys`
- Rebuild overlay if needed

### No Input Event Device

**Symptom:** `/dev/input/event*` doesn't exist

**Possible Causes:**
1. Device tree overlay not loaded
2. Kernel CONFIG_INPUT_EVDEV not enabled
3. gpio-keys driver not available

**Solutions:**
- Check loaded overlays: `dtoverlay -l`
- Verify overlay in config.txt: `cat /boot/config.txt | grep ereader-buttons`
- Check kernel config: `zcat /proc/config.gz | grep INPUT_EVDEV`
- Rebuild kernel with CONFIG_INPUT_EVDEV=y if needed

### button-test Application Fails to Initialize

**Symptom:** "Failed to initialize button input system"

**Error Message Examples:**
```
ERROR: Failed to initialize button input system
Make sure:
  1. The ereader-buttons device tree overlay is loaded
  2. Buttons are wired correctly (see WIRING_GUIDE.md)
  3. You have permission to access /dev/input/event* devices
```

**Solutions:**

1. **Check device tree overlay:**
   ```bash
   ls /proc/device-tree/gpio-keys/
   # Should show button definitions
   ```

2. **Check permissions:**
   ```bash
   ls -la /dev/input/event*
   # Should be accessible by root
   ```

3. **Check kernel messages:**
   ```bash
   dmesg | grep gpio-keys
   # Look for initialization messages or errors
   ```

4. **Manually test input device:**
   ```bash
   hexdump -C /dev/input/event0
   # Press a button - should see hex output
   # CTRL+C to exit
   ```

### Multiple Events from Single Button Press

**Symptom:** Pressing button once shows 3-5 PRESS/RELEASE events

**Cause:** Button bounce (mechanical contact bouncing)

**Solutions:**
- Increase device tree debounce interval:
  ```dts
  debounce-interval = <50>;  /* Increase from 20ms to 50ms */
  ```
- Recompile overlay and reflash
- Add hardware debouncing capacitor (0.1µF between GPIO and GND)
- Check button quality - cheap switches bounce more

### No REPEAT Events During Long Press

**Symptom:** Holding button doesn't generate REPEAT events

**Explanation:** This is normal. Auto-repeat is not enabled by default in the gpio-keys driver configuration. The device tree would need `linux,can-repeat` property to enable it.

**Not a Problem:** The e-reader application will implement its own repeat logic if needed.

### Color Codes Not Showing

**Symptom:** Output shows escape codes like `^[[1;32m` instead of colors

**Cause:** Terminal doesn't support ANSI color codes

**Solutions:**
- Use a different terminal emulator
- Color codes only work on serial console or SSH, not on HDMI console
- Colors are cosmetic only - functionality unaffected

## Post-Testing Next Steps

### If All Tests Pass

✅ Document successful button verification in [[PHASE_02_LOG]]
✅ Proceed to Phase 3: Basic E-Reader Application
✅ button_input library can be used in future applications
✅ No further button hardware changes needed

### If Tests Fail

❌ Do not proceed to Phase 3
❌ Fix wiring/hardware issues
❌ Retest from Level 1
❌ Document issues in [[PHASE_02_LOG]]
❌ Seek help if issues persist

## Advanced Testing

### Stress Test: Rapid Button Presses

Test debouncing and reliability:

```bash
# Rapidly press and release SELECT button 20 times
# Observe button-test output
```

**Expected:** Clean PRESS/RELEASE pairs, no missed events
**If Failed:** Increase debounce interval

### Stress Test: All Buttons Simultaneously

Press all 5 buttons at once:

**Expected:** See all 5 PRESS events, then all 5 RELEASE events
**If Failed:** Check for common ground issues

### Performance Test: Event Latency

Measure time between physical press and event detection:

**Expected:** < 50ms latency (debounce interval + processing)
**Acceptable:** Up to 100ms
**If Higher:** Check CPU load, reduce poll interval in device tree

## References

- [[BUTTON_LAYOUT]] - Button hardware specifications
- [[WIRING_GUIDE]] - Complete wiring instructions
- [[PHASE_02_LOG]] - Phase 2 progress and issues
- [Linux Input Subsystem Documentation](https://www.kernel.org/doc/html/latest/input/input.html)

## Revision History

- 2026-01-13: Initial button testing guide created
