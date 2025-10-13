# Build Guide - Open Source E-Reader

Complete step-by-step guide to building your own e-reader from scratch.

## Overview

This guide will walk you through building a trade paperback-sized e-reader with:
- 6" e-ink display (800x600)
- 8 tactile buttons for one-handed reading
- 3+ weeks battery life
- Linux-based OS
- Total cost: ~$75 for first build

**Estimated build time**: 10-15 hours (PCB assembly + case printing + software setup)
**Skill level**: Intermediate (basic soldering and Linux experience helpful)

---

## Phase 1: Gather Materials

### Required Tools

**Soldering**:
- Soldering iron (30-60W, temperature controlled)
- Solder (0.6-1mm, lead or lead-free)
- Flux (liquid or pen)
- Solder wick (for mistakes)
- Helping hands or PCB holder
- Multimeter (for testing)

**Assembly**:
- Small Phillips screwdriver
- Tweezers or small pliers
- Wire strippers
- Flush cutters
- Heat gun or soldering iron (for heat-set inserts)

**3D Printing**:
- 3D printer (or access to one)
- PLA or PETG filament
- Slicing software (Cura, PrusaSlicer)

**Software**:
- Linux PC or VM (for cross-compilation)
- microSD card reader
- USB-to-TTL serial adapter (debugging)

### Order Components

**See**: `/docs/hardware/FINAL_BOM.md` for complete list

**Quick shopping list**:

**From Amazon** (fast shipping):
1. Orange Pi Zero LTS module - $18
2. 6" E-ink display (GDEW060T7 or Waveshare) - $30
3. 2x 18650 battery cells - $6
4. TP4056 charging module (5-pack) - $8
5. Tactile button assortment - $10
6. 18650 battery holder - $2

**From JLCPCB** (PCB):
7. Custom PCB (5 boards minimum) - $2 + $15 shipping

**From Digikey** (electronics):
8. Resistors, capacitors, diodes kit - $15
9. Connectors and headers - $5
10. microSD card 16GB - $5

**Total initial investment**: ~$120
(Includes spares and extra components for testing)

**Budget tip**: Start with Orange Pi + display (~$50) to test software before ordering PCB.

---

## Phase 2: PCB Assembly

### Step 1: Inspect PCB

When PCBs arrive:
- [ ] Check for manufacturing defects
- [ ] Verify dimensions (120mm x 170mm)
- [ ] Confirm mounting holes are clear
- [ ] Check silkscreen is readable

### Step 2: Gather Components

Organize parts by type:
- SMD resistors and capacitors
- Through-hole buttons
- Connectors
- ICs and modules

**Tip**: Use a printed BOM as checklist

### Step 3: SMD Component Soldering

**Order**: Start with smallest parts first

**Surface mount resistors/capacitors**:
1. Apply small amount of solder to one pad
2. Hold component with tweezers
3. Reflow solder on one side to tack part
4. Solder other side
5. Reflow first side if needed

**SMD ICs** (TP4056):
1. Apply flux to pads
2. Align chip carefully
3. Tack one corner pin
4. Check alignment, adjust if needed
5. Solder remaining pins
6. Use solder wick to clean bridges

**Practice first**: If new to SMD, practice on scrap PCB

### Step 4: Through-Hole Components

**Tactile buttons** (8x):
1. Insert buttons from top side
2. Verify they're flush with PCB
3. Solder from bottom side
4. Check each pin is soldered

**Connectors**:
1. USB Micro connector (if SMD, solder like IC)
2. FPC connector for display (careful with pins!)
3. Pin headers for debug/expansion

**Battery holder**:
1. Insert from top
2. Check polarity markings!
3. Solder large pads securely

### Step 5: Modules

**Orange Pi Zero**:
- Option A: Socket headers (removable)
  1. Solder female headers to PCB
  2. Install Orange Pi module

- Option B: Direct solder (permanent)
  1. Place module on headers
  2. Solder all pins
  3. More secure but not removable

**MT3608 Boost converter**:
- Solder header pins or direct mount
- Check input/output polarity!

