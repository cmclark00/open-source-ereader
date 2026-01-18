---
type: reference
title: Case and Enclosure Design Guide
created: 2026-01-17
tags:
  - hardware
  - enclosure
  - case
  - 3d-printing
  - assembly
related:
  - "[[BOM]]"
  - "[[WIRING_GUIDE]]"
  - "[[BUTTON_LAYOUT]]"
  - "[[DISPLAY_SPECS]]"
  - "[[BUILD_GUIDE]]"
---

# Case and Enclosure Design Guide

This document provides comprehensive information for designing and building a physical enclosure for the Open Source E-Reader. Multiple options are presented, from simple DIY solutions to custom 3D-printed cases.

## Table of Contents

1. [Design Requirements](#design-requirements)
2. [Enclosure Options](#enclosure-options)
3. [3D Printable Case Design](#3d-printable-case-design)
4. [Commercial Project Box Option](#commercial-project-box-option)
5. [Laser-Cut Acrylic Option](#laser-cut-acrylic-option)
6. [DIY Cardboard/Wood Option](#diy-cardboardwood-option)
7. [Assembly Instructions](#assembly-instructions)
8. [Finishing Techniques](#finishing-techniques)
9. [Design Files and Resources](#design-files-and-resources)

---

## Design Requirements

### Physical Dimensions

The case must accommodate the following components and their dimensions:

#### Core Components

| Component | Dimensions (L × W × H) | Notes |
|-----------|------------------------|-------|
| Raspberry Pi Zero W | 65mm × 30mm × 5mm | Including USB/HDMI ports protruding 3mm |
| Waveshare 4.2" Display Module | 103mm × 78.5mm × 1.3mm | Display panel only |
| Display Driver Board | 39mm × 29mm × 9mm | Mounted on back of display |
| Display Active Area | 84.8mm × 63.6mm | Visible screen area to expose |
| GPIO Header (soldered) | 51mm × 5mm × 8.5mm | 2x20 pins, may have wires attached |
| MicroSD Card | 15mm × 11mm × 1mm | Protrudes 2mm when inserted |
| 5x Tactile Buttons | 6×6mm or 12×12mm | Height varies: 4.3-7.3mm |

#### Optional Components (Future Phases)

| Component | Dimensions | Notes |
|-----------|------------|-------|
| LiPo Battery (2000mAh) | 60mm × 35mm × 8mm | Optional for portable use |
| PowerBoost 1000C | 36mm × 22mm × 6mm | Battery charging circuit |
| Power Switch | 13mm × 8mm × 6mm | Toggle or slide switch |

### Minimum Internal Dimensions

Based on the component sizes above, the minimum internal dimensions are:

- **Width:** 110mm (allows 3mm clearance on each side of display)
- **Height:** 85mm (allows clearance above/below display)
- **Depth:** 15mm minimum (20mm recommended for comfortable grip and wire routing)
  - 5mm for Pi Zero W
  - 9mm for display driver board
  - 3-6mm for clearance and internal wiring

**Recommended external dimensions:** 120mm × 95mm × 22mm (±5mm)

### Required Features

The case design must include:

1. **Display Window:** 86mm × 65mm minimum opening for screen visibility
2. **Button Access:** 5 openings or external button caps for UP, DOWN, SELECT, BACK, MENU
3. **Power Access:** Opening for micro-USB power connector (or DC barrel jack)
4. **SD Card Access:** Slot or removable panel to access microSD card
5. **Ventilation:** Small ventilation holes (optional but recommended)
6. **Assembly Method:** Snap-fit, screws, or sliding panels to close case

### Optional Features

- **Mounting holes:** For wall mount or stand attachment
- **Lanyard/strap attachment:** Loop or hole for carrying strap
- **Reset button access:** Small hole aligned with Pi's reset pads (if soldered)
- **Status LED window:** Transparent section to see Pi's activity LED
- **Serial console access:** Small opening for USB-TTL adapter (debugging)
- **Speaker grille:** For future audio features
- **Kickstand:** Integrated or attachable stand for hands-free reading

---

## Enclosure Options

### Option 1: 3D Printed Custom Case
- **Difficulty:** Medium
- **Cost:** $10-20 (filament + printing service)
- **Tools Required:** 3D printer or access to printing service, sandpaper, small screwdriver
- **Pros:** Perfect fit, professional appearance, customizable, durable
- **Cons:** Requires 3D modeling skills or using provided files, printing time

### Option 2: Commercial Project Box
- **Difficulty:** Easy
- **Cost:** $8-15
- **Tools Required:** Drill, files, hobby knife, ruler
- **Pros:** Quick, readily available, no printing needed
- **Cons:** May not fit perfectly, requires manual cutting/drilling

### Option 3: Laser-Cut Acrylic
- **Difficulty:** Medium
- **Cost:** $15-30 (acrylic sheets + cutting service)
- **Tools Required:** Laser cutter access or online service, screwdriver
- **Pros:** Professional look, precise cuts, transparent options
- **Cons:** Fragile, requires laser cutter access

### Option 4: DIY Cardboard/Wood
- **Difficulty:** Easy
- **Cost:** $5-10
- **Tools Required:** Hobby knife, ruler, glue, paint (optional)
- **Pros:** Cheap, accessible, rapid prototyping
- **Cons:** Less durable, less professional appearance

---

## 3D Printable Case Design

### Design Overview

The 3D-printable case consists of two main parts:

1. **Front Panel:** Houses the display, buttons, and provides the user interface
2. **Rear Shell:** Contains the Raspberry Pi, wiring, and optional battery
3. **Optional Middle Spacer:** Additional clearance if using battery

### Design Specifications

**File Format:** STL (for printing), STEP/F3D (for editing)
**Recommended Material:** PLA or PETG filament
**Layer Height:** 0.2mm (0.1mm for fine details)
**Infill:** 20% (walls and buttons should be 100%)
**Print Time:** 3-5 hours total (both parts)
**Supports:** Required for button wells and overhang areas

### Detailed Component Design

#### Front Panel Design

```
Dimensions: 120mm × 95mm × 3mm base thickness
Features:
- Display window: 87mm × 66mm (centered)
- Display bezel: 1.5mm overlap to secure screen
- Button wells: 5 circular recesses (7mm diameter for 6x6mm buttons)
- Mounting posts: 4 corner posts with screw holes (M2.5 or M3)
- Cable routing channel: 5mm × 2mm groove from display to Pi cavity
```

**Button Placement (recommended layout):**
```
Front view:
                    [MENU]
         +------------------+
         |   ____________   |
    [UP] |  |            |  | [BACK]
         |  |  Display   |  |
  [DOWN] |  |  84×63mm   |  | [SELECT]
         |  |____________|  |
         +------------------+
```

Alternative layouts:
- **Bottom edge:** All 5 buttons in a row below display
- **Side edge:** Vertical column of buttons on right side
- **Gamepad style:** UP/DOWN/SELECT as D-pad, BACK/MENU as shoulder buttons

#### Rear Shell Design

```
Dimensions: 120mm × 95mm × 19mm depth
Features:
- Raspberry Pi mounting: 4 M2.5 standoff posts (58mm × 23mm spacing)
- SD card slot: 16mm × 12mm opening aligned with Pi's SD slot
- Power port: 9mm × 4mm opening for micro-USB connector
- Ventilation slots: 3-5 parallel slots (30mm × 1.5mm) on sides
- Wire channels: Internal grooves for routing jumper wires
- Screw bosses: 4 corner posts with M3 threaded inserts (or screw posts)
```

#### Button Caps (separate parts)

```
Quantity: 5 (one for each button)
Dimensions: 10mm diameter × 8mm height (for 12×12mm buttons)
            8mm diameter × 6mm height (for 6×6mm buttons)
Features:
- Stem: 5.5mm × 5.5mm square (fits 6×6mm button) or 11.5mm (fits 12×12mm)
- Stem depth: 2mm insertion into button switch
- Top surface: Textured or labeled (UP arrow, DOWN arrow, etc.)
- Press feel: 0.5mm travel before engaging switch
```

**Button cap labeling options:**
- Embossed icons (up/down arrows, checkmark, X, hamburger menu)
- Printed labels (apply after printing)
- Color coding (different filament colors)
- Braille dots (accessibility feature)

### 3D Modeling Instructions

#### Using OpenSCAD (Parametric Design)

If designing from scratch or customizing, OpenSCAD allows parametric modeling:

```openscad
// ereader_case.scad
// Parametric e-reader case design

// Parameters (adjust as needed)
case_width = 120;
case_height = 95;
case_depth = 19;
wall_thickness = 2.5;
display_width = 87;
display_height = 66;
button_diameter = 7;

// Front panel module
module front_panel() {
    difference() {
        // Base plate
        cube([case_width, case_height, 3]);

        // Display cutout
        translate([(case_width - display_width)/2,
                   (case_height - display_height)/2, -1])
            cube([display_width, display_height, 5]);

        // Button holes (example for one button)
        translate([10, case_height/2, -1])
            cylinder(h=5, d=button_diameter);
    }

    // Add mounting posts
    translate([5, 5, 3])
        cylinder(h=3, d=5);
    // ... repeat for other corners
}

// Rear shell module
module rear_shell() {
    difference() {
        // Outer shell
        cube([case_width, case_height, case_depth]);

        // Hollow interior
        translate([wall_thickness, wall_thickness, wall_thickness])
            cube([case_width - 2*wall_thickness,
                  case_height - 2*wall_thickness,
                  case_depth]);

        // SD card slot
        translate([-1, 30, 7])
            cube([wall_thickness + 2, 16, 12]);

        // Micro-USB port
        translate([-1, 50, 9])
            cube([wall_thickness + 2, 9, 4]);
    }

    // Pi mounting posts
    pi_post_x = [10, 68, 10, 68];
    pi_post_y = [10, 10, 33, 33];
    for (i = [0:3]) {
        translate([pi_post_x[i], pi_post_y[i], wall_thickness])
            cylinder(h=8, d=5);
    }
}

// Render parts
front_panel();
translate([0, case_height + 10, 0])
    rear_shell();
```

**OpenSCAD file location:** `hardware/3d-models/ereader_case.scad`

#### Using FreeCAD (GUI Modeling)

FreeCAD provides a visual interface for CAD design:

1. **Create Base Sketch:**
   - New document → Part Design workbench
   - Create sketch on XY plane with rectangle 120mm × 95mm
   - Pad sketch to 3mm thickness (front panel)

2. **Add Display Cutout:**
   - Create new sketch on front face
   - Rectangle 87mm × 66mm centered
   - Pocket cut through panel

3. **Add Button Holes:**
   - Create sketch with 5 circles (7mm diameter)
   - Position according to button layout
   - Pocket 5mm deep

4. **Create Rear Shell:**
   - Similar process with deeper extrusion (19mm)
   - Shell command to hollow interior (2.5mm walls)
   - Boolean cuts for ports

**FreeCAD file location:** `hardware/3d-models/ereader_case.FCStd`

### Printing Guidelines

#### Printer Settings

| Setting | Recommended Value | Notes |
|---------|------------------|-------|
| Material | PLA or PETG | PLA easier, PETG more durable |
| Nozzle Temp | 200-220°C (PLA), 230-250°C (PETG) | Check filament specs |
| Bed Temp | 60°C (PLA), 80°C (PETG) | Heated bed recommended |
| Layer Height | 0.2mm | 0.1mm for smoother finish |
| Wall Lines | 3-4 | Stronger walls |
| Infill | 20% | Gyroid or grid pattern |
| Supports | Yes | For button wells and overhangs |
| Brim/Raft | Optional | Improves bed adhesion |

#### Print Orientation

- **Front Panel:** Print face-down (display side down) for smoothest finish
- **Rear Shell:** Print opening-up to minimize supports
- **Button Caps:** Print stem-up with supports

#### Post-Processing

1. **Remove Supports:** Carefully remove support material with pliers
2. **Clean Holes:** Use 2.5mm or 3mm drill bit to clean screw holes
3. **Test Fit:** Verify components fit before final assembly
4. **Sand Edges:** Use 220-grit sandpaper for smooth edges
5. **Optional Painting:** See [Finishing Techniques](#finishing-techniques)

### Assembly Hardware

| Item | Specification | Quantity | Purpose |
|------|--------------|----------|---------|
| Machine Screws | M3 × 10mm | 4 | Attach front to rear |
| Standoff Screws | M2.5 × 6mm | 4 | Mount Raspberry Pi |
| Threaded Inserts | M3 heat-set inserts | 4 | Optional, for re-assembly |
| Adhesive | Hot glue or double-sided tape | As needed | Secure display |

---

## Commercial Project Box Option

For those without access to 3D printing, commercial project boxes provide a quick enclosure solution.

### Recommended Project Boxes

#### Option A: Hammond 1591XXCBK
- **Dimensions:** 120mm × 94mm × 34mm (external)
- **Material:** ABS plastic (black)
- **Cost:** $8-12
- **Supplier:** Digi-Key, Mouser, Amazon
- **Part Number:** Hammond 1591XXCBK

**Pros:** Perfect size, widely available, durable
**Cons:** Requires drilling/cutting for display and buttons

#### Option B: Takachi SW-95B
- **Dimensions:** 115mm × 90mm × 30mm
- **Material:** ABS plastic
- **Cost:** $10-15
- **Supplier:** Digi-Key, Mouser

**Pros:** Clean design, good size match
**Cons:** Less common, may require ordering

#### Option C: Serpac 032C
- **Dimensions:** 130mm × 95mm × 40mm
- **Material:** ABS plastic
- **Cost:** $8-12
- **Supplier:** Digi-Key

**Pros:** Extra room for battery
**Cons:** Slightly oversized

### Modification Instructions

#### Tools Required
- Rotary tool (Dremel) or drill
- Step drill bits (for round holes)
- Files (flat and round)
- Hobby knife
- Ruler and marker
- Safety glasses

#### Step-by-Step Modification

**1. Mark Display Opening**
- Measure and mark center of front panel
- Draw rectangle 86mm × 65mm centered
- Use ruler to ensure straight lines

**2. Cut Display Window**
- Drill pilot holes at corners (3mm)
- Use hobby knife to score outline
- Cut along lines with rotary tool or coping saw
- File edges smooth and square
- Test-fit display module

**3. Mark Button Positions**
- Layout button positions according to design
- For 6×6mm buttons: 7mm holes
- For 12×12mm buttons: 13mm holes
- Ensure adequate spacing (minimum 15mm between centers)

**4. Drill Button Holes**
- Use step drill bit for clean holes
- Start with pilot hole (2mm)
- Gradually enlarge to final size
- Test-fit buttons through holes

**5. Cut Power Port Access**
- Mark micro-USB position on side panel
- Rectangle approximately 9mm × 4mm
- File to size, test with cable connected

**6. Cut SD Card Access**
- Rectangle on rear or side: 16mm × 12mm
- Align with Pi's SD card slot position
- Consider removable panel with small screws

**7. Optional: Ventilation Holes**
- Drill 3-5 small holes (2-3mm) on sides
- Improves air circulation
- Not critical for Pi Zero (low heat)

#### Button Mounting

**Method 1: Direct Mount (Recessed)**
- Drill holes sized for button body
- Buttons sit flush or slightly recessed
- Secure with hot glue from inside
- Access buttons with finger through hole

**Method 2: Panel Mount Buttons**
- Use panel-mount tactile switches instead
- Larger mounting holes (varies by switch type)
- Threads or snap-fit secure to panel
- More expensive but cleaner look

**Method 3: Extended Actuators**
- 3D print or purchase button caps/extenders
- Mount buttons inside case on PCB
- Actuators extend through holes in panel
- Professional appearance

### Internal Layout

```
Top-down view inside project box:

+----------------------------------------+
|  [Display Module]                      |
|  +----------------------------+        |
|  | Waveshare 4.2" E-Paper     |        |
|  | 103×78mm                   |        |
|  +----------------------------+        |
|          |                             |
|          | (Ribbon cable)              |
|          v                             |
|  +--[Driver Board]--+                  |
|  |    39×29mm      |                  |
|  +-----------------+                  |
|                                        |
|  +--[Raspberry Pi Zero W]--+          |
|  |      65×30mm             |          |
|  |  +-+-+                  |          |
|  |  |SD |                  |          |
|  +--+---+------------------+          |
|                                        |
|  [Wire routing area]                  |
|                                        |
|  [Optional: Battery space]            |
+----------------------------------------+
```

### Securing Components

- **Display:** Double-sided foam tape or hot glue around edges
- **Raspberry Pi:** M2.5 screws into standoffs (glue standoffs to case bottom)
- **Buttons:** Hot glue or epoxy from inside
- **Wires:** Cable ties or adhesive clips to prevent movement

---

## Laser-Cut Acrylic Option

Laser-cut acrylic provides a professional, modern aesthetic with precise dimensions.

### Design Overview

The acrylic case uses a layered construction:

1. **Front Panel:** 3mm clear or black acrylic with display cutout
2. **Spacer Layers:** 3-5mm acrylic forming walls (can use multiple 3mm layers)
3. **Rear Panel:** 3mm acrylic with port cutouts
4. **Internal Supports:** Small acrylic brackets for mounting Pi

Total layers: 5-7 pieces depending on desired thickness

### Materials Required

| Material | Thickness | Dimensions | Quantity | Cost |
|----------|-----------|------------|----------|------|
| Clear/Black Acrylic | 3mm | 120mm × 95mm | 2 sheets | $8 |
| Acrylic (spacer) | 3mm | 120mm × 95mm | 3-4 sheets | $12-16 |
| M3 Screws | 25mm length | - | 4-6 | $2 |
| M3 Nuts | - | - | 4-6 | $1 |

**Total Cost:** $23-30 + laser cutting service ($10-20)

### Laser Cutting Design Files

Create vector files (SVG, DXF, or AI format) for laser cutting:

#### Layer 1: Front Panel (3mm)
```
- Outer rectangle: 120mm × 95mm
- Display cutout: 87mm × 66mm (centered)
- Button holes: 5× circles, 7mm diameter (positioned per layout)
- Corner mounting holes: 4× circles, 3.2mm diameter
- Engrave labels: "UP", "DOWN", etc. (optional)
```

#### Layers 2-4: Spacer Frames (3mm each)
```
- Outer rectangle: 120mm × 95mm
- Inner cutout: 115mm × 90mm (creates 2.5mm wall)
- Corner mounting holes: 4× circles, 3.2mm diameter
- Cable routing notches: 10mm × 3mm cutouts as needed
- Alignment pins: 2mm holes for dowel pins (optional)
```

#### Layer 5: Rear Panel (3mm)
```
- Outer rectangle: 120mm × 95mm
- SD card slot: 16mm × 12mm rectangle
- Micro-USB port: 9mm × 4mm rectangle
- Ventilation holes: Pattern of 3mm circles
- Corner mounting holes: 4× circles, 3.2mm diameter
```

#### Internal Supports (3mm)
```
- Pi standoffs: 4× rectangles 10mm × 10mm with M2.5 hole
- Display support brackets: 2× L-brackets 20mm × 15mm
```

**File locations:** `hardware/laser-cut/ereader_case_[layer].svg`

### Assembly Instructions

1. **Prepare Acrylic Pieces:**
   - Remove protective film from acrylic
   - Clean with isopropyl alcohol
   - Check all holes align properly

2. **Test Fit Components:**
   - Place Pi in position, verify mounting holes
   - Test display fit in front panel cutout
   - Ensure buttons fit through holes

3. **Glue Internal Supports:**
   - Use acrylic cement (welding) or epoxy
   - Attach Pi standoffs to rear panel
   - Let cure completely (24 hours)

4. **Stack Spacer Layers:**
   - Apply acrylic cement between spacer layers
   - Use alignment pins or clamps
   - Ensure even bond

5. **Mount Components:**
   - Secure Raspberry Pi to standoffs
   - Connect all wiring
   - Position display in front panel cutout

6. **Final Assembly:**
   - Attach front panel with M3 screws
   - Attach rear panel with M3 screws
   - Tighten evenly, don't over-torque (acrylic can crack)

### Finishing

- **Edge Polishing:** Flame polish or sand with fine grit (1000+) for clear edges
- **Engraving:** Laser engrave labels, logos, or artwork
- **LED Effects:** Add RGB LED strips between layers for backlighting (purely aesthetic)

---

## DIY Cardboard/Wood Option

Perfect for prototyping or low-cost builds.

### Cardboard Prototype

**Materials:**
- Thick cardboard (corrugated, 5mm+)
- Craft glue or hot glue
- Ruler, pencil, hobby knife
- Optional: Paint or contact paper

**Construction:**
1. Create template on paper (120mm × 95mm)
2. Transfer to cardboard, cut out pieces
3. Cut display window and button holes
4. Layer cardboard for thickness (3-4 layers = ~20mm depth)
5. Glue layers together
6. Mount components with hot glue
7. Optional: Paint or cover with decorative paper

**Pros:** Immediate, costs < $5, easy to modify
**Cons:** Not durable, looks DIY

### Wood Enclosure

**Materials:**
- Thin plywood or basswood (3-6mm)
- Wood glue
- Sandpaper
- Wood stain or paint
- Optional: Varnish

**Construction:**
1. Cut wood panels to size (use template)
2. Cut display window with scroll saw or laser cutter
3. Drill button holes with Forstner bits
4. Assemble box with wood glue and small brads
5. Sand all surfaces smooth
6. Stain or paint
7. Apply varnish for protection

**Pros:** Durable, looks handcrafted, natural aesthetic
**Cons:** Requires woodworking tools, more time-intensive

**Estimated Cost:** $10-20
**Difficulty:** Medium
**Time:** 3-5 hours

---

## Assembly Instructions

### General Assembly Sequence

Regardless of enclosure type, follow this general assembly order:

#### 1. Prepare Enclosure
- [ ] Clean all parts
- [ ] Verify all cutouts and holes are correct size
- [ ] Test-fit all components
- [ ] Sand or file any rough edges

#### 2. Install Buttons (if external mount)
- [ ] Insert buttons into front panel holes
- [ ] Secure with hot glue or locking mechanism
- [ ] Test button press action (should click smoothly)

#### 3. Mount Raspberry Pi
- [ ] Attach standoffs or mount to base
- [ ] Ensure SD card is accessible
- [ ] Verify micro-USB port aligns with case opening

#### 4. Connect Display
- [ ] Wire display to Pi according to [[WIRING_GUIDE]]
- [ ] Secure display driver board inside case
- [ ] Route ribbon cable or jumper wires neatly

#### 5. Wire Buttons
- [ ] Connect each button to GPIO pin (see [[BUTTON_LAYOUT]])
- [ ] Connect all button commons to ground rail
- [ ] Use cable ties to organize wiring

#### 6. Test Functionality
- [ ] Power on device (before closing case)
- [ ] Verify display shows image
- [ ] Test all button inputs
- [ ] Check for shorts or loose connections

#### 7. Final Assembly
- [ ] Route all wires away from screw posts
- [ ] Close case and install screws
- [ ] Verify no wires are pinched
- [ ] Test all functions again

#### 8. Finishing Touches
- [ ] Apply labels or button markings
- [ ] Install rubber feet (optional)
- [ ] Add decorative elements

### Wiring Management

**Best Practices:**
- Use color-coded wires (consistent scheme)
- Keep wires as short as practical
- Route wires along case edges
- Use cable ties or adhesive clips
- Label wires with tape tags (optional)
- Leave slack for opening case

**Suggested Color Scheme:**
- Red: 3.3V power
- Black: Ground
- Blue/Green/Yellow: GPIO signals
- Orange: Display control signals

### Button Wiring Inside Case

If buttons are mounted in front panel and Pi is in rear:

1. **Pre-solder Wire Leads:**
   - Solder 15-20cm wires to each button terminal
   - Use heat shrink tubing to insulate
   - Label each wire (UP, DOWN, SELECT, etc.)

2. **Route Through Case:**
   - Thread wires from front to rear through cable channel
   - Keep wires flat along case wall
   - Avoid crossing wires at different heights

3. **Connect to Pi GPIO:**
   - Use female Dupont connectors or solder directly to header
   - Group ground wires together
   - Secure wire bundle with cable tie

---

## Finishing Techniques

### Painting 3D Printed Parts

**Materials:**
- Sandpaper: 150, 220, 400 grit
- Filler primer spray paint
- Color spray paint (your choice)
- Clear coat (matte or gloss)
- Masking tape

**Process:**
1. **Sand Surface:** Start with 150 grit, progress to 220 grit
2. **Fill Layer Lines:** Apply filler primer (2-3 coats)
3. **Sand Smooth:** Use 400 grit for smooth finish
4. **Apply Color:** 2-3 thin coats, 15 min between coats
5. **Clear Coat:** 2 coats for protection and finish
6. **Cure:** Let dry 24-48 hours before assembly

**Color Suggestions:**
- Matte black (classic e-reader look)
- White or gray (minimalist)
- Wood grain finish (unique aesthetic)
- Custom colors to match decor

### Surface Treatments

#### Vinyl Wrap
- Purchase vinyl skin material
- Cut to size of panels
- Apply with heat gun for smooth finish
- Wide variety of colors and textures available

#### Fabric/Leather Covering
- Thin fabric or faux leather
- Apply with contact cement
- Trim excess with hobby knife
- Creates premium book-like feel

#### Hydro Dipping
- Water transfer printing technique
- Many patterns available (carbon fiber, wood grain, etc.)
- Requires dipping tank and film
- Professional results with practice

### Adding Texture

- **Stippling:** Use rotary tool with burr bit for grip texture
- **Knurling:** Diamond pattern for better grip
- **Soft-Touch Coating:** Rubber-like spray coating (Plasti-Dip)

### Button Customization

- **Labels:** Apply vinyl letters or symbols
- **Embossed Icons:** 3D printed with raised symbols
- **Colored Caps:** Print in different filament colors
- **Backlighting:** Add LED under translucent button caps

---

## Design Files and Resources

### Available Design Files

All design files for the case are available in the repository:

```
hardware/
├── 3d-models/
│   ├── ereader_case_front.stl          # Ready to print
│   ├── ereader_case_rear.stl           # Ready to print
│   ├── ereader_button_cap_6mm.stl      # For 6×6mm buttons
│   ├── ereader_button_cap_12mm.stl     # For 12×12mm buttons
│   ├── ereader_case.scad               # OpenSCAD source (parametric)
│   ├── ereader_case.FCStd              # FreeCAD source
│   └── ereader_case.step               # STEP format (universal)
├── laser-cut/
│   ├── layer_1_front.svg               # Front panel
│   ├── layer_2-4_spacer.svg            # Spacer frames
│   ├── layer_5_rear.svg                # Rear panel
│   ├── supports.svg                    # Internal brackets
│   └── README.txt                      # Assembly notes
├── schematics/
│   └── case_dimensions.pdf             # Dimension drawings
└── photos/
    ├── assembled_front.jpg
    ├── assembled_rear.jpg
    ├── internal_layout.jpg
    └── button_detail.jpg
```

**Note:** Some files may be works-in-progress. Check the `README.txt` in each directory for current status.

### Design Tools

#### 3D Modeling Software
- **OpenSCAD** (Free, open-source, parametric) - [openscad.org](https://openscad.org)
- **FreeCAD** (Free, open-source, GUI-based) - [freecadweb.org](https://www.freecadweb.org)
- **Fusion 360** (Free for hobbyists) - [autodesk.com/fusion360](https://www.autodesk.com/products/fusion-360)
- **Blender** (Free, powerful but complex) - [blender.org](https://www.blender.org)

#### 2D Vector Software (for laser cutting)
- **Inkscape** (Free, open-source) - [inkscape.org](https://inkscape.org)
- **LibreCAD** (Free, CAD-focused) - [librecad.org](https://librecad.org)
- **Adobe Illustrator** (Paid, industry standard)

#### Slicer Software (for 3D printing)
- **Cura** (Free, beginner-friendly) - [ultimaker.com/cura](https://ultimaker.com/software/ultimaker-cura)
- **PrusaSlicer** (Free, advanced features) - [prusa3d.com/prusaslicer](https://www.prusa3d.com/page/prusaslicer_424/)
- **Simplify3D** (Paid, professional)

### External Resources

#### 3D Printing Services
- **Shapeways** - [shapeways.com](https://www.shapeways.com)
- **Sculpteo** - [sculpteo.com](https://www.sculpteo.com)
- **3D Hubs** / **Hubs** - [hubs.com](https://www.hubs.com)
- **Local makerspaces** - Search for nearby fablabs and maker spaces

#### Laser Cutting Services
- **Ponoko** - [ponoko.com](https://www.ponoko.com)
- **SendCutSend** - [sendcutsend.com](https://sendcutsend.com)
- **Local makerspaces** - Many offer laser cutter access

#### Design Inspiration
- **Thingiverse** - [thingiverse.com](https://www.thingiverse.com) (search: e-reader case, raspberry pi case)
- **Printables** - [printables.com](https://www.printables.com)
- **GrabCAD** - [grabcad.com](https://grabcad.com)
- **YouTube** - Search: "raspberry pi case design tutorial"

### Community Contributions

We encourage the community to design and share alternative cases:

- Fork the repository and add your design to `hardware/3d-models/community/`
- Include README with: materials, print settings, photos
- Submit pull request following [[CONTRIBUTING]] guidelines
- Designs will be credited and linked in this document

**Submission Format:**
```
hardware/3d-models/community/your-design-name/
├── README.md (description, materials, assembly)
├── model.stl (printable files)
├── model.source (SCAD/F3D/STEP source)
└── photos/ (assembly photos)
```

---

## Troubleshooting

### Common Issues and Solutions

#### Display Doesn't Fit in Window
- **Cause:** Window too small or display positioned incorrectly
- **Solution:** Carefully enlarge window with file, test-fit frequently

#### Buttons Are Hard to Press
- **Cause:** Holes too small, button actuators binding
- **Solution:** Enlarge holes slightly, ensure button moves freely

#### Case Doesn't Close Flush
- **Cause:** Wires preventing closure, component interference
- **Solution:** Improve wire routing, check for component height clearance

#### Screws Strip Threads in Plastic
- **Cause:** Over-tightening, screws too large
- **Solution:** Use heat-set inserts (3D print), or larger screw bosses

#### Display Falls Out of Front Panel
- **Cause:** Insufficient adhesive, no retention
- **Solution:** Add small clips or bezel overlap, use stronger adhesive

#### SD Card Inaccessible After Assembly
- **Cause:** Access hole misaligned or too small
- **Solution:** Mark position carefully before cutting, test-fit before final assembly

### Design Improvements

If you encounter issues, consider these design modifications:

- **Add Registration Features:** Small pins or tabs to align front/rear panels
- **Increase Tolerances:** Add 0.5mm clearance for easier assembly
- **Wire Management:** Add dedicated cable routing channels
- **Tool-Free Assembly:** Design snap-fit latches instead of screws
- **Modular Design:** Separable button panel for easier servicing

---

## Next Steps

After building your enclosure:

1. **Complete Assembly:** Follow [[BUILD_GUIDE]] for full device assembly
2. **Test Device:** Verify all functions work correctly
3. **Add Books:** Transfer e-books to device (see [[USER_GUIDE]])
4. **Share Your Build:** Post photos to community forums, contribute improvements
5. **Iterate Design:** Note what works well and what could be improved

### Future Enhancements

Consider these upgrades for your case:

- **Battery Integration:** Add cavity for LiPo battery and charge circuit
- **Kickstand:** Hinged or folding stand for hands-free reading
- **Carry Handle:** Integrated strap or handle
- **Waterproofing:** Gaskets and sealed buttons for outdoor use
- **Solar Panel:** Small solar cell for emergency charging
- **E-Ink Status Display:** Additional small e-ink for battery/status info

---

## Revision History

- **2026-01-17:** Initial case design documentation created
- Links to 3D model files and laser-cut templates to be added as they are developed

## Related Documentation

- [[BUILD_GUIDE]] - Complete assembly guide
- [[WIRING_GUIDE]] - Electrical connections
- [[BOM]] - Parts list
- [[BUTTON_LAYOUT]] - Button GPIO assignments
- [[DISPLAY_SPECS]] - Display dimensions and specifications
