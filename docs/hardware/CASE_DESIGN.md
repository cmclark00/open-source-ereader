# 3D Case Design Specifications

Complete specifications for 3D printable case design.

## Design Overview

**Style**: Two-piece snap-fit design (front + back)
**Material**: PLA or PETG
**Print time**: ~6-8 hours total (both pieces)
**Material cost**: ~$4-5
**Finish**: Matte black (or custom color)

## Overall Dimensions

```
External dimensions:
Width:  130mm (5.1")
Height: 185mm (7.3")
Depth:  8mm (0.31")

Internal clearances:
Display area: 122.4mm x 90.6mm (flush with front)
PCB space: 120mm x 170mm x 3mm
Battery space: 70mm x 40mm x 5mm
```

## Front Case Design

```
Cross-section view (side):

    ┌───────────────────────────────┐ ← 0mm (top surface)
    │   ┌───────────────────────┐   │
    │   │  Display Window       │   │ ← 0.5mm bezel
    │   │  (e-ink visible)      │   │
    │   │                       │   │
  2mm│   │                       │   │
    │   └───────────────────────┘   │
    │                               │
    │   [●][●][●]                  │ ← Button holes
    │   [●][●][●]                  │   2mm dia, 4mm deep
    │                               │
    │   Mounting posts ○  ○        │ ← M2.5 threaded inserts
    │                               │
    └───────────────┬───────────────┘
                    │
                 Snap tabs → Interlock with back case
```

### Front Case Features:

1. **Display bezel**:
   - 3mm minimum around display
   - Slight lip (0.5mm) to hold display in place
   - Smooth, rounded edges for comfort

2. **Button holes**:
   - 8 holes for tactile switches
   - 6.5mm diameter (for 6mm switch + tolerance)
   - Recessed 1mm for switch body
   - Button cap protrudes 2mm from surface

3. **Mounting posts**:
   - 4 posts with M2.5 threaded insert holes
   - Height: 6mm (matches component clearance)
   - Diameter: 5mm (4mm insert + 0.5mm wall)

4. **Snap tabs**:
   - 6-8 tabs around perimeter
   - 0.5mm interference fit
   - Chamfered edges for easy assembly
   - Living hinge for flexibility

5. **USB port cutout**:
   - Bottom edge, centered
   - 10mm x 5mm opening
   - Chamfered for cable clearance

6. **Screen protector recess** (optional):
   - 0.3mm deep recess for tempered glass
   - Adhesive screen protector fits flush

## Back Case Design

```
Cross-section view (side):

    ┌───────────────────────────────┐ ← 0mm (outer surface)
    │                               │
    │   ┌───────────────────────┐   │
  2mm│   │  PCB mounting area  │   │ ← Standoffs for PCB
    │   │                     │   │
    │   │  ┌───────────────┐  │   │
    │   │  │  Battery bay  │  │   │ ← Recessed area
  5mm│   │  │  (underneath) │  │   │   for 18650 cells
    │   │  └───────────────┘  │   │
    │   │                     │   │
    │   └───────────────────────┘   │
    │                               │
    │   Snap receptacles ▼▼▼       │ ← Interlocks with front
    │                               │
    └───────────────────────────────┘
```

### Back Case Features:

1. **Battery compartment**:
   - Recessed area: 72mm x 42mm x 5mm deep
   - Holds 2x 18650 cells in holder
   - Wire routing channels to PCB
   - Retention clips or velcro strap

2. **PCB standoffs**:
   - 4 standoffs matching front case posts
   - Height: 2mm (clearance for components)
   - Snap-fit or screw attachment

3. **Ventilation** (optional):
   - Small holes or slots for heat dissipation
   - 1mm holes in grid pattern
   - Over processor area only

4. **Grip texture**:
   - Diamond knurling pattern on back
   - 0.3mm raised texture
   - Improves grip, looks professional

5. **Label recess**:
   - Small indented area for device info
   - 40mm x 20mm x 0.5mm deep
   - Can place sticker or engrave text

## Assembly Method

### Option 1: Snap-Fit (Recommended for MVP)

```
Snap tab detail:

Front case tab (male):
    ┌─────┐
    │     │
    │   ┌─┴─┐  ← 0.5mm hook
    │   │   │
    ├───┴───┤
    │       │  ← 0.8mm flexible arm
    └───────┘

Back case receptacle (female):
    ┌────────┐
    │  ┌──┐  │
    │  │  │  │  ← Pocket for tab
    │  └──┘  │
    │        │
    └────────┘

Assembly: Press together until tabs click
Disassembly: Carefully pry apart (may require flat tool)
```

