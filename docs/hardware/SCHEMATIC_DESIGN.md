# PCB Schematic Design

Complete schematic design for the e-reader main board.

## Design Overview

**Board type**: Single PCB design
**Layers**: 2-layer (MVP), 4-layer (production)
**Size**: 120mm x 170mm (fits inside device)
**Processor**: Orange Pi Zero LTS module (H3 + RAM + power)
**Cost**: ~$2 for 5 boards (JLCPCB)

## System Block Diagram

```
                    ┌─────────────────────────────┐
                    │   ORANGE PI ZERO MODULE     │
                    │  (H3 SoC + 512MB RAM)       │
                    │                             │
                    │  [SD Card Slot] ────────────┼─── microSD Card (OS)
                    │                             │
                    │  GPIO Pins:                 │
                    │   - SPI0 ──────────────────┼─→ E-Ink Display
                    │   - GPIO for buttons        │
                    │   - UART (debug)            │
                    │   - I2C (future sensors)    │
                    │   - USB OTG                 │
                    └──────────────┬──────────────┘
                                   │ 5V Power
                                   │
                    ┌──────────────▼──────────────┐
                    │   POWER MANAGEMENT          │
                    ├─────────────────────────────┤
   [USB] ──→ │ TP4056 Charging IC            │
             │  + Protection                  │
             │                                │
[Battery] ◄─→│ 2x 18650 Cells (7.4V)         │
             │  5000-6000mAh                  │
             │                                │
             │ DC-DC Boost/Buck to 5V         │
             │  (for H3 module)               │
             └────────────────────────────────┘
                           │
                           │ 5V / 3.3V
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
        ▼                                     ▼
   ┌────────┐                          ┌──────────┐
   │ DISPLAY│                          │ BUTTONS  │
   │        │                          │  x 8     │
   │ 6" SPI │                          │          │
   │ E-Ink  │                          │ To GPIO  │
   └────────┘                          └──────────┘
```

## Detailed Schematics

### 1. Power Supply Circuit

```
USB Micro Connector (J1)
  │
  │ VBUS (5V)
  ├───────────┬─────────────────┐
  │           │                 │
  │        [D1]                 │
  │       Schottky              │
  │       1N5819             [C1]
  │          │               100uF
  │          │                 │
  │          ▼                GND
  │     TP4056 Charging IC
  │     ┌─────────────┐
  │     │1 TEMP    8 VCC├──┐
  │     │             │    │
  │     │2 PROG    7 BAT├──┼─→ To Battery Pack
  │     │             │    │
  │     │3 GND     6 STDBY│
  │     │             │
  │     │4 GND     5 CHRG│
  │     └─────────────┘
  │          │ │
  │         [R1] [R2]
  │         1.2K  1K
  │          │    │
  └──────────┴────┴─────── GND

Battery Pack (2S Configuration):
  │
  [18650 Cell 1] ─┬─ 3.7V
  [18650 Cell 2] ─┴─ 3.7V
                   │
              Total: 7.4V
                   │
              Protection IC
              (DW01A + FS8205A)
                   │
             ┌─────┴─────┐
             │           │
       BAT+  │           │  BAT-
             │           │
         To TP4056       GND
             │
             ▼
      MT3608 Boost Module
      (7.4V → 5V @ 2A)
             │
             ▼
        To Orange Pi 5V Pin
```

#### Component Values:

| Ref | Part | Value | Package | Notes |
|-----|------|-------|---------|-------|
| J1 | USB Micro | - | SMD | Charging port |
| D1 | Schottky diode | 1N5819 | SOD-123 | Reverse protection |
| C1 | Capacitor | 100µF | 0805 | Input filter |
| U1 | Charge IC | TP4056 | SOP-8 | 1A charge current |
| R1 | Resistor | 1.2kΩ | 0805 | Charge current set |
| R2 | Resistor | 1kΩ | 0805 | Status LED |
| BT1 | Battery holder | 2x 18650 | THT | Keystone 1042P |
| U2 | Boost converter | MT3608 | Module | 7.4V to 5V |

### 2. Orange Pi Zero Module Connection