**TP4056 charging IC**:
- If using module: solder with headers
- If using bare IC: already done in SMD step

### Step 6: Wiring

**Display connection**:
- Use 8-wire ribbon cable (or individual wires)
- Lengths: ~10cm (keep short)
- Connections per schematic:
  ```
  Orange Pi → Display
  SPI_MOSI  → DIN
  SPI_CLK   → CLK
  SPI_CS    → CS
  GPIO2     → DC
  GPIO3     → RST
  GPIO6     → BUSY
  3.3V      → VCC
  GND       → GND
  ```

**Power wiring**:
- Battery → TP4056 → Boost → Orange Pi
- Use 24AWG wire minimum (thicker is better)
- Keep positive and negative separate
- Check polarity multiple times!

### Step 7: Testing

**Visual inspection**:
- [ ] All solder joints shiny and complete
- [ ] No solder bridges
- [ ] All components oriented correctly
- [ ] No missing parts

**Continuity test**:
- [ ] Check power and ground (should NOT be shorted!)
- [ ] Verify button connections to GPIOs
- [ ] Test display SPI lines

**Power test** (NO BATTERY YET):
- Connect USB power to charging port
- Measure voltage at boost converter output: 4.8-5.2V
- Measure current draw: <50mA (no load)

**If voltage wrong**: Check boost converter adjustment pot

---

## Phase 3: Initial Power-On

### Step 1: Prepare microSD Card

**Download OS image** (from project releases):
```bash
# Flash image to SD card
sudo dd if=ereader-os-v1.img of=/dev/sdX bs=4M status=progress
sync
```

Or build from source (see `/docs/software/BUILD_INSTRUCTIONS.md`)

### Step 2: Connect Serial Console

Connect USB-to-TTL adapter:
```
Serial Adapter    →    PCB Debug Header
GND              →    GND (Pin 1)
RX               →    TX  (Pin 2)
TX               →    RX  (Pin 3)
```

Open serial terminal:
```bash
screen /dev/ttyUSB0 115200
# or
sudo minicom -D /dev/ttyUSB0 -b 115200
```

### Step 3: First Boot (No Battery)

1. Insert microSD card into Orange Pi
2. Connect USB power to charging port
3. Watch serial console for boot messages

**Expected output**:
```
U-Boot 2022.10 (Jan 01 2024)
...
Starting kernel ...
[    0.000000] Booting Linux on physical CPU 0x0
...
ereader-os login: root
```

**If no output**:
- Check serial connections
- Verify SD card is good image
- Check Orange Pi is powered (LED on?)
- Try different SD card

### Step 4: Display Test

**Login** (default password: ereader):
```bash
# Test GPIO access
cd /sys/class/gpio
echo 6 > export
echo out > gpio6/direction

# Test SPI
ls /dev/spidev*
# Should show: /dev/spidev0.0

# Run display test
cd /opt/ereader/test
./display-test

# Should show test pattern on display
```

**If display doesn't work**:
- Check SPI connections
- Verify display power (3.3V)
- Check BUSY line
- Review schematic for correct GPIO mappings

---

## Phase 4: Battery Installation

### Step 1: Battery Safety

**IMPORTANT SAFETY NOTES**:
- Use protected 18650 cells only
- Check polarity multiple times
- Never short battery terminals
- Have fire extinguisher nearby
- Work in ventilated area

### Step 2: Install Protection

**If batteries don't have built-in protection**:
- Use TP4056 module with protection IC
- Or add separate BMS (Battery Management System)

**Check TP4056 protection**:
- Should have DW01A chip visible
- Two MOSFETs (FS8205A or similar)

### Step 3: Insert Batteries

1. Check battery holder polarity markings
2. Insert cells with correct polarity:
   ```
   [+ 18650 Cell 1 -]
   [+ 18650 Cell 2 -]
   ```
3. Connect holder to TP4056 BAT+ and BAT-
4. Double-check polarity!
5. Secure wires with tape or heat shrink

### Step 4: Charging Test

1. Connect USB power
2. TP4056 LED should light (red = charging)
3. Measure battery voltage: Should be 7.2-8.4V (2S)
4. Let charge for 10 minutes
5. Disconnect USB
6. Voltage should remain stable