**Pros**:
- No screws needed
- Clean appearance
- Quick assembly
- Lightweight

**Cons**:
- May wear out after many open/close cycles
- Requires careful printing (tabs can break)
- Not as secure as screws

### Option 2: Screw Assembly

```
Use 4x M2.5 screws:

Front case:
    ○ ← Threaded insert (heat-set)
        M2.5 x 6mm depth

Back case:
    ○ ← Screw hole (clearance)
        3mm diameter

Screw: M2.5 x 8mm (Pan head or flat head)
```

**Pros**:
- Very secure
- Easy to disassemble/reassemble
- More durable
- Professional look

**Cons**:
- Requires heat-set inserts (extra step)
- Screws visible (unless hidden)
- Slightly heavier

### Hybrid Approach (Recommended):

- 4 screws at corners (primary retention)
- Snap tabs on sides (alignment and rattle prevention)
- Best of both worlds!

## Material Selection

### PLA (Polylactic Acid)
**Properties**:
- Easy to print
- Rigid and strong
- Good surface finish
- Biodegradable
- Low cost

**Settings**:
- Temp: 200-210°C
- Bed: 60°C
- Speed: 40-60 mm/s

**Best for**: Prototyping, indoor use

### PETG (Polyethylene Terephthalate Glycol)
**Properties**:
- More flexible than PLA
- Better impact resistance
- Slight flex in snap tabs (good!)
- More heat resistant
- Food-safe

**Settings**:
- Temp: 230-250°C
- Bed: 75-85°C
- Speed: 30-50 mm/s

**Best for**: Final version, durability

### ABS (Acrylonitrile Butadiene Styrene)
**Properties**:
- Very strong
- Heat resistant
- Can be vapor smoothed (acetone)
- Requires heated enclosure

**Settings**:
- Temp: 230-250°C
- Bed: 100-110°C
- Enclosure required

**Best for**: Production, professional finish

**Recommendation**: PETG for final design (best balance)

## Print Settings

### Front Case:
```
Layer height: 0.2mm (balance of speed/quality)
Infill: 20% (lightweight but strong)
Wall thickness: 1.2mm (3 perimeters)
Top/bottom layers: 4 layers
Supports: Yes (for overhangs around buttons)
Orientation: Face down (best surface finish)
```

### Back Case:
```
Layer height: 0.2mm
Infill: 15% (less critical, save material)
Wall thickness: 1.2mm
Top/bottom layers: 4 layers
Supports: Minimal (mostly printable without)
Orientation: Face up
```

### Print time estimate:
- Front case: 4-5 hours
- Back case: 3-4 hours
- Total: 7-9 hours

### Material usage:
- Front case: ~40g
- Back case: ~30g
- Total: ~70g (~$1.50-2 in filament)

## Button Caps Design

```
Button cap (page forward/back):

    Top view:          Side view:
    ┌─────────┐        ┌─────────┐
    │    ►    │        │  Label  │ ← 1mm tall label
    │         │        ├─────────┤
    │  NEXT   │        │         │ ← 2mm cap height
    └─────────┘        │  ┌───┐  │
    10mm x 8mm         │  │   │  │ ← Stem (fits switch)
                       │  └───┘  │   4.5mm x 4.5mm
                       └─────────┘

Features:
- Raised arrow symbol (0.5mm)
- Text label (optional)
- Slightly concave top (finger comfort)
- Tight fit on switch stem
```

Print separately:
- 8 button caps total
- Print upside-down for smooth top surface
- 0.1mm layer height for detail
- 100% infill (small parts)
- Print time: ~30 minutes for all 8

## Assembly Steps

### 1. Prepare PCB:
- Solder all components
- Test functionality
- Install in front case

### 2. Install threaded inserts (if using screws):
- Heat soldering iron to 200°C
- Press inserts into mounting posts
- Let cool completely

### 3. Mount display:
- Place display in front case bezel
- Secure with small amount of double-sided tape
- Route FPC cable to PCB connector

### 4. Install buttons:
- Snap button caps onto tactile switches
- Verify smooth operation through case holes

### 5. Battery installation:
- Place battery holder in back case
- Route wires through channels
- Secure with velcro strap or clips

### 6. Final assembly:
- Lower PCB into front case
- Align mounting holes
- Press back case onto front case (snap tabs)
- If using screws: Insert and tighten M2.5 screws

