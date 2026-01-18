---
type: reference
title: Hardware Schematic Documentation - Open Source E-Reader
created: 2026-01-17
tags:
  - hardware
  - schematic
  - electrical
  - design
  - circuit
related:
  - "[[WIRING_GUIDE]]"
  - "[[BOM]]"
  - "[[DISPLAY_SPECS]]"
  - "[[BUTTON_LAYOUT]]"
  - "[[POWER_SUPPLY]]"
---

# Hardware Schematic Documentation

This document provides comprehensive electrical schematic documentation for the Open Source E-Reader project. It describes all electrical connections, component values, signal specifications, and design decisions.

## Table of Contents

1. [Schematic Overview](#schematic-overview)
2. [System Architecture](#system-architecture)
3. [Display SPI Interface](#display-spi-interface)
4. [Button Input System](#button-input-system)
5. [Power Supply Circuit](#power-supply-circuit)
6. [GPIO Pin Assignments](#gpio-pin-assignments)
7. [Electrical Specifications](#electrical-specifications)
8. [Design Guidelines](#design-guidelines)
9. [Schematic Files](#schematic-files)
10. [References](#references)

## Schematic Overview

The e-reader consists of four main electrical subsystems:

1. **Display Interface** - SPI communication with Waveshare 4.2" e-paper display
2. **Button Input System** - 5 GPIO buttons with internal pull-up configuration
3. **Power Supply** - 5V input with optional battery and monitoring
4. **Controller** - Raspberry Pi Zero W (BCM2835 SoC)

**Key Specifications:**
- **Operating Voltage:** 5V primary, 3.3V logic
- **Power Consumption:** 120-275mA (0.6-1.4W)
- **Interface Protocols:** SPI (display), I2C (optional battery monitor), GPIO (buttons)
- **Component Count:** Minimal - 1 controller, 1 display, 5 buttons (+ optional components)

## System Architecture

### Block Diagram

```
┌────────────────┐
│  5V Power PSU  │
│   (Micro-USB)  │
└───────┬────────┘
        │
        ├─────────────┐
        │             │
        │    ┌────────┴───────────┐
        │    │  Optional Battery  │
        │    │  & UPS HAT (U2)    │
        │    │  INA219 Monitor    │
        │    └────────┬───────────┘
        │             │
        ▼             ▼
┌────────────────────────────────┐
│   Raspberry Pi Zero W (U1)     │
│   BCM2835 @ 1GHz, 512MB RAM    │
│   40-pin GPIO Header           │
├────────────────────────────────┤
│  SPI0  │  I2C1  │  GPIO Inputs │
└───┬──────────┬──────────┬──────┘
    │          │          │
    ▼          ▼          ▼
┌──────┐  ┌────────┐  ┌────────┐
│ J1:  │  │  U2:   │  │ SW1-5: │
│E-Ink │  │Battery │  │Buttons │
│Disp. │  │Monitor │  │(5x)    │
└──────┘  └────────┘  └────────┘
```

### Component Reference Designators

| Ref | Component | Description |
|-----|-----------|-------------|
| **U1** | Raspberry Pi Zero W | Main controller (BCM2835 SoC, 512MB RAM, WiFi) |
| **J1** | Waveshare 4.2" Display | E-paper display module (400×300, Rev 2.2) |
| **SW1** | UP Button | Tactile switch (6×6mm or 12×12mm) |
| **SW2** | DOWN Button | Tactile switch (6×6mm or 12×12mm) |
| **SW3** | SELECT Button | Tactile switch (6×6mm or 12×12mm) |
| **SW4** | BACK Button | Tactile switch (6×6mm or 12×12mm) |
| **SW5** | MENU Button | Tactile switch (6×6mm or 12×12mm) |
| **C1-C5** | Debounce Caps (Optional) | 0.1µF ceramic capacitors |
| **J2** | Power Input | Micro-USB Type B connector |
| **J3** | Battery Connector (Optional) | JST PH 2-pin connector |
| **U2** | Battery Monitor (Optional) | INA219 I2C power monitor IC |

## Display SPI Interface

### Schematic Diagram

```
Raspberry Pi Zero W (U1)                    Waveshare E-Paper Display (J1)

    3.3V (Pin 1) ────────────────────────── VCC (Pin 1)

    GND (Pin 6)  ────────────────────────── GND (Pin 2)

    GPIO 10 (Pin 19) ──┐
    [MOSI, SPI0]       └─────────────────── DIN (Pin 3)

    GPIO 11 (Pin 23) ──┐
    [SCLK, SPI0]       └─────────────────── CLK (Pin 4)

    GPIO 8 (Pin 24) ───┐
    [CE0, SPI0]        └─────────────────── CS (Pin 5)

    GPIO 25 (Pin 22) ──┐
    [General Purpose]  └─────────────────── DC (Pin 6)

    GPIO 17 (Pin 11) ──┐
    [General Purpose]  └─────────────────── RST (Pin 7)

    GPIO 24 (Pin 18) ──┐
    [General Purpose]  └─────────────────── BUSY (Pin 8)
```

### Display Connector Pinout (J1)

| Pin | Signal | Direction | Pi GPIO | Pi Pin | Description |
|-----|--------|-----------|---------|--------|-------------|
| 1 | VCC | Input | 3.3V | 1 | Power supply (3.3V, 26mA max) |
| 2 | GND | - | GND | 6 | Ground reference |
| 3 | DIN | Input | GPIO 10 | 19 | SPI MOSI - Data from Pi to display |
| 4 | CLK | Input | GPIO 11 | 23 | SPI Clock - Serial clock signal |
| 5 | CS | Input | GPIO 8 | 24 | SPI Chip Select (active low) |
| 6 | DC | Input | GPIO 25 | 22 | Data/Command control (L=cmd, H=data) |
| 7 | RST | Input | GPIO 17 | 11 | Reset signal (active low, 10ms pulse) |
| 8 | BUSY | Output | GPIO 24 | 18 | Busy status (H=busy, L=ready) |

### SPI Bus Configuration

- **SPI Device:** `/dev/spidev0.0` (SPI0, CE0)
- **SPI Mode:** Mode 0 (CPOL=0, CPHA=0)
  - Clock idle state: LOW
  - Data sampled on rising edge
- **Clock Frequency:** 4MHz typical, 10MHz maximum
- **Bit Order:** MSB first (most significant bit transmitted first)
- **Data Width:** 8-bit transfers

### Signal Timing Requirements

**Reset Pulse (RST):**
```
RST: ───┐           ┌─────────────
        │           │
        └───────────┘
         ← 10ms →
```

**BUSY Signal Monitoring:**
```
BUSY: ───────┐                    ┌────
             │   Display updating │
             └────────────────────┘
              ← ~2 seconds (full refresh) →
```

Must wait for BUSY to go LOW before:
- Sending new display commands
- Updating display content
- Sending power-off commands

**DC Signal Timing:**
```
CS:  ────┐                    ┌────
         └────────────────────┘

DC:  ────┐     ┌─────────────────
         │     │  (must be stable)
         └─────┘
         ← setup time
```

DC must be stable before CS goes LOW.

### Display Power Consumption

| Mode | Current | Voltage | Power | Duration |
|------|---------|---------|-------|----------|
| Refresh | 26mA | 3.3V | 85.8mW | ~2 seconds |
| Standby | <0.01mA | 3.3V | <0.033mW | Indefinite |

**Note:** Display retains image without power (bistable e-ink technology).

## Button Input System

### Schematic Diagram (Per Button)

```
                    Raspberry Pi Zero W

                           3.3V
                            │
                        [~50kΩ]  ← Internal Pull-Up Resistor
                            │     (enabled via device tree)
            GPIO Pin ───────┼─────── To BCM2835 GPIO Controller
                            │
                         [SW_X]   ← Tactile Switch (NO)
                            │
                           GND

Optional Hardware Debouncing:

            GPIO Pin ───────┼─────── To BCM2835 GPIO Controller
                            │
                          [0.1µF] ← C1-C5 (ceramic capacitor)
                            │
                           GND
```

### Button Connection Table

| Button | Function | Pi GPIO | Pi Pin | Switch Terminal 1 | Switch Terminal 2 |
|--------|----------|---------|--------|-------------------|-------------------|
| **SW1** | UP | GPIO 5 | 29 | Connect to GPIO 5 | Connect to GND |
| **SW2** | DOWN | GPIO 6 | 31 | Connect to GPIO 6 | Connect to GND |
| **SW3** | SELECT | GPIO 13 | 33 | Connect to GPIO 13 | Connect to GND |
| **SW4** | BACK | GPIO 19 | 35 | Connect to GPIO 19 | Connect to GND |
| **SW5** | MENU | GPIO 26 | 37 | Connect to GPIO 26 | Connect to GND |

**Common Ground:** All buttons share common ground at Pin 34 and/or Pin 39.

### Button Logic Levels

| State | GPIO Level | Voltage | Switch Position |
|-------|------------|---------|-----------------|
| **Not Pressed** | HIGH (1) | ~3.3V | Open (NO contact) |
| **Pressed** | LOW (0) | ~0V | Closed (connected to GND) |

**Pull-up Configuration:**
- Internal BCM2835 pull-up resistors: ~50kΩ typical (range: 50-65kΩ)
- Enabled via device tree: `GPIO_ACTIVE_LOW` flag
- No external resistors required

### Linux Input Event Mapping

| Button | GPIO | Event Code | Event Name | Numeric Value |
|--------|------|------------|------------|---------------|
| SW1 (UP) | 5 | KEY_UP | Arrow Up | 103 |
| SW2 (DOWN) | 6 | KEY_DOWN | Arrow Down | 108 |
| SW3 (SELECT) | 13 | KEY_ENTER | Enter/Return | 28 |
| SW4 (BACK) | 19 | KEY_ESC | Escape | 1 |
| SW5 (MENU) | 26 | KEY_MENU | Menu | 139 |

Defined in Linux kernel: `linux/input-event-codes.h`

### Debouncing Strategy

Three-layer debouncing approach:

1. **Optional Hardware Debouncing (C1-C5):**
   - 0.1µF ceramic capacitor between GPIO and GND
   - RC time constant: τ = R × C = 50kΩ × 0.1µF = 5ms
   - Filters high-frequency bounce

2. **Device Tree Debouncing:**
   ```dts
   debounce-interval = <20>;  /* 20 milliseconds */
   ```
   - Linux GPIO subsystem filters rapid transitions
   - Configurable in device tree overlay

3. **Software Debouncing (Application Layer):**
   - Additional 50ms filter in button event handler
   - Ignores repeated events within time window
   - Provides belt-and-suspenders reliability

**Total Effective Debounce:** ~70-75ms (hardware + device tree + software)

### Button Component Specifications

**Tactile Switch Requirements:**
- **Type:** Through-hole tactile switch, momentary (NO)
- **Size:** 6×6mm or 12×12mm footprint
- **Actuation Force:** 160-260gf (gram-force)
- **Travel Distance:** 0.25-0.5mm
- **Contact Bounce:** <5ms
- **Lifetime:** 100,000 cycles minimum (500,000+ for SELECT button)
- **Contact Resistance:** <100mΩ
- **Insulation Resistance:** >100MΩ

**Recommended Parts:**
- Omron B3F-1000 (6×6mm, $0.30)
- Omron B3F-4055 (12×12mm, $0.50)
- E-Switch TL1105 (6×6mm, $0.25)
- C&K PTS645 (12×12mm, $0.45)

## Power Supply Circuit

### Basic Power Schematic

```
                        Primary Power (No Battery)

  Micro-USB (J2)
   5V Input
      │
      ├─── VCC (5V) ───┐
      │                │
      └─── GND ────────┼─── GND
                       │
                       ▼
           ┌───────────────────────┐
           │  Raspberry Pi Zero W  │
           │      (U1)             │
           │                       │
           │  ┌─────────────────┐  │
           │  │  BCM2835 SoC    │  │
           │  │  + Regulators   │  │
           │  └────────┬────────┘  │
           │           │           │
           │     3.3V Regulator    │
           │           │           │
           └───────────┼───────────┘
                       │
                       ├─── 3.3V Rail (50mA max available)
                       │     │
                       │     ├─── Display VCC (26mA peak)
                       │     └─── I2C Pull-ups
                       │
                       └─── GND
```

### Power Distribution

| Rail | Voltage | Max Current | Source | Load |
|------|---------|-------------|--------|------|
| **5V** | 5.0V ± 0.25V | 2A | Micro-USB PSU | Pi Zero W input |
| **3.3V** | 3.3V ± 0.1V | 50mA available | Pi onboard regulator | Display, I2C |
| **GND** | 0V | - | Common reference | All grounds |

**Power Budget:**

| Component | Typical | Peak | Notes |
|-----------|---------|------|-------|
| Pi Zero W (idle) | 120mA | 150mA | WiFi disabled, CPU idle |
| Pi Zero W (active) | 150mA | 200mA | Normal operation |
| Pi Zero W (WiFi) | 200mA | 300mA | WiFi transmission |
| Display Refresh | 26mA | 26mA | 2-second duration |
| **Total** | **~175mA** | **~300mA** | Typical reading mode |

**Power Supply Requirements:**
- Input: 5V 2A minimum (2.5A recommended)
- Quality: Official Raspberry Pi PSU strongly recommended
- Ripple: <100mVpp
- Voltage droop: <200mV during load transients

### Optional Battery Circuit

```
        Battery Power with UPS HAT and Monitoring

    LiPo Battery (J3)         USB Charger (J2)
    3.7V, 2-5Ah                    5V 2A
         │                           │
         │        ┌──────────────────┤
         │        │  Charge Control  │
         └────────┤  & Protection    │
                  │  (on UPS HAT)    │
                  └────────┬─────────┘
                           │
                    Boost Converter
                    (3.7V → 5V)
                           │
                           ├───── 5V to Pi Zero W
                           │
                  ┌────────┴─────────┐
                  │   INA219 (U2)    │
                  │  Power Monitor   │
                  │  I2C @ 0x40      │
                  └────────┬─────────┘
                           │
                    I2C Bus (SDA/SCL)
                           │
                  ┌────────┴─────────┐
                  │  Pi Zero W GPIO  │
                  │  GPIO 2 (SDA)    │
                  │  GPIO 3 (SCL)    │
                  └──────────────────┘
```

### INA219 Power Monitor (U2 - Optional)

**Pin Connections:**

| INA219 Pin | Function | Connection | Pi Zero Pin |
|------------|----------|------------|-------------|
| VCC | Power | 3.3V | Pin 1 |
| GND | Ground | GND | Pin 9 |
| SDA | I2C Data | GPIO 2 (SDA) | Pin 3 |
| SCL | I2C Clock | GPIO 3 (SCL) | Pin 5 |
| VIN+ | Voltage Sense + | Battery/Supply + | - |
| VIN- | Voltage Sense - | Battery/Supply - | - |

**I2C Configuration:**
- **Bus:** I2C1 (`/dev/i2c-1`)
- **Default Address:** 0x40 (configurable: 0x40, 0x41, 0x44, 0x45)
- **Pull-up Resistors:** 1.8kΩ on Pi Zero W (built-in)
- **Clock Speed:** 100kHz (standard mode)
- **Data Rate:** 12-bit ADC resolution

**Measured Parameters:**
- Bus voltage: 0-26V range
- Shunt voltage: ±320mV (with 0.1Ω shunt)
- Current: Calculated from shunt voltage
- Power: Calculated from voltage × current

### Battery Specifications

**Single-Cell Li-Po / Li-ion:**

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Nominal Voltage** | 3.7V | Average operating voltage |
| **Capacity** | 2000-5000mAh | Recommended range |
| **Full Charge** | 4.2V | 100% state of charge |
| **Empty** | 3.2V | 0% - DO NOT discharge below |
| **Critical** | 3.3V | Initiate shutdown warning |
| **Protection** | Required | Over-charge, over-discharge, short-circuit |

**Estimated Battery Life:**

| Capacity | Reading Mode | WiFi Active | Notes |
|----------|--------------|-------------|-------|
| 2000mAh | ~10 hours | ~6 hours | 175mA avg / 275mA WiFi |
| 3000mAh | ~15 hours | ~9 hours | Most balanced option |
| 5000mAh | ~25 hours | ~15 hours | Maximum capacity |

*Assumptions: 85% battery efficiency, reading mode = 175mA average*

## GPIO Pin Assignments

### Complete Pin Allocation

```
Raspberry Pi Zero W 40-Pin GPIO Header
(Numbered 1-40, physical pin numbers)

         Function         Pin   Pin   Function
  ────────────────────────────────────────────────
  3.3V Power              [1]   [2]   5V Power
  I2C1 SDA (GPIO 2)       [3]   [4]   5V Power
  I2C1 SCL (GPIO 3)       [5]   [6]   Ground
  GPIO 4                  [7]   [8]   UART TXD (GPIO 14)
  Ground                  [9]   [10]  UART RXD (GPIO 15)
  Display RST (GPIO 17)   [11]  [12]  GPIO 18
  GPIO 27                 [13]  [14]  Ground
  GPIO 22                 [15]  [16]  GPIO 23
  3.3V Power              [17]  [18]  Display BUSY (GPIO 24)
  SPI MOSI (GPIO 10)      [19]  [20]  Ground
  SPI MISO (GPIO 9)       [21]  [22]  Display DC (GPIO 25)
  SPI SCLK (GPIO 11)      [23]  [24]  SPI CE0 (GPIO 8)
  Ground                  [25]  [26]  SPI CE1 (GPIO 7)
  ID_SD (Reserved)        [27]  [28]  ID_SC (Reserved)
  Button UP (GPIO 5)      [29]  [30]  Ground
  Button DOWN (GPIO 6)    [31]  [32]  GPIO 12
  Button SELECT (GPIO 13) [33]  [34]  Ground
  Button BACK (GPIO 19)   [35]  [36]  GPIO 16
  Button MENU (GPIO 26)   [37]  [38]  GPIO 20
  Ground                  [39]  [40]  GPIO 21
```

### Pin Usage Summary

| Function | GPIOs Used | Physical Pins | Status |
|----------|------------|---------------|--------|
| **Display SPI** | 8, 10, 11 | 19, 23, 24 | Used |
| **Display Control** | 17, 24, 25 | 11, 18, 22 | Used |
| **Buttons** | 5, 6, 13, 19, 26 | 29, 31, 33, 35, 37 | Used |
| **I2C (Optional)** | 2, 3 | 3, 5 | Reserved |
| **UART Debug** | 14, 15 | 8, 10 | Available |
| **Available** | 4, 12, 16, 18, 20, 21, 22, 23, 27 | Various | Expansion |

### Available GPIOs for Future Expansion

| GPIO | Physical Pin | Potential Use |
|------|--------------|---------------|
| GPIO 4 | 7 | PWM output, 1-Wire bus |
| GPIO 12 | 32 | PWM output, additional button |
| GPIO 16 | 36 | Additional button, status LED |
| GPIO 18 | 12 | PWM output (conflicts with PCM) |
| GPIO 20 | 38 | Additional button, PCM data |
| GPIO 21 | 40 | Additional button, PCM clock |
| GPIO 22 | 15 | General purpose I/O |
| GPIO 23 | 16 | General purpose I/O |
| GPIO 27 | 13 | General purpose I/O |

**Reserved but Unused:**
- GPIO 2, 3: I2C for battery monitor (if not installed)
- GPIO 9: SPI MISO (not needed for write-only display)
- GPIO 14, 15: UART TX/RX for serial console debugging

## Electrical Specifications

### GPIO Electrical Characteristics

**BCM2835 GPIO Controller:**

| Parameter | Min | Typ | Max | Unit | Notes |
|-----------|-----|-----|-----|------|-------|
| **Input Logic High (VIH)** | 2.0 | - | 3.3 | V | CMOS input |
| **Input Logic Low (VIL)** | 0 | - | 0.8 | V | CMOS input |
| **Output Logic High (VOH)** | 3.0 | 3.3 | 3.3 | V | @ 2mA load |
| **Output Logic Low (VOL)** | 0 | 0 | 0.4 | V | @ 2mA sink |
| **Input Leakage Current** | - | - | ±1 | µA | Tri-state |
| **Max Current per Pin** | - | - | 16 | mA | Per GPIO |
| **Internal Pull-up** | 50 | 50 | 65 | kΩ | Typical 50kΩ |
| **Internal Pull-down** | 50 | 50 | 65 | kΩ | Typical 50kΩ |
| **Input Capacitance** | - | 5 | 10 | pF | Typical |

**Important Notes:**
- Absolute maximum GPIO voltage: -0.5V to +3.6V (exceeding will damage SoC)
- Total current budget for all GPIOs: 50mA maximum
- Do NOT connect 5V signals directly to GPIO pins
- ESD rating: Human body model (HBM) 2kV typical

### Power Supply Specifications

**5V Rail (from Micro-USB):**

| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Input Voltage | 4.75 | 5.0 | 5.25 | V |
| Input Current | - | 175 | 500 | mA |
| Ripple Voltage (p-p) | - | - | 100 | mV |
| Transient Response | - | - | 200 | mV droop |

**3.3V Rail (from Pi Regulator):**

| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| Output Voltage | 3.2 | 3.3 | 3.4 | V |
| Available Current | - | - | 50 | mA |
| Load Regulation | - | - | 100 | mV |

**Note:** 3.3V rail powers display and I2C pull-ups. Do not exceed 50mA total external load.

### SPI Timing Specifications

**BCM2835 SPI0 Controller:**

| Parameter | Min | Typ | Max | Unit | Notes |
|-----------|-----|-----|-----|------|-------|
| **Clock Frequency** | 0 | 4 | 125 | MHz | Core clock / divider |
| **Display Max Clock** | - | 4 | 10 | MHz | IL0398 limit |
| **Setup Time (tsu)** | 10 | - | - | ns | Data before clock |
| **Hold Time (th)** | 10 | - | - | ns | Data after clock |
| **CS Setup Time** | 10 | - | - | ns | CS before clock |
| **CS Hold Time** | 10 | - | - | ns | CS after clock |

**Recommended Settings:**
- Clock speed: 4MHz (safe for all cable lengths)
- SPI mode: 0 (CPOL=0, CPHA=0)
- Bits per word: 8
- CS active low: Yes

### I2C Timing Specifications

**BCM2835 I2C1 Controller (Standard Mode):**

| Parameter | Min | Typ | Max | Unit | Notes |
|-----------|-----|-----|-----|------|-------|
| **Clock Frequency** | - | 100 | 100 | kHz | Standard mode |
| **Rise Time (tr)** | - | - | 1000 | ns | With 1.8kΩ pull-up |
| **Fall Time (tf)** | - | - | 300 | ns | |
| **Setup Time (tsu)** | 250 | - | - | ns | Data before clock |
| **Hold Time (th)** | 0 | - | - | ns | Data after clock |

**Pull-up Resistors:**
- Built-in: 1.8kΩ to 3.3V (on Pi Zero W)
- Bus capacitance: <400pF (short wires)
- Maximum bus length: <30cm for reliable operation

## Design Guidelines

### PCB Layout Recommendations

If designing a custom PCB or HAT:

#### Trace Width and Spacing

| Signal Type | Trace Width | Spacing | Impedance |
|-------------|-------------|---------|-----------|
| Power (5V, 3.3V) | ≥20mil (0.5mm) | 10mil | - |
| Ground | ≥30mil (0.75mm) | - | - |
| SPI Signals | 8-10mil (0.2-0.25mm) | 8mil | 50Ω |
| I2C Signals | 8-10mil | 8mil | - |
| GPIO (buttons) | 8mil | 8mil | - |

#### Ground Plane
- Use solid ground pour on bottom layer
- Via stitching: Place ground vias every 0.5" (12.7mm)
- Avoid splits in ground plane under SPI traces
- Connect all ground pins with low-impedance path

#### Component Placement
- Bypass capacitors: Place within 0.2" of IC power pins
- Buttons: Group together for easier wiring
- Display connector: Position near Pi GPIO header
- Keep SPI traces short and matched length

#### Decoupling and Filtering

| Location | Component | Value | Purpose |
|----------|-----------|-------|---------|
| Pi 5V input | Electrolytic | 10µF, 16V | Bulk capacitance |
| Pi 5V input | Ceramic | 0.1µF, 16V | High-freq bypass |
| Display VCC | Ceramic | 10µF, 6.3V | Supply filtering |
| Display VCC | Ceramic | 0.1µF, 16V | High-freq bypass |
| Button GPIOs (opt) | Ceramic | 0.1µF, 16V | Debouncing |

### Signal Integrity

**SPI Bus Design:**
- Maximum trace length: 10cm (4 inches)
- Match MOSI, SCLK, CS trace lengths within ±5mm
- Avoid routing SPI traces near WiFi antenna
- Use ground trace on either side of SPI signals (if multilayer)

**Cable Considerations:**
- Use twisted pair for MOSI/GND and SCLK/GND
- Maximum cable length: 15cm (6 inches)
- Use shielded cable for lengths >10cm
- Reduce clock frequency if longer cables required

### ESD Protection

**Recommended for Production Designs:**

| Signal | Protection Device | Part Example |
|--------|-------------------|--------------|
| SPI signals | TVS diode array | TPD4E02B04 |
| Button inputs | 1kΩ series resistor + TVS | PESD5V0S1BA |
| USB power | TVS diode | SMAJ5.0A |

**Not Required:**
- Development / prototype builds
- Internal devices (not exposed to user)
- When using proper enclosure

### Thermal Design

**Heat Dissipation:**

| Component | Typ Power | Max Power | Heatsink |
|-----------|-----------|-----------|----------|
| Pi Zero W | 0.75W | 1.5W | Not required |
| Display | 0.086W (refresh) | 0.086W | Not required |
| Battery charging | - | 5W | Ventilation recommended |

**Operating Temperature Range:**
- Pi Zero W: 0°C to 85°C (industrial)
- Display: 0°C to 50°C (see [[DISPLAY_SPECS]])
- Battery: 0°C to 45°C (charging), -20°C to 60°C (storage)

**Recommendations:**
- Provide ventilation slots in enclosure
- Avoid direct sunlight on display
- Monitor battery temperature during charging (if applicable)

## Schematic Files

### File Locations

All schematic files are located in the `hardware/schematics/` directory:

| File | Description | Format |
|------|-------------|--------|
| `complete-schematic.txt` | Full system schematic (ASCII) | Text |
| `display-schematic.txt` | Display interface detail | Text |
| `button-schematic.txt` | Button system detail | Text |
| `power-schematic.txt` | Power supply detail | Text |
| `README.md` | Schematic directory guide | Markdown |

**Future Files (to be created):**
- `ereader-fritzing.fzz` - Fritzing project file
- `ereader-kicad/` - KiCad project directory
- `schematic.pdf` - Exported PDF schematic
- `schematic.png` - Exported image schematic

### Creating PCB Schematics

See `hardware/schematics/README.md` for detailed instructions on:
- Using Fritzing to create schematics
- Using KiCad for professional PCB design
- Exporting to various formats (PDF, PNG, SVG)

### Version Control

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-17 | Initial release for Phase 07 |

## References

### Related Documentation

- [[WIRING_GUIDE]] - Physical wiring and assembly instructions
- [[BOM]] - Bill of materials with part numbers and suppliers
- [[DISPLAY_SPECS]] - E-paper display technical specifications
- [[BUTTON_LAYOUT]] - Button assignments and physical layout
- [[POWER_SUPPLY]] - Power supply and battery monitoring details
- [[BUILD_GUIDE]] - Complete step-by-step build instructions

### External Resources

**Raspberry Pi:**
- [Raspberry Pi Zero W Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [BCM2835 ARM Peripherals](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
- [GPIO Electrical Specifications](https://www.raspberrypi.com/documentation/computers/config_txt.html)

**Display:**
- [Waveshare 4.2" E-Paper Wiki](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module)
- [UC8176 Controller Datasheet](https://cursedhardware.github.io/epd-driver-ic/UC8176c.pdf)

**Components:**
- [INA219 Datasheet](https://www.ti.com/lit/ds/symlink/ina219.pdf) - TI Power Monitor
- [Omron B3F Switch Datasheet](https://omronfs.omron.com/en_US/ecb/products/pdf/en-b3f.pdf)

**Standards:**
- [SPI Bus Specification](https://www.nxp.com/docs/en/data-sheet/SPI.pdf)
- [I2C Bus Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)

## License

This schematic documentation is part of the Open Source E-Reader project.

**Hardware Schematics:** CERN Open Hardware License v2 - Permissive (CERN-OHL-P-2.0)
**Documentation:** Creative Commons Attribution-ShareAlike 4.0 (CC BY-SA 4.0)

See LICENSE file for complete terms.

---

**Document Version:** 1.0
**Last Updated:** 2026-01-17
**Hardware Revision:** Phase 07 Release
**Maintainer:** Open Source E-Reader Project