### Step 5: Battery Power Test

1. Disconnect USB
2. Device should stay powered from battery
3. Check boost converter output: Still 5V
4. Measure battery current: 150-300mA typical
5. If works: Successful battery install!

**If device doesn't power from battery**:
- Check boost converter enable pin
- Verify battery voltage >6V
- Check all connections
- Test boost converter separately

---

## Phase 5: 3D Print Case

### Step 1: Prepare Files

Download STL files:
- `front_case_v1.stl`
- `back_case_v1.stl`
- `button_caps.stl`

### Step 2: Slice

**Recommended settings** (Cura/PrusaSlicer):
```
Material: PETG
Layer height: 0.2mm
Infill: 20% (front), 15% (back)
Wall thickness: 1.2mm
Supports: Yes (front case only)
Orientation: Front face down, back face up
Print temperature: 230°C (PETG)
Bed temperature: 80°C
```

### Step 3: Print

**Print order**:
1. Button caps (30 minutes) - Test printer first!
2. Back case (3-4 hours)
3. Front case (4-5 hours)

**During print**:
- Check first layer adhesion
- Monitor for warping
- Ensure supports are printing correctly

### Step 4: Post-Processing

**Remove supports**:
- Carefully cut away support material
- Use flush cutters for tight areas
- Sand support marks with 200-grit

**Install threaded inserts** (if using):
- Heat soldering iron to 200°C
- Press insert into mounting post
- Hold until plastic cools (10 seconds)
- Repeat for all 4 posts

**Clean up**:
- Remove stringing with heat gun
- Sand any rough edges
- Test fit all components

---

## Phase 6: Final Assembly

### Step 1: Install Display

1. Remove protective film from display
2. Place display in front case bezel
3. Apply small strips of double-sided tape
4. Press firmly to seat
5. Route FPC cable to PCB connector

### Step 2: Install PCB

1. Lower PCB into front case
2. Align with mounting posts
3. Route USB cable through port
4. Route battery wires to back compartment

### Step 3: Button Caps

1. Snap button caps onto tactile switches
2. Press firmly until seated
3. Test each button press
4. Should feel smooth and tactile

### Step 4: Close Case

**Snap-fit method**:
1. Align back case with front
2. Press along edges until tabs click
3. Check all sides are flush

