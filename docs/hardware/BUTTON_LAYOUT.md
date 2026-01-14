---
type: reference
title: Button Layout and GPIO Pin Assignments
created: 2026-01-13
tags:
  - hardware
  - buttons
  - gpio
  - input
  - navigation
related:
  - "[[WIRING_GUIDE]]"
  - "[[BOM]]"
---

# Button Layout and GPIO Pin Assignments

This document defines the physical button layout, GPIO pin assignments, and electrical configuration for the e-reader's navigation buttons. The design uses 5 tactile push buttons for complete navigation control without requiring a touchscreen.

## Button Functions

The e-reader uses 5 buttons for navigation:

| Button | Function | Primary Use |
|--------|----------|-------------|
| **UP** | Navigate up | Move selection up in menus, scroll up in text |
| **DOWN** | Navigate down | Move selection down in menus, scroll down in text |
| **SELECT** | Confirm/Enter | Activate selected item, open book, confirm action |
| **BACK** | Go back/Cancel | Return to previous screen, cancel operation, close menu |
| **MENU** | Open menu | Access main menu, context menu, settings |

### Button Behavior

- **Short press:** Standard navigation action
- **Long press (future):** Alternative actions (e.g., MENU long-press for quick settings)
- **Simultaneous press (future):** Special functions (e.g., BACK+SELECT for screenshot)

## GPIO Pin Assignments

The following GPIO pins are assigned to each button. These pins were chosen to:
1. Avoid conflicts with SPI pins used by the display (GPIO 7-11)
2. Avoid UART pins needed for serial console (GPIO 14-15)
3. Use physically adjacent pins for easier wiring
4. Leave room for future expansion (I2C, additional sensors)

### Pin Assignment Table

| Button | GPIO Number | Physical Pin | Wiring |
|--------|-------------|--------------|--------|
| **UP** | GPIO 5 | Pin 29 | GPIO 5 to GND via button |
| **DOWN** | GPIO 6 | Pin 31 | GPIO 6 to GND via button |
| **SELECT** | GPIO 13 | Pin 33 | GPIO 13 to GND via button |
| **BACK** | GPIO 19 | Pin 35 | GPIO 19 to GND via button |
| **MENU** | GPIO 26 | Pin 37 | GPIO 26 to GND via button |
| **Common GND** | GND | Pin 34, 39 | Common ground for all buttons |

### GPIO Pin Rationale

**Pins used by display (AVOID):**
- GPIO 8 (CE0), GPIO 10 (MOSI), GPIO 11 (SCLK) - SPI interface
- GPIO 17 (RST), GPIO 24 (BUSY), GPIO 25 (DC) - Display control signals

**Pins used by system (AVOID):**
- GPIO 14, GPIO 15 - UART serial console (debugging)
- GPIO 2, GPIO 3 - I2C (reserved for future RTC/sensors)

**Available pins selected:**
- GPIO 5, 6, 13, 19, 26 - All on the right side of the GPIO header
- Physically grouped together (pins 29-37) for easy wiring
- No special functions that would conflict with button usage

### Linux Input Event Codes

The device tree will map these buttons to standard Linux input event codes:

| Button | Event Code | Numeric Value | Standard Key |
|--------|------------|---------------|--------------|
| UP | KEY_UP | 103 | Arrow Up |
| DOWN | KEY_DOWN | 108 | Arrow Down |
| SELECT | KEY_ENTER | 28 | Enter/Return |
| BACK | KEY_ESC | 1 | Escape |
| MENU | KEY_MENU | 139 | Menu |

These event codes are defined in `linux/input-event-codes.h` and are standard across Linux systems.

## Electrical Configuration

### Pull-up Resistor Strategy

The buttons use **internal pull-up resistors** with **active-low configuration**:

- **Idle state:** GPIO pin pulled HIGH (3.3V) by internal pull-up resistor
- **Pressed state:** GPIO pin pulled LOW (0V) when button connects it to GND
- **Device tree configuration:** `GPIO_ACTIVE_LOW` flag set for each button

**Advantages of this approach:**
- No external pull-up resistors required (saves components and cost)
- Standard configuration for Raspberry Pi GPIO buttons
- Lower power consumption (no current flow when button is not pressed)
- More robust against floating inputs