```
Orange Pi Zero LTS (26-pin header):

   Pin 1  [3.3V] ────────────────────────┐
   Pin 2  [5V]   ────────────────────────┼──→ From power supply
   Pin 3  [GPIO12/I2C_SDA] ──────────────┼──→ Future I2C devices
   Pin 4  [5V]   ────────────────────────┘
   Pin 5  [GPIO11/I2C_SCL] ──────────────────→ Future I2C devices
   Pin 6  [GND]  ─────────────────────────┬─→ Ground plane
   Pin 7  [GPIO6] ────────────────────────┤
   Pin 8  [GPIO13/UART_TX] ───────────────┤──→ Debug serial
   Pin 9  [GND]  ─────────────────────────┤
   Pin 10 [GPIO14/UART_RX] ───────────────┤──→ Debug serial
   Pin 11 [GPIO1] ────────────────────────┤
   Pin 12 [GPIO7] ────────────────────────┤──→ Button inputs
   Pin 13 [GPIO0] ────────────────────────┤     (8 total)
   Pin 14 [GND]  ─────────────────────────┤
   Pin 15 [GPIO3] ────────────────────────┤
   Pin 16 [GPIO19/SPI_CS] ────────────────┤──→ E-Ink CS
   Pin 17 [3.3V] ─────────────────────────┤──→ 3.3V rail
   Pin 18 [GPIO18] ───────────────────────┤
   Pin 19 [GPIO15/SPI_MOSI] ──────────────┤──→ E-Ink data
   Pin 20 [GND]  ─────────────────────────┤
   Pin 21 [GPIO16/SPI_MISO] ──────────────┤──→ E-Ink (optional)
   Pin 22 [GPIO2] ────────────────────────┤
   Pin 23 [GPIO14/SPI_CLK] ───────────────┤──→ E-Ink clock
   Pin 24 [GPIO10/SPI_CS] ────────────────┤
   Pin 25 [GND]  ─────────────────────────┤
   Pin 26 [GPIO21] ───────────────────────┘
```

### 3. E-Ink Display Interface (SPI)

```
Orange Pi GPIO → E-Ink Display

         Orange Pi              Display Module
                                (24-pin FPC)
   SPI_MOSI (Pin 19) ────────→ DIN (Data In)
   SPI_CLK  (Pin 23) ────────→ CLK (Clock)
   SPI_CS   (Pin 16) ────────→ CS  (Chip Select)
   GPIO2    (Pin 22) ────────→ DC  (Data/Command)
   GPIO3    (Pin 15) ────────→ RST (Reset)
   GPIO6    (Pin 7)  ────────→ BUSY (Busy signal)
   3.3V     (Pin 17) ────────→ VCC (3.3V power)
   GND      (Pin 6)  ────────→ GND (Ground)

Connection details:

       Orange Pi SPI Header
            (Male pins)
                │
                ├───── 30cm ribbon cable
                │      (8 conductors)
                │
                ▼
       FPC Connector on PCB
       (Female, 24-pin, 0.5mm pitch)
                │
                └───── E-Ink display FPC cable
                       (from display module)

Note: Display may require level shifting if it's 5V
Most modern e-inks are 3.3V compatible.
```

#### Display Connector Pinout:

| Pin | Signal | Direction | Description |
|-----|--------|-----------|-------------|
| 1 | VCC | Input | 3.3V power |
| 2 | GND | - | Ground |
| 3 | BUSY | Output | Display busy signal |
| 4 | RST | Input | Reset (active low) |
| 5 | DC | Input | Data/Command select |
| 6 | CS | Input | SPI chip select |
| 7 | CLK | Input | SPI clock |
| 8 | DIN | Input | SPI data in |

### 4. Button Input Circuit

```
All 8 buttons use identical circuit:

   3.3V
    │
    ├───[R] 10kΩ (Pull-up)
    │
    ├────────────→ To GPIO pin (Orange Pi)
    │
   [SW] Button (N.O.)
    │
   GND

When button pressed: GPIO reads LOW (0V)
When button released: GPIO reads HIGH (3.3V)

Button to GPIO mapping:

Button Function     GPIO Pin    Physical Pin
────────────────────────────────────────────
Page Forward        GPIO1       Pin 11
Page Back           GPIO7       Pin 12
Nav Up              GPIO0       Pin 13
Nav Down            GPIO18      Pin 18
Nav Left            GPIO2       Pin 22
Nav Right (Select)  GPIO3       Pin 15
Home                GPIO21      Pin 26
Menu                GPIO10      Pin 24
Power               GPIO6       Pin 7 (or to AXP209 if added)
```

