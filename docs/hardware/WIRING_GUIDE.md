---
type: reference
title: Wiring Guide - Raspberry Pi Zero W to Waveshare 4.2" E-Paper Display
created: 2026-01-13
tags:
  - hardware
  - wiring
  - display
  - raspberry-pi
  - waveshare
related:
  - "[[BOM]]"
  - "[[DISPLAY_SPECS]]"
---

# Wiring Guide: Raspberry Pi Zero W to Waveshare 4.2" E-Paper Display

This guide provides detailed instructions for connecting the Waveshare 4.2" E-Paper Display Module (Rev 2.2) to the Raspberry Pi Zero W using SPI communication.

## Overview

The Waveshare 4.2" e-paper display communicates with the Raspberry Pi Zero W via the SPI (Serial Peripheral Interface) protocol. The display requires:
- 4-wire SPI connection (MOSI, MISO, SCLK, CS)
- 3 additional GPIO pins for control signals (DC, RST, BUSY)
- Power supply (3.3V and GND)

**Important:** Always power off the Raspberry Pi Zero W before making any GPIO connections.

## Pin Connections

### Pin Mapping Table

| Waveshare Display Pin | Description | RPi Zero W Pin | BCM GPIO | Notes |
|----------------------|-------------|----------------|----------|-------|
| VCC | Power (3.3V) | Pin 1 or 17 | 3.3V | Red wire recommended |
| GND | Ground | Pin 6, 9, 14, 20, 25, 30, 34, or 39 | GND | Black wire recommended |
| DIN (MOSI) | SPI Data In | Pin 19 | GPIO 10 (MOSI) | Master Out Slave In |
| CLK (SCLK) | SPI Clock | Pin 23 | GPIO 11 (SCLK) | Clock signal |
| CS | Chip Select | Pin 24 | GPIO 8 (CE0) | Active low |
| DC | Data/Command | Pin 22 | GPIO 25 | Low=command, High=data |
| RST | Reset | Pin 11 | GPIO 17 | Active low reset |
| BUSY | Busy Signal | Pin 18 | GPIO 24 | Output from display |

### Alternative Pin Configuration (if needed)

If you need to use different GPIO pins for DC, RST, or BUSY, any available GPIO can be used. The SPI pins (MOSI, SCLK, CS) should remain on the hardware SPI interface for best performance.

**Common alternative assignments:**
- DC: GPIO 25, GPIO 22, GPIO 27
- RST: GPIO 17, GPIO 27, GPIO 22
- BUSY: GPIO 24, GPIO 23, GPIO 18

**Important:** If you change pin assignments, you must update the device tree overlay and driver configuration accordingly.

## Raspberry Pi Zero W GPIO Pinout Reference

```
                 3.3V (1) (2) 5V
       GPIO 2 / I2C SDA (3) (4) 5V
       GPIO 3 / I2C SCL (5) (6) GND
            GPIO 4 GPCLK0 (7) (8) GPIO 14 / TXD
                      GND (9) (10) GPIO 15 / RXD
                  GPIO 17 (11) (12) GPIO 18 / PCM_CLK
                  GPIO 27 (13) (14) GND
                  GPIO 22 (15) (16) GPIO 23
                     3.3V (17) (18) GPIO 24
GPIO 10 / MOSI / SPI_MOSI (19) (20) GND
GPIO 9 / MISO / SPI_MISO (21) (22) GPIO 25
GPIO 11 / SCLK / SPI_CLK (23) (24) GPIO 8 / CE0
                      GND (25) (26) GPIO 7 / CE1
         GPIO 0 / ID_SD (27) (28) GPIO 1 / ID_SC
                  GPIO 5 (29) (30) GND
                  GPIO 6 (31) (32) GPIO 12
                 GPIO 13 (33) (34) GND
                 GPIO 19 (35) (36) GPIO 16
                 GPIO 26 (37) (38) GPIO 20
                      GND (39) (40) GPIO 21
```

## Wiring Instructions

### Method 1: Soldered Connection (Recommended for permanent installation)

1. **Prepare the Display Module**
   - If your Waveshare display came with a header, ensure it's properly soldered
   - Identify the 8 pins needed: VCC, GND, DIN, CLK, CS, DC, RST, BUSY

2. **Prepare the Raspberry Pi Zero W**
   - Solder a 40-pin header to the GPIO pads if not already installed
   - Use a standard 2x20 pin header (2.54mm pitch)

3. **Connect with Jumper Wires**
   - Use female-to-female jumper wires (if both boards have headers)
   - Recommended: Use color-coded wires (Red=3.3V, Black=GND, other colors for signals)
   - Follow the pin mapping table above

4. **Verify Connections**
   - Double-check each connection against the table
   - Ensure no crossed wires or short circuits
   - Verify 3.3V (NOT 5V) is connected to VCC

### Method 2: Ribbon Cable Connection

