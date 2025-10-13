# Final Bill of Materials (BOM)

Updated component list incorporating all design decisions.

## Build Configuration: MVP with Standard Display

This BOM reflects the decided configuration:
- ✅ 6" Standard resolution e-ink (800x600, 167 PPI)
- ✅ 8 tactile buttons for navigation and page turning
- ✅ On-screen keyboard for text entry
- ⏸️ No Bluetooth (can add later)
- ⏸️ No WiFi (can add later)

**Target**: Trade paperback size, excellent novel reading, under $60

---

## Core Components

| Component | Specification | Part Number | Qty | Unit $ | Total $ | Supplier |
|-----------|--------------|-------------|-----|--------|---------|----------|
| **Display** | 6" E-Ink 800x600 167PPI | GDEW060T7 or compatible | 1 | $30.00 | $30.00 | Good Display, Waveshare |
| **Processor Module** | Allwinner H3 + 512MB RAM | Orange Pi Zero LTS | 1 | $18.00 | $18.00 | Orange Pi store |
| **Storage** | microSD card slot | Molex 5031821852 | 1 | $0.75 | $0.75 | Digikey, Mouser |

**Core Subtotal: $48.75**

---

## Power System

| Component | Specification | Part Number | Qty | Unit $ | Total $ | Supplier |
|-----------|--------------|-------------|-----|--------|---------|----------|
| **Battery Holder** | 18650 x2 holder | Keystone 1042P | 1 | $1.50 | $1.50 | Digikey |
| **Battery Cells** | 18650 2500mAh | LG INR18650 MJ1 | 2 | $3.00 | $6.00 | Battery supplier |
| **Charge Controller** | Li-Ion charger w/protection | TP4056 module | 1 | $1.00 | $1.00 | AliExpress, Amazon |
| **USB Connector** | Micro USB receptacle | Molex 1050170001 | 1 | $0.30 | $0.30 | Digikey, Mouser |
| **Power Switch** | Slide switch SPDT | C&K JS202011SCQN | 1 | $0.50 | $0.50 | Digikey |

**Power Subtotal: $9.30**

---

## Input System

| Component | Specification | Part Number | Qty | Unit $ | Total $ | Supplier |
|-----------|--------------|-------------|-----|--------|---------|----------|
| **Page Buttons** | 6x6mm tactile, 1M cycles | Omron B3F-4055 | 2 | $0.25 | $0.50 | Digikey, Mouser |
| **Nav Buttons** | 6x6mm tactile, 100K cycles | Omron B3F-4005 | 4 | $0.15 | $0.60 | Digikey, Mouser |
| **Function Buttons** | 6x6mm tactile | Omron B3F-4005 | 2 | $0.15 | $0.30 | Digikey, Mouser |
| **Button Caps** | 3D printed caps | Custom STL | 8 | $0.05 | $0.40 | Self-printed |

**Input Subtotal: $1.80**

---

## PCB and Passives

| Component | Specification | Part Number | Qty | Unit $ | Total $ | Supplier |
|-----------|--------------|-------------|-----|--------|---------|----------|
| **PCB** | 2-layer, 120x170mm | Custom design | 1 | $2.00 | $2.00 | JLCPCB, PCBWay |
| **Resistors** | Various 0805 SMD | Assorted | ~20 | - | $0.50 | Digikey kit |
| **Capacitors** | Various 0805 SMD | Assorted | ~15 | - | $1.00 | Digikey kit |
| **Diodes** | Schottky, SOD-123 | 1N5819 | 2 | $0.10 | $0.20 | Digikey |
| **LED** | Status indicator | Generic 0805 | 1 | $0.05 | $0.05 | Digikey |

**PCB/Passives Subtotal: $3.75**

---

## Enclosure and Mechanical

| Component | Specification | Part Number | Qty | Unit $ | Total $ | Supplier |
|-----------|--------------|-------------|-----|--------|---------|----------|
| **Case** | 3D printed front/back | Custom STL | 1 | $4.00 | $4.00 | Self-printed |
| **Screws** | M2.5 x 8mm | Generic | 6 | $0.05 | $0.30 | Hardware store |
| **Standoffs** | M2.5 x 5mm | Generic | 4 | $0.10 | $0.40 | Digikey |