### Internal Pull-up Configuration

The BCM2835 GPIO controller has configurable internal pull-up/pull-down resistors (~50kΩ typical). These are enabled via device tree:

```dts
gpios = <&gpio 5 GPIO_ACTIVE_LOW>;
```

The `GPIO_ACTIVE_LOW` flag tells the kernel to:
1. Enable internal pull-up resistor on the GPIO pin
2. Invert the logic (reading 0 = button pressed, 1 = button released)

### External Pull-up Option (Advanced)

If you prefer external pull-up resistors for any reason:

- **Resistor value:** 10kΩ (standard value for GPIO buttons)
- **Connection:** Between GPIO pin and 3.3V
- **Rating:** 1/4W or 1/8W resistors are sufficient

**Schematic (per button):**
```
3.3V ----[ 10kΩ ]---- GPIO_X ----[Button]---- GND
```

**Note:** When using external pull-ups, you can omit the internal pull-up by using `GPIO_PULL_UP` flag in device tree, but it's simpler to rely on internal pull-ups.

### Debouncing Strategy

Button bounce is handled by two mechanisms:

1. **Hardware debouncing (device tree):**
   - `debounce-interval = <20>;` in device tree (20ms typical)
   - Linux GPIO subsystem filters rapid transitions

2. **Software debouncing (application layer):**
   - Additional filtering in button-test application
   - Ignores repeated events within 50ms window
   - Belt-and-suspenders approach for reliability

## Physical Layout

### Recommended Button Placement

```
┌────────────────────────────────────┐
│                                    │
│  ┌──────────────────────────────┐  │
│  │                              │  │
│  │                              │  │
│  │    Waveshare 4.2" Display   │  │
│  │        (400 x 300)           │  │
│  │                              │  │
│  │                              │  │
│  │                              │  │
│  └──────────────────────────────┘  │
│                                    │
│              [MENU]                │
│                                    │
│        [UP]          [SELECT]      │
│                                    │
│       [DOWN]          [BACK]       │
│                                    │
└────────────────────────────────────┘
```

### Alternative Layout (D-Pad Style)

```
┌────────────────────────────────────┐
│                                    │
│  ┌──────────────────────────────┐  │
│  │                              │  │
│  │    Waveshare 4.2" Display   │  │
│  │        (400 x 300)           │  │
│  └──────────────────────────────┘  │
│                                    │
│              [MENU]                │
│                                    │
│              [UP]                  │
│        [BACK] [SELECT] [  ]        │
│             [DOWN]                 │
│                                    │
└────────────────────────────────────┘
```

### Side-Mounted Buttons (Vertical Reading)

For e-reader held in portrait orientation:

```
       Left Side              Right Side
            ↓                      ↓

    [MENU]  ┌─────────┐  [SELECT]
            │         │
    [UP]    │ Display │  [BACK]
            │         │
    [DOWN]  └─────────┘  [    ]
```

Choose the layout that best fits your enclosure design and personal preference.

## Button Specifications

### Recommended Tactile Switches

| Specification | Value | Notes |
|---------------|-------|-------|
| **Type** | Through-hole tactile switch | |
| **Size** | 6x6mm or 12x12mm | 12x12mm easier to press |
| **Actuation Force** | 160gf - 260gf | Standard tactile feel |
| **Travel** | 0.25mm - 0.5mm | Typical for tactile switches |
| **Lifetime** | 100,000+ cycles minimum | |
| **Mounting** | Through-hole PCB or perfboard | |

### Part Numbers (Examples)

- **6x6mm:** Omron B3F-1000, E-Switch TL1105
- **12x12mm:** Omron B3F-4055, C&K PTS645
- **Panel mount:** 12mm momentary push button switches

See [[BOM]] for specific purchase recommendations.

## Wiring Instructions

### Quick Reference Wiring

1. **Button 1 (UP):** GPIO 5 (Pin 29) → Button → GND (Pin 34 or 39)
2. **Button 2 (DOWN):** GPIO 6 (Pin 31) → Button → GND (Pin 34 or 39)
3. **Button 3 (SELECT):** GPIO 13 (Pin 33) → Button → GND (Pin 34 or 39)
4. **Button 4 (BACK):** GPIO 19 (Pin 35) → Button → GND (Pin 34 or 39)
5. **Button 5 (MENU):** GPIO 26 (Pin 37) → Button → GND (Pin 34 or 39)