**Screw method**:
1. Align back case with front
2. Insert M2.5 screws in 4 corners
3. Tighten evenly (don't overtighten!)
4. Check case doesn't crack

### Step 5: Final Testing

**Power on test**:
- [ ] Device boots from battery
- [ ] Display shows boot logo
- [ ] All buttons respond
- [ ] USB charging works
- [ ] No rattles or loose parts

**Reading test**:
- [ ] Open test EPUB file
- [ ] Page forward/back work
- [ ] Navigation smooth
- [ ] Display refresh looks good
- [ ] Comfortable to hold

---

## Phase 7: Software Setup

### Load Books

**Via USB**:
1. Connect USB cable to PC
2. Device appears as USB mass storage
3. Copy EPUB/PDF files to `/books/` folder
4. Safely eject

**Via microSD** (if no USB):
1. Remove SD card
2. Mount on PC
3. Copy books to `/media/books/`
4. Re-insert card

### Configure Settings

```bash
# SSH into device (if network available)
ssh root@ereader.local

# Or use serial console

# Edit config
vi /etc/ereader/config.conf

# Settings:
# - Font size
# - Refresh rate
# - Sleep timeout
# - Button mapping
```

### Update Firmware

```bash
# Download latest release
wget https://releases/ereader-os-v1.1.img

# Flash to SD card
dd if=ereader-os-v1.1.img of=/dev/mmcblk0 bs=4M

# Reboot
reboot
```

---

## Troubleshooting

### Display Issues

**Problem**: Display not responding
- Check SPI connections
- Verify 3.3V power to display
- Test BUSY line signal
- Try different display test patterns

**Problem**: Ghosting or artifacts
- Increase full refresh frequency
- Check waveform timing
- Verify display temperature compensation

**Problem**: Slow refresh
- May be normal for e-ink (500ms)
- Check SPI clock speed
- Reduce display resolution if supported

### Power Issues

**Problem**: Won't turn on
- Check battery voltage (should be >6V)
- Verify boost converter output
- Check Orange Pi power LED
- Test with USB power only

**Problem**: Battery drains quickly
- Check for high current draw (>100mA idle)
- Verify sleep mode working
- Check for software infinite loops
- Measure standby current

**Problem**: Won't charge
- Check USB cable and adapter
- Verify TP4056 LED lights up
- Check battery polarity
- Test TP4056 module separately

### Button Issues

**Problem**: Button not responding
- Check solder joints
- Verify GPIO connection
- Test with multimeter (continuity)
- Check pull-up resistor

**Problem**: Button stuck or mushy
- Check button cap alignment
- Verify switch not damaged
- Adjust cap fit if too tight
- Replace switch if faulty

### Software Issues

**Problem**: Won't boot
- Check SD card image
- Verify SD card not corrupted
- Try different SD card
- Check serial console for errors

**Problem**: Kernel panic
- Update kernel to latest version
- Check device tree configuration
- Verify RAM size correct
- Review serial console log

---

## Optimization and Tuning

### Display Performance

**Improve refresh speed**:
```bash
# Edit display driver config
vi /boot/config.txt

# Adjust waveform timing
display_waveform=faster

# Reduce full refresh frequency
full_refresh_interval=10
```

### Battery Life

**Extend battery**:
```bash
# Reduce CPU speed
cpufreq-set -f 600MHz

# Aggressive sleep
echo 60 > /sys/power/autosleep_delay

# Disable unused features
systemctl disable wifi
systemctl disable bluetooth
```

### Button Responsiveness

**Adjust debounce**:
```c
// In button driver:
#define DEBOUNCE_MS 30  // Reduce from 50
```

---

## Next Steps

### Enhancements

**Add features**:
- Frontlight for night reading
- WiFi book downloads
- Bluetooth page turner
- Audio support
- Larger battery

**Improve design**:
- Custom font loading
- Better UI themes
- Dictionary support
- Note-taking capability

### Share Your Build

**Document your experience**:
- Take photos during build
- Note any issues encountered
- Share improvements
- Help others in community

**Contribute back**:
- Submit bug reports
- Contribute code improvements
- Share case designs
- Write tutorials

---

## Getting Help

**Resources**:
- Project documentation: `/docs/`
- Community forum: (coming soon)
- GitHub issues: Report bugs
- Discord/Matrix: Real-time chat

**Before asking for help**:
1. Check this build guide
2. Search existing issues
3. Review troubleshooting section
4. Collect debug info (serial console output, photos)

---

## Bill of Materials Quick Reference

| Component | Cost | Where to Buy |
|-----------|------|--------------|
| Orange Pi Zero LTS | $18 | Amazon, AliExpress |
| 6" E-ink display | $30 | Waveshare, Good Display |
| PCB (5x) | $17 | JLCPCB |
| Components | $15 | Digikey, LCSC |
| 18650 cells (2x) | $6 | Battery store |
| Case filament | $4 | 3D printing |
| **Total** | **$90** | (First build with extras) |

**Batch pricing** (10 units): ~$65 per unit
**Production** (100+ units): ~$55 per unit

---

## Safety Warnings

**⚠️ Lithium Battery Safety**:
- Never short circuit batteries
- Don't puncture or crush cells
- Keep away from heat/fire
- Use protected cells only
- Have fire safety equipment ready

**⚠️ Soldering Safety**:
- Work in ventilated area
- Don't breathe solder fumes
- Hot iron can cause burns
- Keep workspace clear

**⚠️ Sharp Tools**:
- Flush cutters are sharp
- Be careful with solder wick
- Protect fingers when crimping

---

**Congratulations on building your e-reader!**

You now have a fully functional, open-source device that you built yourself. Enjoy reading!

---

**Document version**: 1.0
**Last updated**: 2024-01-XX
**Build time estimate**: 10-15 hours
**Difficulty**: Intermediate