Some Waveshare displays include a ribbon cable and adapter board. If using this:

1. Connect the ribbon cable to the Waveshare display's FPC connector
2. Connect the adapter board to the Raspberry Pi GPIO header
3. Verify the pin mapping matches the table above (consult Waveshare documentation)

### Method 3: Custom PCB (Advanced)

For a permanent installation, consider designing a custom PCB or HAT that:
- Mounts directly on the Raspberry Pi GPIO header
- Includes level shifters (optional, e-paper typically 3.3V compatible)
- Adds additional components (buttons, battery management, etc.)

## Testing the Connection

### Step 1: Visual Inspection

Before powering on:
1. Verify all connections match the pin mapping table
2. Check for loose wires or potential shorts
3. Ensure the display is firmly seated (if using a connector)

### Step 2: Power-On Check

1. Connect power to the Raspberry Pi Zero W
2. Watch the display during boot:
   - The display may flicker or show static during initialization
   - This is normal behavior for e-paper displays

### Step 3: Software Verification

Once the system is booted with the custom firmware:

1. **Check SPI Interface:**
   ```bash
   ls /dev/spi*
   # Should show: /dev/spidev0.0 and /dev/spidev0.1
   ```

2. **Check GPIO Availability:**
   ```bash
   # For each GPIO pin used (17, 24, 25):
   ls /sys/class/gpio/gpio17
   ls /sys/class/gpio/gpio24
   ls /sys/class/gpio/gpio25
   ```

3. **Run Display Test:**
   ```bash
   /usr/bin/display-test
   # Should display "Hello E-Reader" on the e-paper screen
   ```

## Troubleshooting

### Display Not Responding

1. **Check power supply:**
   - Ensure 3.3V is present on VCC pin (measure with multimeter)
   - Verify ground connection is solid

2. **Verify SPI is enabled:**
   ```bash
   # Check if SPI modules are loaded
   lsmod | grep spi
   ```

3. **Check GPIO pin states:**
   ```bash
   # Export and read GPIO pins
   echo 17 > /sys/class/gpio/export
   cat /sys/class/gpio/gpio17/value
   ```

4. **Check for physical issues:**
   - Reseat all connections
   - Check for bent pins
   - Verify no solder bridges on GPIO header

### Display Shows Garbage or Partial Image

1. **SPI timing issues:**
   - Try reducing SPI clock speed in device tree overlay
   - Check for loose wiring on SCLK, MOSI, or CS pins

