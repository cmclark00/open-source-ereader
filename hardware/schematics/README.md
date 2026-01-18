# Hardware Schematics

This directory contains schematic diagrams and design files for the Open Source E-Reader project.

## Contents

- **complete-schematic.txt** - Full ASCII schematic showing all connections
- **display-schematic.txt** - E-paper display SPI interface schematic
- **button-schematic.txt** - GPIO button input schematic
- **power-schematic.txt** - Power supply and battery management schematic
- **ereader-fritzing.fzz** - Fritzing project file (to be created)
- **ereader-kicad/** - KiCad project directory (to be created)

## Schematic Overview

The e-reader hardware consists of four main subsystems:

1. **Display Interface** - Waveshare 4.2" e-paper display via SPI
2. **Button Input** - 5 GPIO buttons for navigation
3. **Power Supply** - 5V input with optional battery monitoring
4. **Controller** - Raspberry Pi Zero W

## Creating PCB Schematics

### Using Fritzing (Recommended for Beginners)

Fritzing is a free, open-source tool for creating circuit diagrams and PCB layouts.

**Installation:**
- Download from: https://fritzing.org/
- Windows/Mac/Linux versions available

**Steps to Create Schematic:**

1. Open Fritzing
2. Create New Project: File → New
3. Add Components:
   - Search for "Raspberry Pi Zero" in Parts panel
   - Search for "Waveshare E-Paper" or use generic SPI device
   - Add 5x "Tactile Switch" for buttons
   - Add power supply components

4. Wire Components:
   - Use Breadboard View for initial layout
   - Switch to Schematic View for formal diagram
   - Connect according to pin mappings in SCHEMATIC.md

5. Export:
   - File → Export → As Image → PNG (for documentation)
   - File → Export → As Image → PDF (for printing)
   - File → Export → As Image → SVG (for vector graphics)

6. Save:
   - Save as `ereader-fritzing.fzz` in this directory

**Fritzing Parts:**
- Raspberry Pi Zero W: Search "Raspberry Pi Zero"
- E-Paper Display: Use "Adafruit 2.13 E-Ink" as template, modify pins
- Buttons: "Tactile Switch" part
- Wires: Breadboard wires or schematic connections

### Using KiCad (Recommended for Production)

KiCad is professional-grade, free EDA software used for PCB design.

**Installation:**
- Download from: https://www.kicad.org/
- Windows/Mac/Linux versions available

**Steps to Create Schematic:**

1. Create New Project: File → New → Project
2. Open Schematic Editor: Click "Schematic Editor" icon
3. Add Symbols:
   - Press 'A' to add symbol
   - Search for "Raspberry_Pi_Zero" (or create custom symbol)
   - Add connector symbols for display (8-pin header)
   - Add 5x "SW_Push" symbols for buttons
   - Add power symbols: VCC (3.3V), GND, +5V

4. Place Components and Wire:
   - Use 'W' to start wiring
   - Add labels with 'L' for signal names (MOSI, SCLK, etc.)
   - Add global labels for power nets

5. Annotate Schematic:
   - Tools → Annotate Schematic (assigns reference designators)

6. Electrical Rules Check:
   - Inspect → Electrical Rules Checker
   - Fix any errors or warnings

7. Generate Netlist:
   - Tools → Generate Netlist (for PCB layout)

8. Export:
   - File → Plot → PDF (for documentation)
   - File → Plot → SVG (for web viewing)

9. Save:
   - Save project in `hardware/schematics/ereader-kicad/`

**KiCad Symbol Libraries:**
- Raspberry Pi: "MCU_RaspberryPi" library
- Switches: "Switch" library
- Connectors: "Connector_Generic" library

## Pin Mapping Reference

### Display SPI Interface

| Display Pin | Pi Zero GPIO | Physical Pin |
|-------------|--------------|--------------|
| VCC         | 3.3V         | Pin 1        |
| GND         | GND          | Pin 6        |
| DIN (MOSI)  | GPIO 10      | Pin 19       |
| CLK (SCLK)  | GPIO 11      | Pin 23       |
| CS          | GPIO 8       | Pin 24       |
| DC          | GPIO 25      | Pin 22       |
| RST         | GPIO 17      | Pin 11       |
| BUSY        | GPIO 24      | Pin 18       |

### Button Connections

| Button  | GPIO    | Physical Pin | Connection |
|---------|---------|--------------|------------|
| UP      | GPIO 5  | Pin 29       | Active-low |
| DOWN    | GPIO 6  | Pin 31       | Active-low |
| SELECT  | GPIO 13 | Pin 33       | Active-low |
| BACK    | GPIO 19 | Pin 35       | Active-low |
| MENU    | GPIO 26 | Pin 37       | Active-low |
| GND     | GND     | Pin 34/39    | Common     |

## Component Values and Part Numbers

### Display Interface
- **Display:** Waveshare 4.2" E-Paper Module (Rev 2.2)
- **Interface:** SPI Mode 0, max 10MHz
- **Power:** 3.3V, ~26mA during refresh

### Button Input
- **Buttons:** 5x tactile switches (6x6mm or 12x12mm)
  - Omron B3F-1000 or equivalent
  - Actuation force: 160-260gf
  - Life: 100,000 cycles minimum
- **Pull-up resistors:** Internal to Pi Zero W (~50kΩ)
- **Debounce capacitors (optional):** 5x 0.1µF ceramic (50V)

### Power Supply
- **Input:** 5V 2A micro-USB
- **Battery (optional):** 3.7V LiPo, 2000-5000mAh
- **Monitoring (optional):** INA219 I2C power monitor

## Design Notes

### SPI Bus Configuration
- **SPI Device:** `/dev/spidev0.0` (CE0)
- **Clock Speed:** 4MHz typical, 10MHz maximum
- **Mode:** SPI Mode 0 (CPOL=0, CPHA=0)
- **Bit Order:** MSB first

### GPIO Configuration
- **Buttons:** Active-low with internal pull-up resistors
- **Display Control:** Standard push-pull outputs (DC, RST)
- **Display BUSY:** Input with no pull resistor (driven by display)

### Power Distribution
- **Pi Zero W Power:** 5V via micro-USB (150-200mA typical)
- **Display Power:** 3.3V from Pi GPIO (26mA peak during refresh)
- **Total Power:** <300mA typical, <500mA peak

### ESD Protection
- Consider adding TVS diodes on SPI lines for production
- ESD protection on button inputs recommended for production
- Not required for prototype/development builds

## Schematic Best Practices

1. **Clear Labeling:** Label all nets with descriptive names (not just GPIO numbers)
2. **Power Symbols:** Use standard VCC/GND symbols, not just wires
3. **Reference Designators:**
   - U1 = Raspberry Pi Zero W
   - J1 = Display connector
   - SW1-SW5 = Buttons (UP, DOWN, SELECT, BACK, MENU)
   - C1-C5 = Optional debounce capacitors
4. **Title Block:** Include project name, revision, date, author
5. **Notes:** Add design notes and specifications to schematic

## Export Formats

For documentation, export schematics in multiple formats:

- **PDF:** High-quality printing and archival
- **PNG:** Web viewing and README inclusion (300 DPI minimum)
- **SVG:** Vector format for scaling
- **JPEG:** Thumbnail previews only (avoid for technical diagrams)

## Related Documentation

- [SCHEMATIC.md](../../docs/hardware/SCHEMATIC.md) - Detailed schematic documentation
- [WIRING_GUIDE.md](../../docs/hardware/WIRING_GUIDE.md) - Physical wiring instructions
- [BOM.md](../../docs/hardware/BOM.md) - Bill of materials with part numbers
- [DISPLAY_SPECS.md](../../docs/hardware/DISPLAY_SPECS.md) - Display technical specs
- [BUTTON_LAYOUT.md](../../docs/hardware/BUTTON_LAYOUT.md) - Button GPIO assignments

## Contributing

If you create improved schematics using Fritzing or KiCad:

1. Ensure all components are properly labeled
2. Run electrical rules check (KiCad) or DRC (Fritzing)
3. Export to PDF and PNG formats
4. Submit pull request with source files and exports
5. Update this README with any new information

## License

Hardware schematics are released under the CERN Open Hardware License (OHL).
See LICENSE file for details.

---

**Last Updated:** 2026-01-17
**Schematic Version:** 1.0
**Hardware Revision:** Phase 07 Release
