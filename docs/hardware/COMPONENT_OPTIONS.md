# Hardware Component Options

This document outlines potential hardware components for the open-source e-reader project, prioritizing low cost, availability, and open-source support.

## Display Options

### E-Ink Displays

| Size | Resolution | Type | Est. Cost | Notes |
|------|-----------|------|-----------|-------|
| 6.0" | 800x600 | Pearl/Carta | $25-40 | Most common, good availability |
| 4.3" | 800x600 | Pearl | $15-25 | Cheaper, more compact |
| 7.5" | 1872x1404 | Carta | $50-80 | Better for PDFs, exceeds budget |

**Recommended**: 6" 800x600 E-Ink display with SPI interface

**Suppliers**:
- Waveshare (good documentation)
- Good Display (OEM, bulk pricing)
- Dalian Good Display (GDEW060)

**Interface**: SPI preferred (simpler, fewer pins)

## Processor/SoC Options

### Option 1: Allwinner V3s (RECOMMENDED)
- **Cost**: $1-2 in bulk
- **Architecture**: ARM Cortex-A7, 1.2 GHz
- **RAM**: 64MB integrated DDR2
- **Pros**: Ultra-cheap, mainline Linux support, built-in RAM
- **Cons**: Limited RAM (need external RAM chip for better performance)
- **Best for**: Ultra-budget builds

### Option 2: Allwinner H3
- **Cost**: $3-5
- **Architecture**: ARM Cortex-A7 Quad-core, 1.2 GHz
- **RAM**: External DDR3 (256MB-512MB)
- **Pros**: Better performance, good community support
- **Cons**: Requires external RAM
- **Best for**: Balanced performance/cost

### Option 3: Rockchip RK3066
- **Cost**: $5-8
- **Architecture**: ARM Cortex-A9 Dual-core
- **RAM**: 512MB DDR3
- **Pros**: More powerful, good for PDF rendering
- **Cons**: Higher power consumption
- **Best for**: Feature-rich builds

### Option 4: i.MX6 ULL/ULZ
- **Cost**: $4-7
- **Architecture**: ARM Cortex-A7
- **RAM**: 256-512MB DDR3
- **Pros**: Excellent mainline Linux support, industrial-grade
- **Cons**: Slightly more expensive
- **Best for**: Long-term reliability

### Option 5: ESP32-S3 (Alternative approach)
- **Cost**: $2-4
- **Architecture**: Dual-core Xtensa LX7
- **RAM**: 512KB SRAM + external PSRAM
- **Pros**: Very cheap, WiFi/BLE built-in, low power
- **Cons**: Not full Linux (FreeRTOS), limited for complex documents
- **Best for**: Minimalist readers (TXT, simple EPUB)

## Memory

### RAM
- **DDR3 256MB**: $2-3 (minimum for Linux + GUI)
- **DDR3 512MB**: $4-6 (recommended)

### Storage
- **microSD slot**: $0.50 (flexible, user-upgradeable) - RECOMMENDED
- **4GB eMMC**: $2-3
- **8GB eMMC**: $4-5
- **SPI Flash 128MB**: $1-2 (minimal OS only)

## Power Management

### Battery
- **2000mAh Li-Po**: $3-5
- **3000mAh Li-Po**: $5-8
- **18650 cell holder**: $2-3 (user-replaceable option)

### Charging Circuit
- **TP4056 module**: $0.50 (basic USB charging)
- **MCP73831**: $0.30 (IC only, requires PCB design)
- **BQ24075**: $1-2 (better power path management)

### Power Regulation
- **AXP209 PMIC**: $0.80 (commonly used with Allwinner)
- **TPS65217 PMIC**: $2-3 (better efficiency)
- **Discrete regulators**: $1-2 (3.3V and 1.2V LDOs)

## Input Options

### Physical Buttons
- **Tactile switches**: $0.05 each (need 5-8 buttons)
- **Navigation D-pad**: $0.50

### Touch Input
- **Capacitive touch controller**: $1-2 (if display supports it)
- **Resistive touch**: $2-3 (add-on overlay)

## Additional Components

### Connectivity (Optional)
- **ESP8266 WiFi module**: $1-2 (for book downloads)
- **Built-in WiFi**: Free if using H3 or ESP32

### Storage/Reading Light
- **USB OTG port**: $0.20 (for file transfer)
- **Front light LEDs**: $1-2 (for night reading)

## PCB and Assembly

- **2-layer PCB**: $5-10 for prototype (batch of 5)
- **4-layer PCB**: $15-25 (better signal integrity)
- **3D printed case**: $3-5 in materials

## Bill of Materials (BOM) Estimates

### Ultra-Budget Build (~$35-45)
- Allwinner V3s + 64MB DDR2 integrated: $2
- 6" E-ink display: $30
- microSD slot: $0.50
- 2000mAh battery + TP4056: $4
- Buttons and misc: $2
- PCB: $2 (bulk pricing)
- Case: $3

### Recommended Build (~$45-60)
- Allwinner H3 + 512MB DDR3: $8
- 6" E-ink display: $35
- 8GB eMMC: $4
- 3000mAh battery + BQ24075: $7
- Buttons and misc: $3
- PCB: $3
- Case: $4

### Performance Build (~$70-90)
- i.MX6 ULL + 512MB DDR3: $11
- 6" high-res E-ink: $50
- 8GB eMMC: $4
- 3000mAh battery + PMIC: $8
- USB-C, buttons: $4
- 4-layer PCB: $5
- Case: $5

## Recommended Starting Configuration

For the best balance of cost, performance, and ease of development:

1. **SoC**: Allwinner H3 or V3s (H3 if budget allows)
2. **Display**: 6" Waveshare e-Paper display (800x600)
3. **RAM**: 256MB DDR3 minimum (512MB preferred)
4. **Storage**: microSD slot (most flexible)
5. **Battery**: 2500-3000mAh Li-Po with TP4056 charging
6. **Input**: 6-8 physical buttons (page up/down, menu, select, home, back)
7. **Connectivity**: Optional ESP8266 module for WiFi

**Target Total**: $40-50 in component costs (bulk pricing)

## Next Steps

1. Select specific display model and obtain datasheet
2. Choose SoC and create initial schematic
3. Design power tree
4. Plan PCB layout
5. Order development board or reference design for testing