### 7. Testing:
- Power on device
- Test all buttons
- Verify display works
- Check for any rattles or loose parts

## Finishing Options

### As-Printed:
- Matte finish
- Visible layer lines
- Functional but basic
- No extra work

### Sanded and Painted:
1. Sand with 200 → 400 → 800 grit
2. Apply primer (filler primer works best)
3. Sand again with 800 grit
4. Apply 2-3 coats of paint
5. Clear coat for protection
**Result**: Smooth, professional finish

### Vapor Smoothing (ABS only):
1. Print in ABS
2. Expose to acetone vapor in sealed container
3. Surface melts slightly and smooths
4. Let cure 24 hours
**Result**: Glass-smooth surface

### Hydro Dipping:
1. Print and sand surface
2. Apply base coat
3. Hydro dip with pattern film
4. Clear coat
**Result**: Custom patterns (carbon fiber, wood grain, etc.)

**Recommendation for MVP**: As-printed PETG (matte black)

## Design Variations

### Minimalist Version:
- No texture on back
- Flush buttons (no caps)
- Thin bezels
- Weight: 40g (case only)

### Ergonomic Version:
- Curved back (follows hand contour)
- Textured grip areas
- Larger bezels for comfort
- Weight: 55g

### Rugged Version:
- Thick walls (2mm)
- Rubber bumpers on corners
- Sealed USB port (with cap)
- Weight: 70g

### Premium Version:
- Wood-filled PLA
- Metal buttons
- Leather texture
- Felt interior lining

## CAD Files Structure

```
/hardware/case/
├── front_case_v1.stl          (Ready to print)
├── back_case_v1.stl           (Ready to print)
├── button_caps.stl            (8 caps on one plate)
├── source/
│   ├── case_design.FCStd      (FreeCAD project)
│   ├── case_design.step       (Universal format)
│   └── parametric_model.scad  (OpenSCAD source)
├── renders/
│   ├── assembled_view.png
│   ├── exploded_view.png
│   └── button_detail.png
└── prints/
    ├── gcode/                 (Sliced for specific printers)
    └── print_profiles/        (Cura/PrusaSlicer profiles)
```

## Bill of Materials - Case

| Item | Specification | Qty | Cost |
|------|---------------|-----|------|
| PLA/PETG Filament | 70g | 1 | $1.75 |
| M2.5 Heat-Set Inserts | 6mm length | 4 | $0.40 |
| M2.5 Screws | 8mm pan head | 4 | $0.20 |
| Velcro Strap | 20mm x 150mm | 1 | $0.50 |
| Rubber Feet | 6mm diameter | 4 | $0.20 |
| **Total** | | | **$3.05** |

## Testing and Validation

### Fit Check:
- [ ] All components fit without forcing
- [ ] No interference between PCB and case
- [ ] Buttons operate smoothly
- [ ] Display sits flush in bezel
- [ ] Battery compartment has clearance
- [ ] USB cable fits port cutout

### Strength Test:
- [ ] Case survives 1m drop onto carpet
- [ ] Snap tabs hold securely
- [ ] No creaking or flexing during normal use
- [ ] Buttons don't crack under repeated pressing

### Ergonomics Test:
- [ ] Comfortable to hold for 30+ minutes
- [ ] All buttons reachable one-handed
- [ ] No sharp edges
- [ ] Good weight balance
- [ ] Device doesn't slip from hand

### Aesthetics:
- [ ] Clean appearance
- [ ] No visible print defects on front
- [ ] Bezels are symmetrical
- [ ] Professional look

## Customization Guide

Users can modify design for:

1. **Different display sizes**:
   - Adjust bezel opening dimensions
   - Resize overall case proportionally

2. **Different button layouts**:
   - Move button hole positions
   - Add or remove buttons

3. **Personalization**:
   - Add name or logo on back
   - Custom color combinations
   - Texture patterns

4. **Accessibility**:
   - Larger buttons
   - Braille labels
   - Ergonomic grip shapes

## Next Steps

1. Create 3D models in FreeCAD
2. Export STL files
3. Slice with Cura/PrusaSlicer
4. Print prototype (fast settings)
5. Test fit with components
6. Iterate design if needed
7. Print final version (high quality)
8. Post-process and assemble

---

**Status**: Design specifications complete
**Ready for**: 3D modeling
**Estimated design time**: 6-8 hours for first model
**Prototype cost**: $3-4 per case set