### Common Ground Bus

All buttons share a common ground connection:
- Use a breadboard ground rail, or
- Solder all button grounds together on perfboard, or
- Design a custom PCB with ground plane

### Pin Header Location

All button GPIO pins are conveniently located on the bottom-right of the GPIO header:

```
GPIO Header (Bottom-Right Section):
        ...
Pin 27: GPIO 0          Pin 28: GPIO 1
Pin 29: GPIO 5          Pin 30: GND
Pin 31: GPIO 6          Pin 32: GPIO 12
Pin 33: GPIO 13         Pin 34: GND
Pin 35: GPIO 19         Pin 36: GPIO 16
Pin 37: GPIO 26         Pin 38: GPIO 20
Pin 39: GND             Pin 40: GPIO 21
```

## Testing Button Connections

### Using Device Tree GPIO Sysfs

Before implementing the full button driver, test individual buttons:

```bash
# Export GPIO pin for button testing
echo 5 > /sys/class/gpio/export

# Set as input
echo in > /sys/class/gpio/gpio5/direction

# Read button state (should show "1" when not pressed, "0" when pressed)
cat /sys/class/gpio/gpio5/value

# Press the button and read again
cat /sys/class/gpio/gpio5/value

# Clean up
echo 5 > /sys/class/gpio/unexport
```

Repeat for each button GPIO (5, 6, 13, 19, 26).

### Using evtest Utility

Once the device tree overlay is loaded:

```bash
# List input devices
evtest

# Select the gpio-keys device and press buttons
# Should see EV_KEY events with the correct key codes
```

### Expected Output

```
Event: time 1234567890.123456, type 1 (EV_KEY), code 103 (KEY_UP), value 1
Event: time 1234567890.223456, type 1 (EV_KEY), code 103 (KEY_UP), value 0
```

- `value 1` = button pressed
- `value 0` = button released

## Future Expansion

### Reserved GPIO Pins

The following GPIO pins remain available for future features:

| GPIO | Physical Pin | Potential Use |
|------|--------------|---------------|
| GPIO 12 | Pin 32 | Additional button, PWM for backlight |
| GPIO 16 | Pin 36 | Additional button, status LED |
| GPIO 20 | Pin 38 | Additional button |
| GPIO 21 | Pin 40 | Additional button |

### Additional Features

- **Power button:** Could use GPIO 3 with `gpio-shutdown` overlay
- **Status LED:** Use GPIO 16 or 12 with PWM for brightness control
- **Backlight control:** GPIO 12 has PWM capability for future backlit display
- **Accelerometer:** I2C pins (GPIO 2/3) reserved for orientation detection

## Troubleshooting

### Button Not Responding

1. **Check wiring:**
   - Verify correct GPIO pin and GND connection
   - Ensure button is making contact when pressed
   - Check for loose jumper wires

2. **Test GPIO directly:**
   - Use sysfs method above to read GPIO value
   - Should toggle between 0 and 1 when button pressed

3. **Verify device tree:**
   - Check that overlay is loaded: `ls /proc/device-tree/gpio-keys/`
   - Verify button definitions in compiled overlay

### Multiple Button Presses Detected

1. **Debouncing issue:**
   - Increase debounce interval in device tree (try 50ms)
   - Add software debouncing in application

2. **Electrical noise:**
   - Add 0.1µF capacitor between GPIO pin and GND (hardware debounce)
   - Keep button wires short and away from power lines

3. **Button quality:**
   - Some cheap tactile switches have poor contact
   - Replace with higher quality switches

### Wrong Button Detected

1. **Pin mapping error:**
   - Verify physical pin number vs GPIO number
   - Double-check wiring against this document

2. **Device tree mismatch:**
   - Ensure device tree GPIO assignments match physical wiring
   - Recompile overlay if changes made

## References

- [[WIRING_GUIDE]] - Complete wiring instructions including buttons
- [[BOM]] - Button part numbers and purchasing information
- [Linux Input Event Codes](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h)
- [BCM2835 GPIO Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)

## Revision History

- 2026-01-13: Initial button layout documentation created