#### Button debouncing:

```c
// Software debounce (simple)
#define DEBOUNCE_MS 50

bool read_button_debounced(int gpio) {
    if (gpio_read(gpio) == LOW) {
        usleep(DEBOUNCE_MS * 1000);
        if (gpio_read(gpio) == LOW) {
            return PRESSED;
        }
    }
    return NOT_PRESSED;
}
```

Hardware debounce option (add to each button):
```
   GPIO ──────┬──────→ To Orange Pi
              │
            [C] 100nF
              │
             GND
```

### 5. Debug Serial Console

```
UART Header (for debugging):

   J2 (3-pin header):

   Pin 1: GND ────────→ GND on USB-serial adapter
   Pin 2: TX  ────────→ RX on USB-serial adapter
   Pin 3: RX  ────────→ TX on USB-serial adapter

   Settings: 115200 baud, 8N1

   Connect with: screen /dev/ttyUSB0 115200
```

### 6. Status LEDs (Optional)

```
Charging Status LED:

   TP4056 CHRG Pin ──┬──[R]──[LED]──GND
                     │      330Ω   Red
                     │
   TP4056 STDBY Pin ─┴──[R]──[LED]──GND
                           330Ω   Green

Power Status LED:

   3.3V ──[R]──[LED]──GND
          1kΩ   Blue
```

### 7. Optional: ESP32 for Bluetooth (Footprint)

```
ESP32-WROOM-32 Module Interface:

       Orange Pi              ESP32
                         ┌────────────┐
   3.3V (Pin 1)  ───────►│VCC         │
   GND (Pin 6)   ───────►│GND         │
   UART_TX (Pin 8) ─────►│RX  ESP32   │
   UART_RX (Pin 10) ◄────│TX  Module  │
   GPIO4         ───────►│EN          │
   GPIO5         ───────►│IO0 (prog)  │
                         └────────────┘

Leave unpopulated for MVP.
Add later for Bluetooth/WiFi features.
```

## PCB Layout Guidelines

### Layer Stack (2-layer):

```
TOP (Component side):
- Orange Pi module (large, centered)
- Button switches (right side)
- TP4056 charging IC
- USB connector (bottom edge)
- Status LEDs
- Most SMD components

BOTTOM (Solder side):
- Battery holder (large, centered)
- Power conversion (MT3608 module)
- Ground pour
- Debug header
```

### Trace Width Guidelines:

| Signal Type | Current | Width | Notes |
|------------|---------|-------|-------|
| Power (5V) | 2A | 1.5mm | From battery to module |
| Power (3.3V) | 500mA | 0.8mm | To display, buttons |
| Battery | 3A | 2.0mm | High current path |
| SPI signals | <10mA | 0.3mm | Keep short, < 10cm |
| GPIO | <10mA | 0.3mm | General purpose |
| Ground | - | Pour | Fill all empty space |

### Critical Design Rules:

1. **SPI traces** (to display):
   - Keep as short as possible (< 10cm ideal)
   - Route together, parallel
   - Add ground plane underneath
   - Consider series resistors (33Ω) to reduce ringing

2. **Power routing**:
   - Star ground from battery
   - Wide traces for high current
   - Add bulk capacitors (100µF) near Orange Pi
   - Separate analog/digital grounds (if possible)

3. **Button placement**:
   - Align with case design
   - Allow for button cap clearance
   - Keep pull-up resistors close to GPIOs

4. **Mounting holes**:
   - M2.5 holes at 4 corners
   - Connect to ground
   - Minimum 3mm from board edge

### PCB Dimensions and Cutouts:

```
Top view PCB outline:

    0,0 ────────────────────────── 120mm
     │                              │
     │  ○ M2.5 mounting hole       │
     │                              │
     │    ┌──────────────┐         │
     │    │  Orange Pi   │         │
     │    │  Zero Module │         │
30mm │    │  (soldered)  │         │
     │    └──────────────┘         │
     │                              │
     │                        [SW]  │ ← Button locations
     │                        [SW]  │
     │                        [SW]  │
     │                        [SW]  │
     │                              │
     │  ┌─────────────────┐        │
     │  │  Battery        │        │
     │  │  Holder         │        │
     │  │  (underneath)   │        │
     │  └─────────────────┘        │
     │                              │
     │  [USB] ← Connector          │
     │                              │
     │  ○ M2.5 mounting hole       │
     │                              │
    170mm ──────────────────────────┘
```