**Mechanical Subtotal: $4.70**

---

## Consumables and Misc

| Component | Specification | Qty | Unit $ | Total $ | Notes |
|-----------|--------------|-----|--------|---------|-------|
| **microSD Card** | 16GB Class 10 | 1 | $5.00 | $5.00 | OS + books storage |
| **Wire** | 24-26 AWG | 1m | $1.00 | $1.00 | Connections |
| **Solder** | Lead-free or leaded | - | - | $0.50 | If building yourself |
| **Adhesive** | Double-sided tape | - | - | $0.50 | Mounting components |

**Misc Subtotal: $7.00**

---

## TOTAL BOM COST: $75.30

### Cost Reduction Options

To reach $50-60 target:

#### Option 1: Use Smaller Display
- Replace 6" ($30) with 5.83" ($25)
- **New total**: $70.30

#### Option 2: Use Single 18650 Cell
- Replace 2x18650 ($6) with 1x18650 ($3)
- Reduces battery life to ~1.5 weeks
- **New total**: $72.30

#### Option 3: Design Custom H3 Board (More Complex)
- Replace OrangePi module ($18) with:
  - H3 chip: $4
  - 512MB DDR3: $5
  - AXP209 PMIC: $1
  - Subtotal: $10
- Requires 4-layer PCB (+$3) and more complex assembly
- **Savings**: $5
- **New total**: $70.30

#### Option 4: Combination Approach
- 5.83" display: Save $5
- Single 18650: Save $3
- Custom H3 board: Save $5
- **New total**: $62.30 ✅ **Under $65!**

#### Option 5: Bulk Pricing (10+ units)
- Display: $25 (save $5)
- Orange Pi: $15 (save $3)
- PCB: $1 (save $1)
- Passives: Bulk discount ~$2
- **Savings**: $11
- **New total**: $64.30

### Recommended for MVP

**Single Unit Build**: $75 (accept higher cost for learning/testing)
**Small Batch (5-10)**: $65 (some bulk discounts)
**Production (100+)**: $50-55 (significant bulk discounts)

---

## Optional Upgrades (Not in Base BOM)

### Bluetooth + WiFi
| Component | Specification | Qty | Unit $ | Total $ |
|-----------|--------------|-----|--------|---------|
| ESP32-WROOM-32 | WiFi + BT module | 1 | $2.50 | $2.50 |
| Antenna | Chip or PCB trace | 1 | $0.50 | $0.50 |
| **Wireless Total** | | | | **$3.00** |

### Frontlight (Night Reading)
| Component | Specification | Qty | Unit $ | Total $ |
|-----------|--------------|-----|--------|---------|
| LED Strip | Flexible white LED | 1 | $2.00 | $2.00 |
| Light Guide | Acrylic sheet | 1 | $3.00 | $3.00 |
| PWM Driver | For brightness control | 1 | $0.50 | $0.50 |
| **Frontlight Total** | | | | **$5.50** |

### Audio (Audiobooks/TTS)
| Component | Specification | Qty | Unit $ | Total $ |
|-----------|--------------|-----|--------|---------|
| Audio Codec | I2S DAC | 1 | $1.50 | $1.50 |
| 3.5mm Jack | Headphone connector | 1 | $0.40 | $0.40 |
| Speaker | Small 8Ω 0.5W | 1 | $1.00 | $1.00 |
| **Audio Total** | | | | **$2.90** |

---

## Purchasing Guide

### Minimum Order Quantities (MOQ)

Most components available in single quantities except:

| Component | Typical MOQ | Strategy |
|-----------|-------------|----------|
| PCBs | 5 boards minimum | Order 5, use best one, keep spares |
| Buttons | Often sold in packs of 10 | Buy assortment kit |
| Passives | Often 10-100 pieces | Buy resistor/capacitor kits |
| Display | Usually 1 piece OK | Can order single units |

### Recommended Suppliers by Region

#### North America
- **Electronics**: Digikey, Mouser (fast shipping, reliable)
- **PCBs**: JLCPCB, OSH Park
- **Batteries**: IMR Batteries, 18650 Battery Store
- **Displays**: Waveshare via Amazon (returnable)

