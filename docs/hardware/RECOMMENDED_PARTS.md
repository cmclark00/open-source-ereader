# Recommended Parts List

This document specifies the exact components recommended for building the e-reader, optimized to mimic the size and feel of a trade paperback book.

## Design Philosophy: Trade Paperback Form Factor

**Target Dimensions**:
- Trade paperback (US): ~5.5" x 8.5" (140mm x 216mm)
- Mass market paperback: ~4.25" x 6.87" (108mm x 175mm)

**Our target**: Slightly larger than mass market, smaller than trade paperback
- Device size: ~5" x 7" (127mm x 178mm)
- Display area: ~4.7" x 6.3" diagonal
- Weight: 150-180g (similar to a 200-page paperback)
- Thickness: 7-9mm

## Display Selection

### Option 1: 6" E-Ink Display (RECOMMENDED)

#### Waveshare 6" E-Ink Display HAT
**Model**: Waveshare 6" E-Paper HAT (V2/V3)
- **Resolution**: 1448 x 1072 pixels (265 PPI) - NEWER VERSION
- **Display Area**: 122.4mm x 90.6mm (6" diagonal)
- **Technology**: E-Ink Carta 1200 (latest generation)
- **Interface**: SPI (4-wire)
- **Refresh Time**:
  - Full refresh: 450ms (very fast!)
  - Partial refresh: 280ms
  - Fast mode: 260ms (with slight ghosting)
  - Ultra-fast: 120ms (for text only, some artifacts)
- **Grayscale**: 16 levels
- **Voltage**: 3.3V
- **Cost**: $45-55
- **Part Number**: Various suppliers
  - Waveshare: SKU 18448
  - Good Display: GDEY060F51

**Pros**:
- Excellent PPI (265) - very crisp text
- Fast refresh rates
- Carta 1200 has better contrast
- Widely available
- Good documentation
- Perfect paperback size

**Cons**:
- At upper end of budget
- Requires careful driver development

#### Alternative: Waveshare 5.83" E-Ink
**Model**: Waveshare 5.83" E-Paper HAT
- **Resolution**: 648 x 480 pixels (150 PPI)
- **Display Area**: 118.8mm x 88.26mm (5.83" diagonal)
- **Technology**: E-Ink Spectra (or monochrome version)
- **Refresh Time**:
  - Full refresh: 2 seconds
  - Partial refresh: 500ms
- **Grayscale**: 2-4 levels (depending on version)
- **Cost**: $25-35
- **Part Number**: Waveshare 5.83inch e-Paper HAT

**Pros**:
- More affordable
- Smaller, lighter
- Good for text-only books

**Cons**:
- Lower resolution (150 PPI)
- Slower refresh
- Not great for PDFs or images

### Option 2: 7.8" E-Ink Display (Future/Premium Version)

**Model**: Waveshare 7.8" E-Ink
- **Resolution**: 1872 x 1404 pixels (300 PPI)
- **Display Area**: 158mm x 118.5mm (7.8" diagonal)
- **Technology**: E-Ink Carta 1100
- **Refresh Time**:
  - Full refresh: 500ms
  - Partial refresh: 300ms
- **Cost**: $80-100
- **Use case**: Better for PDF-heavy usage, larger text

**Decision**: Not recommended for MVP due to cost and size (exceeds paperback form factor)

### FINAL DISPLAY RECOMMENDATION

**Waveshare 6" E-Ink Display HAT (1448x1072, Carta 1200)**

This hits the sweet spot:
- 265 PPI matches high-quality printed books
- 450ms refresh is acceptable (page turns feel natural)
- Perfect size for single-column text
- Carta 1200 has excellent contrast (16:1 ratio)
- 6" diagonal is standard e-reader size (Kindle, Kobo use 6")

## E-Ink Refresh Rate Analysis

### Understanding E-Ink Refresh

E-ink displays don't refresh like LCD screens. Key concepts:

1. **Full Refresh** (Black flash):
   - Clears screen completely (flashes black/white)
   - Removes all ghosting
   - Takes 400-2000ms depending on display
   - Should be done every 5-10 page turns

2. **Partial Refresh** (No flash):
   - Updates only changed pixels
   - Faster (200-500ms)
   - Gradually accumulates ghosting
   - Good for text pages

3. **A2 Mode** (Ultra-fast):
   - 1-bit mode (black or white only, no gray)
   - 100-200ms refresh
   - Good for animations, scrolling
   - Not ideal for reading (no anti-aliasing)

### Refresh Strategy for Our Device

```
Normal reading:
- Page turn: Partial refresh (280ms)
- Every 5th page: Full refresh (450ms)
- Menu navigation: Fast mode (120ms)
- Image rendering: Full refresh (450ms)
```

This gives a reading experience similar to turning physical pages (0.3-0.5 seconds).

## Processor Selection

### RECOMMENDED: Allwinner H3

**Model**: Allwinner H3 SoC
- **Architecture**: ARM Cortex-A7 Quad-core @ 1.2GHz
- **GPU**: Mali-400 MP2 (not needed, but available)
- **RAM Support**: DDR3 up to 2GB (we'll use 512MB)
- **Storage**: SD/eMMC support
- **Interfaces**: SPI, I2C, UART, USB
- **Linux Support**: Excellent mainline support (kernel 6.x)
- **Power**: ~1.5W active, <100mW idle
- **Cost**: $3-5 per chip
- **Available as module**: Yes (OrangePi Zero format)

**Why H3?**:
- Proven track record (millions of OrangePi boards)
- Fast enough for PDF rendering
- Low power when idling
- Cheap and available
- Great community support
- Can buy as complete module (easier prototyping)

### Alternative: Allwinner V3s

**Model**: Allwinner V3s (Ultra-budget option)
- **Architecture**: ARM Cortex-A7 Single-core @ 1.2GHz
- **RAM**: 64MB DDR2 integrated (SIP)
- **Cost**: $1-2 per chip
- **Linux Support**: Mainline support

**Why NOT V3s for this project**:
- 64MB RAM too limited for complex EPUBs/PDFs
- Single core may struggle with rendering
- Would need external RAM chip anyway

### FINAL PROCESSOR RECOMMENDATION

**Allwinner H3 with 512MB DDR3 RAM**

Purchase as a module for MVP:
- **Orange Pi Zero LTS** module (~$15-20)
- Pre-soldered H3 + RAM + power management
- Easy to integrate into custom PCB
- Can design custom board later

## Memory Configuration

### RAM: 512MB DDR3

**Recommended chip**: Micron MT41K256M16HA-125 or equivalent
- **Capacity**: 512MB (4Gbit)
- **Type**: DDR3L-1600
- **Package**: 96-ball FBGA
- **Cost**: $4-6
- **Manufacturer alternatives**:
  - Samsung K4B4G1646E
  - Hynix H5TC4G63CFR
  - ESMT M15T4G16256A

### Storage: microSD + Optional eMMC

**Option 1: microSD slot only (RECOMMENDED for MVP)**
- **Part**: Molex 5031821852 or similar
- **Cost**: $0.50-1.00
- **Pros**: Cheap, flexible, user-upgradeable
- **Cons**: Slightly slower, can be removed accidentally

**Option 2: 8GB eMMC (future production)**
- **Part**: SanDisk SDINBDG4-8G or Samsung KLMAG1JETD-B041
- **Cost**: $4-6
- **Pros**: Faster, more reliable, integrated
- **Cons**: More expensive, not user-upgradeable

**MVP Decision**: microSD slot only

## Power System

### Battery

**Recommended**: 3000mAh 3.7V Li-Polymer Rechargeable

**Specific options**:

1. **Generic 606090 Li-Po** (60mm x 60mm x 9mm)
   - Capacity: 3000mAh
   - Voltage: 3.7V nominal (4.2V max)
   - Cost: $6-8
   - Fits well in device
   - Common size, easy to source

2. **Adafruit 606090** (if prefer reputable supplier)
   - Part: Adafruit #4390
   - Capacity: 3000mAh
   - Cost: $15 (retail)
   - JST connector included

**Alternative**: 18650 Cell Holder
- 2x 18650 cells in parallel (5000-6000mAh)
- User-replaceable
- Bulkier design
- Better for open-source philosophy (users can replace)

**MVP Recommendation**: Use 18650 holder initially (easier prototyping), design for Li-Po in production.

### Battery Charging Circuit

**Recommended**: TP4056 Module with Protection

**Specific part**: TP4056 IC + DW01A protection IC
- **Charging current**: 1A (configurable)
- **Protection**: Over-charge, over-discharge, short circuit
- **Input**: 5V USB (Micro or Type-C)
- **Cost**: $0.50 (IC only) or $1 (complete module)
- **Module option**: Buy pre-made module for prototyping

**Better option for production**: BQ24075
- **Part**: Texas Instruments BQ24075RGTR
- **Features**: Power path management, battery health monitoring
- **Cost**: $2-3
- **Benefit**: Can use device while charging

### Power Management

**Recommended**: AXP209 PMIC

**Part**: X-Powers AXP209
- **Function**: Complete power management IC
- **Features**:
  - Multiple voltage rails (3.3V, 1.2V, 2.8V)
  - Battery charging
  - Battery gauge
  - Power button support
- **Cost**: $0.80-1.50
- **Why**: Commonly paired with Allwinner SoCs, proven design
- **I2C interface**: Easy monitoring and control

**Alternative**: Discrete regulators (cheaper but more complex)
- RT9193 3.3V LDO (~$0.30)
- TPS62160 buck converter for 1.2V (~$1.00)

## Input: Buttons

### Button Layout (6 buttons minimum)

```
                [Power]

    [Menu]              [Home]

    [Page ◄]          [Page ►]

         [Select/OK]
```

**Button specifications**:
- **Type**: Tactile push button, through-hole for easier assembly
- **Actuation Force**: 160-200gf (medium feel)
- **Travel**: 0.5mm
- **Life**: 100,000+ cycles

**Specific parts**:
1. **Omron B3F series** (B3F-4055)
   - 6mm x 6mm square
   - Through-hole
   - Cost: $0.20 each
   - Very reliable

2. **C&K PTS645** series
   - 6mm x 6mm square
   - SMD available
   - Cost: $0.15 each
   - Good tactile feel

**Total cost**: ~$1.50 for 6 buttons

### Optional: D-Pad Navigation

Instead of separate page buttons, consider a D-pad:
- **Part**: Omron B3F-50J series (4-way)
- **Cost**: $1.50
- **Benefit**: More compact, familiar feel

## Connectivity: USB

**For MVP**: Micro USB for charging and data
**For Production**: USB Type-C

### Micro USB Connector (MVP)
**Part**: Molex 1050170001
- Cost: $0.30
- Widely available
- Users already have cables

### USB Type-C (Production)
**Part**: GCT USB4085-GF-A
- Cost: $0.60-0.80
- Modern standard
- Reversible connector

### USB Data Interface

**Recommended**: USB gadget mode (USB-OTG)
- Device appears as USB Mass Storage (SD card visible)
- No drivers needed
- Easy file transfer
- Supported by Allwinner H3

## Optional: WiFi Module

**For future version, not MVP**

### Option 1: ESP8266 Module
**Part**: ESP-12F module
- Cost: $1.50-2.00
- 802.11 b/g/n WiFi
- AT command interface
- UART interface to main SoC

### Option 2: Use H3's WiFi (if available on module)
Some H3 modules include WiFi (like Pine64):
- Realtek RTL8189FTV
- Built-in, no extra cost
- Mainline Linux support

**MVP Decision**: Skip WiFi initially, add in Rev 2

## Physical Design Constraints

### Device Dimensions

```
Target device (inspired by trade paperback):
Width:  130mm (5.1")
Height: 185mm (7.3")
Depth:  8mm (0.31")
Weight: 165g target

Display: 6" (122.4mm x 90.6mm active area)
Bezel:   3-5mm per side
Buttons: Along right edge or bottom
```

### PCB Size

- Single PCB: ~120mm x 170mm
- 2-layer for MVP (cheaper)
- 4-layer for production (better signal integrity, smaller)

### Case Material

**MVP**: 3D printed
- PLA or PETG
- Two-piece snap-fit design
- Print cost: ~$5 in materials

**Production**: Injection molded plastic
- One-time tooling: $2000-5000
- Per-unit: $2-3
- Only for volume production (1000+ units)

## Complete BOM: Focused MVP Build

| Component | Part/Model | Qty | Unit Price | Total |
|-----------|------------|-----|------------|-------|
| **Display** | Waveshare 6" E-Ink (1448x1072) | 1 | $50 | $50.00 |
| **Processor Module** | Orange Pi Zero LTS (H3+512MB) | 1 | $18 | $18.00 |
| **Storage** | microSD card slot | 1 | $0.75 | $0.75 |
| **Battery** | 18650 cell holder (2S) | 1 | $1.50 | $1.50 |
| **Battery Cells** | 18650 cells 2500mAh (2x) | 2 | $3.00 | $6.00 |
| **Charging** | TP4056 module with protection | 1 | $1.00 | $1.00 |
| **USB Connector** | Micro USB breakout | 1 | $0.50 | $0.50 |
| **Buttons** | Omron B3F tactile switches | 6 | $0.20 | $1.20 |
| **PCB** | 2-layer, 120x170mm | 1 | $3.00 | $3.00 |
| **Passive Components** | Resistors, caps, etc. | - | - | $2.00 |
| **Connectors** | Headers, JST connectors | - | - | $2.00 |
| **Case** | 3D printed (PLA) | 1 | $5.00 | $5.00 |
| **MicroSD Card** | 16GB (for OS + books) | 1 | $5.00 | $5.00 |
| **Misc** | Wire, screws, standoffs | - | - | $2.00 |
| **TOTAL** | | | | **$97.95** |

### Cost Reduction Strategies

This BOM is over our $50 target. Ways to reduce:

1. **Use 5.83" display instead of 6"**: Save $20
   - New total: ~$78

2. **Use cheaper 6" display** (older generation, 800x600):
   - Save $20
   - New total: ~$78

3. **Design custom H3 board** instead of module:
   - H3 chip: $4
   - 512MB RAM: $5
   - Power management: $3
   - Total: $12 (save $6)
   - But requires more complex PCB design

4. **Bulk pricing** (100+ units):
   - Display: $35 (save $15)
   - Components: 30% discount ($10 saved)
   - New total: ~$73

5. **Use 4.3" display** (compromise):
   - Display: $20 (save $30)
   - Smaller case, PCB
   - Total: ~$65

### Recommended MVP Configuration for $50 Target

To meet budget, use these substitutions:

| Component | Alternative | Price |
|-----------|-------------|-------|
| Display | Good Display 6" 800x600 (GDEW060T7) | $30 |
| Processor | Design custom H3 board | $12 |
| Storage | microSD slot | $0.75 |
| Battery | 2000mAh Li-Po | $4 |
| Charging | TP4056 IC + DW01A | $1 |
| USB | Micro USB | $0.30 |
| Buttons | Generic tactile | $1.00 |
| PCB | 2-layer | $2 |
| Passives | - | $2 |
| Case | 3D printed | $4 |
| **TOTAL** | | **$57** |

Still slightly over, but achievable with:
- Bulk component pricing
- Smaller case (less material)
- DIY assembly (no labor cost)

## Suppliers and Part Numbers

### Display Sources
1. **Waveshare** (waveshare.com) - Best documentation
2. **Good Display** (www.good-display.com) - OEM, bulk pricing
3. **Buydisplay** (www.buydisplay.com) - US distributor
4. **AliExpress** - Cheapest, longer shipping

### Electronic Components
1. **Digikey** (digikey.com) - Fast, reliable, US
2. **Mouser** (mouser.com) - Fast, reliable, US
3. **LCSC** (lcsc.com) - Cheap, ships from China
4. **Arrow** - Good for SoCs and modules

### PCB Manufacturing
1. **JLCPCB** (jlcpcb.com) - Cheap, good quality ($2 for 5 boards)
2. **PCBWay** - Similar pricing
3. **OSH Park** - US-based, higher quality, more expensive

### 3D Printing
1. **Print yourself** - Cheapest
2. **PCBWay** - Combined PCB + case order
3. **Craftcloud3d** - Compare prices
4. **Local makerspace** - Often cheapest option

## Next Steps

1. Order development board: **Orange Pi Zero LTS** ($18)
2. Order display for testing: **Waveshare 5.83"** ($30) - easier to start with
3. Test software stack on dev board
4. Once software works, design custom PCB
5. Order 6" display for final version

Would you like me to create:
- Detailed schematic for the H3-based design?
- 3D case model with exact dimensions?
- Buildroot configuration for the H3?
- Display driver integration guide?