## BOM for PCB Components

| Ref | Component | Value | Package | Qty | Cost |
|-----|-----------|-------|---------|-----|------|
| J1 | USB Micro | - | SMD | 1 | $0.30 |
| J2 | Pin header | 3-pin | 2.54mm | 1 | $0.10 |
| J3 | FPC connector | 24-pin | 0.5mm | 1 | $0.80 |
| U1 | TP4056 | - | SOP-8 | 1 | $0.30 |
| U2 | Orange Pi Zero LTS | - | Module | 1 | $18.00 |
| U3 | MT3608 boost | - | Module | 1 | $0.80 |
| SW1-8 | Tactile switch | - | 6x6mm | 8 | $1.60 |
| BT1 | Battery holder | 2x18650 | THT | 1 | $1.50 |
| D1 | Schottky diode | 1N5819 | SOD-123 | 1 | $0.10 |
| R1-8 | Resistor | 10kΩ | 0805 | 8 | $0.20 |
| R9 | Resistor | 1.2kΩ | 0805 | 1 | $0.05 |
| R10-12 | Resistor | 330Ω | 0805 | 3 | $0.10 |
| C1-4 | Capacitor | 100µF | 0805 | 4 | $0.80 |
| C5-8 | Capacitor | 100nF | 0805 | 4 | $0.20 |
| LED1-3 | LED | Various | 0805 | 3 | $0.15 |
| | PCB | 2-layer | Custom | 1 | $2.00 |
| **TOTAL** | | | | | **$26.00** |

## Manufacturing Files Checklist

When ordering PCB, generate:

- [ ] Gerber files (RS-274X format)
  - Top copper layer
  - Bottom copper layer
  - Top silkscreen
  - Bottom silkscreen
  - Top soldermask
  - Bottom soldermask
  - Drill file
  - Board outline

- [ ] Bill of Materials (BOM.csv)
- [ ] Component placement file (CPL.csv)
- [ ] Assembly drawing (PDF)
- [ ] Schematic (PDF)

## Testing Procedure

### Power-on test:
1. Connect battery (no module installed)
2. Measure voltage at 5V rail: Should be 4.8-5.2V
3. Check for shorts between power and ground
4. Measure current draw: Should be <5mA (no load)

### Module test:
5. Install Orange Pi module
6. Connect serial console
7. Apply power
8. Watch for boot messages (should see U-Boot)

### Display test:
9. Connect e-ink display
10. Run test pattern script
11. Verify full refresh works
12. Test partial refresh

### Button test:
13. Run GPIO test script
14. Press each button and verify state change
15. Check for cross-talk between buttons

### Full system test:
16. Boot to Linux
17. Load e-reader application
18. Open test EPUB file
19. Verify page turns work
20. Test battery life (measure current)

## Design Files

**KiCad Project**: `/hardware/pcb/ereader-main/`
**Schematic**: `/hardware/pcb/ereader-main/ereader.kicad_sch`
**PCB Layout**: `/hardware/pcb/ereader-main/ereader.kicad_pcb`
**3D View**: `/hardware/pcb/ereader-main/render.png`

## Revision History

- **v0.1** (Current): Initial design, Orange Pi module-based
- **v0.2** (Future): Add ESP32 footprint for Bluetooth
- **v1.0** (Production): Custom H3 design, 4-layer PCB

## Next Steps

1. Create KiCad schematic from this document
2. Design PCB layout following guidelines
3. Run electrical rule check (ERC)
4. Run design rule check (DRC)
5. Generate 3D render
6. Review with team
7. Order prototype PCB (JLCPCB)
8. Assembly and testing
9. Iterate based on findings

---

**Status**: Schematic complete, ready for PCB layout
**Estimated design time**: 8-12 hours for first PCB layout
**Prototype cost**: $2 for PCB + $26 for components = $28 per board