#### Europe
- **Electronics**: Digikey, Mouser, RS Components
- **PCBs**: JLCPCB, PCBWay
- **Displays**: Waveshare EU store

#### Global / Budget Option
- **All components**: AliExpress, LCSC
- **Trade-off**: 2-4 week shipping, less support
- **Savings**: 20-40% cheaper

### Sample Starter Kit Order

**For prototyping (recommended first purchase)**:

**From Amazon (fast delivery)**:
- Orange Pi Zero LTS: $18-22
- Waveshare 5.83" e-paper: $25-30
- 18650 cells (reputable brand): $8-10
- TP4056 charging modules (5-pack): $8
- Tactile button assortment: $10
- Subtotal: ~$75

**From JLCPCB**:
- PCB design (5 boards): $2 + $15 shipping = $17

**From Digikey** (one order for passives):
- Resistor kit
- Capacitor kit
- Connectors
- Subtotal: ~$25

**Total starter investment**: ~$120 (includes spares and test components)

### Building Just One MVP

If building single prototype:
1. Buy Orange Pi Zero + 5.83" display to test software: $48
2. Breadboard power circuit with TP4056 module: $10
3. Test buttons on breadboard: $5
4. Once software works, design final PCB: $17
5. Order correct display size: $30
6. Final assembly

**Staged approach total**: ~$110 but validates design before committing

---

## BOM Versions Summary

| Version | Display | Processor | Features | Total Cost |
|---------|---------|-----------|----------|------------|
| **MVP v1** (this BOM) | 6" standard | OrangePi module | Basic reading | $75 |
| **Budget v1** | 5.83" | OrangePi module | Smaller, cheaper | $70 |
| **Production v1** | 6" standard | Custom H3 board | Cost optimized | $65 |
| **Premium v1** | 6" high-res | OrangePi module | Best quality | $95 |
| **Full-Featured v2** | 6" high-res | Custom H3 board | +WiFi+BT+Light | $110 |

---

## Assembly Cost Estimate

### DIY Assembly (Your Time)
- **Soldering time**: 2-4 hours
- **Case prep**: 1 hour
- **Assembly**: 1 hour
- **Testing/debugging**: 2-4 hours
- **Total time**: 6-10 hours
- **Cost**: $0 (learning experience!)

### Professional Assembly (Future)
- **PCB assembly (PCBA)**: $10-15 per board (50+ qty)
- **Final assembly**: $5-10 per unit
- **Testing**: $2-3 per unit
- **Total labor**: $17-28 per unit

---

## Long-Term Sourcing Strategy

### Phase 1: MVP (1-5 units)
- Buy everything from Amazon/Digikey
- Fast shipping, easy returns
- Accept higher cost
- Focus on learning

### Phase 2: Small Batch (10-50 units)
- Mix of Digikey (critical parts) and LCSC (passives)
- PCB assembly by JLCPCB
- Some bulk discounts kick in
- Target: $60-65 per unit

### Phase 3: Production (100+ units)
- Direct from manufacturers (Good Display, etc.)
- Full PCBA service
- Negotiate bulk pricing
- Custom packaging
- Target: $45-50 per unit

---

## Next Steps

1. **Order initial test kit**: Orange Pi Zero + small display (~$50)
2. **Test software stack**: Get Linux and display working
3. **Design PCB**: Create schematic and layout (KiCad)
4. **Order PCB prototype**: First real board (~$20)
5. **Full assembly**: Build complete device
6. **Test and iterate**: Find issues, improve design
7. **Order final components**: With lessons learned

## Files and Resources

**BOM spreadsheet**: `hardware/bom/mvp_v1.csv` (coming soon)
**KiCad project**: `hardware/pcb/ereader_mvp/` (coming soon)
**3D models**: `hardware/case/` (coming soon)
**Sourcing links**: `hardware/bom/supplier_links.md` (coming soon)

---

**Last Updated**: 2024-01-XX
**BOM Version**: MVP v1.0
**Target Price**: $75 (single unit), $65 (batch of 10), $55 (batch of 100)