2. **Incorrect pin assignments:**
   - Verify DC, RST, and BUSY pins match the driver configuration
   - Double-check pin numbers vs. GPIO numbers (they're different!)

3. **Power supply issues:**
   - E-paper displays can draw significant current during refresh
   - Use a 2A+ power supply for stable operation

### No /dev/spidev Device

1. **Enable SPI in device tree:**
   - Check that SPI is enabled in `/boot/config.txt` or device tree overlay
   - Reboot after making changes

2. **Check kernel config:**
   ```bash
   zcat /proc/config.gz | grep SPI
   # Should show: CONFIG_SPI=y, CONFIG_SPI_BCM2835=y
   ```

## Important Notes

- **Voltage:** The Raspberry Pi GPIO runs at 3.3V. Do NOT connect 5V to display VCC.
- **Static Sensitivity:** E-paper displays can be sensitive to static discharge. Handle with care.
- **Wire Length:** Keep wires as short as possible to reduce interference on SPI signals.
- **SPI Mode:** Waveshare 4.2" typically uses SPI Mode 0 (CPOL=0, CPHA=0).
- **Max SPI Speed:** Start with 4MHz and increase if stable (max typically 10-15MHz).

## Additional Resources

- [Raspberry Pi GPIO Pinout](https://pinout.xyz/)
- [Waveshare 4.2" E-Paper Display Wiki](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- [[DISPLAY_SPECS]] - Technical specifications for the display
- [[BOM]] - Complete parts list including wire recommendations

## Button Wiring (Phase 2)

### Overview

Phase 2 adds 5 tactile push buttons for navigation. The buttons use internal pull-up resistors and are wired in an active-low configuration (button press connects GPIO to GND).

### Button Pin Connections

| Button | GPIO | Physical Pin | Connection |
|--------|------|--------------|------------|
| UP | GPIO 5 | Pin 29 | GPIO 5 → Button → GND |
| DOWN | GPIO 6 | Pin 31 | GPIO 6 → Button → GND |
| SELECT | GPIO 13 | Pin 33 | GPIO 13 → Button → GND |
| BACK | GPIO 19 | Pin 35 | GPIO 19 → Button → GND |
| MENU | GPIO 26 | Pin 37 | GPIO 26 → Button → GND |
| Common GND | - | Pin 34, 39 | Shared ground for all buttons |

### Button Wiring Schematic

```
Button Configuration (Active-Low with Internal Pull-up):

For each button:
3.3V (internal pull-up ~50kΩ)
    │
    ├── GPIO Pin (5, 6, 13, 19, or 26)
    │
    ├── [Tactile Switch] (normally open)
    │
    └── GND (Pin 34 or 39)
```

When button is **not pressed:** GPIO reads HIGH (1)
When button **is pressed:** GPIO reads LOW (0)

### Step-by-Step Button Wiring

1. **Identify Button Locations**
   - Choose physical placement for 5 buttons (see [[BUTTON_LAYOUT]] for suggestions)
   - Keep buttons accessible around the display perimeter

2. **Prepare Buttons**
   - Use 6x6mm or 12x12mm tactile switches (see [[BOM]] for part numbers)
   - If using perfboard, insert buttons and solder in place
   - If using breadboard, insert directly

3. **Connect Button GPIOs**
   - UP button: Connect one terminal to GPIO 5 (Pin 29)
   - DOWN button: Connect one terminal to GPIO 6 (Pin 31)
   - SELECT button: Connect one terminal to GPIO 13 (Pin 33)
   - BACK button: Connect one terminal to GPIO 19 (Pin 35)
   - MENU button: Connect one terminal to GPIO 26 (Pin 37)

4. **Connect Common Ground**
   - Connect the other terminal of ALL buttons to a common ground rail
   - Connect the common ground to Raspberry Pi GND (Pin 34 or 39)
   - Alternative: Wire each button ground directly to nearby GND pins

5. **Verify Connections**
   - Double-check each GPIO pin assignment against [[BUTTON_LAYOUT]]
   - Ensure no shorts between adjacent GPIO pins
   - Verify all buttons share a common ground

### GPIO Header Section for Buttons

The buttons use pins 29-39 on the GPIO header (bottom-right corner):

```
Pin 29: GPIO 5  [UP]        Pin 30: GND
Pin 31: GPIO 6  [DOWN]      Pin 32: GPIO 12 (unused)
Pin 33: GPIO 13 [SELECT]    Pin 34: GND [Common ground]
Pin 35: GPIO 19 [BACK]      Pin 36: GPIO 16 (unused)
Pin 37: GPIO 26 [MENU]      Pin 38: GPIO 20 (unused)
Pin 39: GND [Common ground] Pin 40: GPIO 21 (unused)
```

### Combined Display + Buttons Wiring

Complete pin assignment showing both display and buttons:

| Pin # | GPIO/Function | Usage | Connected To |
|-------|---------------|-------|--------------|
| 1 | 3.3V | Display power | Display VCC |
| 6 | GND | Display ground | Display GND |
| 11 | GPIO 17 | Display reset | Display RST |
| 18 | GPIO 24 | Display busy | Display BUSY |
| 19 | GPIO 10 (MOSI) | Display data | Display DIN |
| 22 | GPIO 25 | Display DC | Display DC |
| 23 | GPIO 11 (SCLK) | Display clock | Display CLK |
| 24 | GPIO 8 (CE0) | Display CS | Display CS |
| 29 | GPIO 5 | UP button | Button → GND |
| 31 | GPIO 6 | DOWN button | Button → GND |
| 33 | GPIO 13 | SELECT button | Button → GND |
| 34 | GND | Button ground | All button commons |
| 35 | GPIO 19 | BACK button | Button → GND |
| 37 | GPIO 26 | MENU button | Button → GND |

### Testing Button Wiring

Before loading the device tree overlay, test buttons using GPIO sysfs:

```bash
# Test UP button (GPIO 5)
echo 5 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio5/direction

# Read value (should be 1 when not pressed)
cat /sys/class/gpio/gpio5/value

# Press the UP button and read again (should be 0)
cat /sys/class/gpio/gpio5/value

# Clean up
echo 5 > /sys/class/gpio/unexport
```

Repeat for all button GPIOs (5, 6, 13, 19, 26) to verify wiring.

### Troubleshooting Button Wiring

**Button always reads 0 (pressed):**
- GPIO shorted to ground - check for solder bridges or crossed wires
- Verify correct GPIO pin, not a permanent GND pin

**Button always reads 1 (not pressed):**
- Button not making contact when pressed
- Broken wire or loose connection
- Button wired backwards (swap the two terminals)

**Button reads random values:**
- Loose connection - check jumper wire contacts
- Missing ground connection
- Electrical noise - keep button wires short and away from power lines

**Multiple buttons trigger simultaneously:**
- Common ground not properly connected
- Crossed wiring - verify each GPIO goes to correct button only

### Optional: Hardware Debouncing

For improved reliability, add a 0.1µF (100nF) ceramic capacitor between each GPIO pin and GND:

```
GPIO Pin ----+---- [Button] ---- GND
             |
          [0.1µF]
             |
            GND
```

This hardware debouncing complements the software debouncing in the device tree and application.

## Revision History

- 2026-01-13: Added button wiring section (Phase 2)
- 2026-01-13: Initial wiring guide created
