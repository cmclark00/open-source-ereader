# Input System Design

Comprehensive design for physical buttons, text entry, and optional Bluetooth connectivity.

## Design Requirements

### Must-Have Features
1. ✅ Tactile page turn buttons (easy one-handed use)
2. ✅ Navigation controls (menu, library browsing)
3. ✅ Text input capability (for search, annotations)
4. ✅ Power button

### Nice-to-Have Features
1. ⭐ Bluetooth for wireless page turner
2. ⭐ Quick-access bookmark button
3. ⭐ Brightness control (if frontlight added)

## Button Layout Options

### Option 1: Minimal Asymmetric (RECOMMENDED)

Perfect for one-handed reading (right or left hand).

```
┌─────────────────────────┐
│  ┌──────────────────┐   │
│  │                  │   │
│  │                  │   │ [▲] Up
│  │                  │   │
│  │   6" E-Ink       │   │ [◄] Page Back
│  │   Display        │   │
│  │                  │   │ [●] Select/OK
│  │                  │   │
│  │                  │   │ [►] Page Forward
│  │                  │   │
│  │                  │   │ [▼] Down
│  └──────────────────┘   │
│                          │
│  [⌂ Home]      [⚙ Menu] │
│     [⏻ Power]           │
└─────────────────────────┘

Total buttons: 8
```

**Button functions**:
- **Page Forward/Back**: Primary reading controls (right side for thumb)
- **Up/Down**: Navigate lists, scroll
- **Select/OK**: Confirm selections, open books
- **Home**: Return to library
- **Menu**: Context menu
- **Power**: Sleep/wake/power off

**Pros**:
- Easy one-handed use (thumb reaches all buttons)
- Page buttons perfectly positioned
- Natural ergonomics
- Left-handed: flip device 180°

**Cons**:
- Requires 8 buttons
- No dedicated text input

### Option 2: Kindle-Style Side Buttons

```
┌──────────────────────────┐
│ [◄]  ┌──────────────┐    │
│ Page │              │    │
│ Back │              │    │
│      │              │    │
│ [◄]  │  6" E-Ink    │    │
│ Page │  Display     │    │
│ Back │              │    │
│      │              │    │
│      │              │    │
│      └──────────────┘    │
│                           │
│  [Home] [Menu] [Select]  │
│         [Power]           │
└──────────────────────────┘

+ Right side: [Page Forward ►] x2
```

**Pros**:
- Large page buttons (easy to hit without looking)
- Ambidextrous (buttons on both sides)
- Can use either thumb or multiple fingers

**Cons**:
- Requires more buttons (10+)
- More complex PCB routing
- Takes up display bezel space

### Option 3: D-Pad + Page Buttons (COMPACT)

```
┌─────────────────────────┐
│  ┌──────────────────┐   │
│  │                  │   │
│  │                  │   │
│  │   6" E-Ink       │   │
│  │   Display        │   │ [◄] Page Back
│  │                  │   │     (large)
│  │                  │   │ [►] Page Fwd
│  │                  │   │     (large)
│  └──────────────────┘   │
│                          │
│      ┌───┐               │
│      │ ▲ │               │
│    ┌─┼─●─┼─┐             │
│    │ │ │ │ │   D-Pad     │
│    └─┼───┼─┘             │
│      │ ▼ │               │
│      └───┘               │
│  [⌂]  [⚙]  [⏻]          │
└─────────────────────────┘

Total buttons: 7 (D-pad = 5-way, + 2 page + 3 function)
```

**Pros**:
- Compact design (fewer individual buttons)
- Familiar (like game controllers)
- D-pad good for text entry (on-screen keyboard)

**Cons**:
- D-pad can be less precise than separate buttons
- Requires specific D-pad component

### RECOMMENDED: Option 1 (Minimal Asymmetric)

Best balance of:
- One-handed usability
- Simple hardware
- Natural page turning
- Full navigation capability

## Text Input Solutions

### Challenge: E-readers without full keyboard need text entry for:
- Library search
- WiFi passwords (if added)
- Notes/annotations
- Dictionary lookup

### Solution 1: On-Screen Keyboard (RECOMMENDED)

**Implementation**:
```
Display on-screen QWERTY keyboard
Navigate with Up/Down/Left/Right buttons
Select letters with OK button
Show current text at top
```

**Example UI**:
```
┌─────────────────────────┐
│ Search: "Harry Po_"     │
├─────────────────────────┤
│ Q W E R T Y U I O P     │
│  A S D F G H J K L      │
│   Z X C V B N M         │
│     [Space] [Del]       │
│  [Cancel]  [Done]       │
└─────────────────────────┘
```

**Navigation**:
- Up/Down/Left/Right: Move between keys
- Select: Type current letter
- Long-press Select: Delete
- Menu: Toggle caps/numbers/symbols

**Pros**:
- No extra hardware needed
- Flexible (can change layout in software)
- Familiar QWERTY layout
- Works with existing buttons

**Cons**:
- Slower than physical keyboard
- Requires multiple button presses per letter
- Not great for long text entry

**Speed**: ~5-10 words per minute (acceptable for searches)

### Solution 2: T9-Style Predictive Text

Use 9-key phone-style input with prediction.

```
┌─────────────────────────┐
│ Search: "hello"         │
│ Suggestions:            │
│ > hello                 │
│   help                  │
│   held                  │
├─────────────────────────┤
│  1     2     3          │
│       ABC   DEF         │
│  4     5     6          │
│  GHI   JKL   MNO        │
│  7     8     9          │
│  PQRS  TUV   WXYZ       │
│  *     0     #          │
│       space             │
└─────────────────────────┘
```

**Map existing buttons**:
- Use number pad overlay on regular buttons
- Predictions appear as you type
- Much faster than on-screen keyboard

**Pros**:
- Faster text entry (one press per letter)
- Predictive text helps
- No on-screen keyboard needed

**Cons**:
- Requires dictionary/prediction database
- Not intuitive for younger users
- Harder to implement

### Solution 3: Mini Physical Keyboard

Add small physical keyboard below display.

**Options**:

**A) BlackBerry-style thumb keyboard**
```
┌─────────────────────────┐
│  ┌──────────────────┐   │
│  │   6" E-Ink       │   │
│  │   Display        │   │
│  └──────────────────┘   │
│                          │
│  [Q][W][E][R][T][Y]...  │
│  [A][S][D][F][G][H]...  │
│  [Z][X][C][V][B][N]...  │
└─────────────────────────┘
```

**Pros**:
- Fast, familiar text entry
- No on-screen UI needed
- Professional feel (like old BlackBerry)

**Cons**:
- Adds 30+ buttons
- Much more complex assembly
- Increases device thickness
- Harder to waterproof
- Higher cost ($5-10 more)

**B) Sliding keyboard**
- Slide-out QWERTY (like old phones)
- Hidden until needed
- Very complex mechanically

**Verdict**: Too complex for MVP

### Solution 4: External Bluetooth Keyboard

Support standard Bluetooth keyboards.

**Pros**:
- Full-speed typing when needed
- No hardware changes
- Users can choose their own keyboard
- Doesn't impact device design

**Cons**:
- Requires Bluetooth module
- Not always convenient to carry
- Additional accessory

### Solution 5: Voice Input

USB microphone for voice-to-text.

**Verdict**: Too complex, requires cloud processing or heavy local ML

### RECOMMENDED TEXT INPUT APPROACH

**Primary: On-screen keyboard with button navigation**
- Implement in software (no hardware cost)
- Good enough for book titles and search
- Can improve with predictive text later

**Future: Optional Bluetooth keyboard support**
- For power users who want faster input
- Requires Bluetooth module (see below)

**For MVP**: On-screen keyboard only

## Bluetooth Integration

### Use Cases
1. **Wireless page turner** (like foot pedal or remote)
2. **External keyboard** (for text entry)
3. **File transfer** (from phone/computer)

### Bluetooth Module Options

#### Option 1: ESP32 with Bluetooth (RECOMMENDED)

**Module**: ESP32-WROOM-32
- **Features**: WiFi + Bluetooth 4.2 + BLE
- **Interface**: UART to main SoC
- **Cost**: $2-3
- **Power**: 80mA active, <1mA sleep
- **Pros**:
  - Both WiFi and Bluetooth
  - Cheap and widely available
  - Good documentation
  - Can run simple tasks independently
- **Cons**:
  - Requires separate programming
  - More complex integration

#### Option 2: Bluetooth-only Module

**Module**: HC-05 or HC-06
- **Features**: Bluetooth 2.0 Classic
- **Interface**: UART
- **Cost**: $3-5
- **Power**: 50mA active
- **Pros**:
  - Simple AT command interface
  - Easy to use
  - Well documented
- **Cons**:
  - Older Bluetooth standard
  - No BLE (higher power)
  - No WiFi

#### Option 3: RTL8723BS (WiFi+BT combo chip)

**Chip**: Realtek RTL8723BS
- **Features**: WiFi + Bluetooth 4.0
- **Interface**: SDIO/UART
- **Cost**: $1.50-2 (chip only)
- **Linux Support**: Good mainline support
- **Pros**:
  - Cheaper in bulk
  - Single chip solution
  - Good Linux drivers
- **Cons**:
  - Requires antenna design
  - More complex PCB layout
  - Need certifications (FCC, CE)

### Bluetooth Page Turner Protocol

**Standard HID mode** (Human Interface Device):
```
Bluetooth page turner acts as keyboard:
- Right button → Right Arrow key
- Left button → Left Arrow key
```

Your device receives standard keyboard events - super simple!

**Compatible devices**:
- Any Bluetooth keyboard/remote
- Foot pedals (for accessibility)
- Wireless presenters
- Custom DIY remotes
- Phone apps (simulate keyboard)

### Bluetooth Implementation Plan

**Phase 1 - MVP (No Bluetooth)**:
- Focus on core reading with physical buttons
- Validate device concept
- Keep cost down

**Phase 2 - Add Bluetooth (Rev 2)**:
- Add ESP32 module to PCB
- Implement BT HID support
- Test with various controllers
- Add WiFi book downloads

**Why delay Bluetooth**:
1. Adds $2-3 to BOM
2. Increases power consumption
3. Requires FCC/CE testing for wireless
4. More complex software
5. Not essential for core reading

**Easy upgrade path**:
- Design PCB with ESP32 footprint
- Leave unpopulated for MVP
- Add later with firmware update

## Button Hardware Specifications

### Recommended Buttons

**For page turn buttons** (most-used):
**Part**: Omron B3F-4055
- **Size**: 6mm x 6mm x 4.3mm
- **Actuation Force**: 160gf (medium)
- **Travel**: 0.5mm
- **Life**: 1,000,000+ cycles
- **Sound**: Quiet click
- **Cost**: $0.25 each
- **Mount**: Through-hole (easier assembly)

**For navigation/function buttons**:
**Part**: Omron B3F-4005
- **Size**: 6mm x 6mm x 3.5mm (lower profile)
- **Actuation Force**: 160gf
- **Life**: 100,000+ cycles
- **Cost**: $0.15 each

**Alternative - Softer touch**:
**Part**: ALPS SKQGAFE010
- **Actuation Force**: 100gf (lighter, softer)
- **Life**: 100,000+ cycles
- **Cost**: $0.20 each

### Button Caps/Covers

Most tactile switches need caps for better feel.

**Options**:
1. **3D printed caps** - Custom shapes, labels
2. **Commercial caps** - E-Switch caps for 6mm buttons
3. **Silicone membrane** - Waterproof, soft feel

**For MVP**: 3D printed caps with printed labels

### Button Placement Ergonomics

**Page turn buttons** (most important):
```
Right edge of device:
- Forward button: 60-80mm from bottom (thumb natural position)
- Back button: 100-120mm from bottom (above forward)
- Both should be reachable without shifting grip
```

**Navigation buttons**:
```
Below display or bottom edge:
- Less frequently used, OK to be less accessible
- Clustered for easy identification
```

**Power button**:
```
Top edge or bottom edge:
- Slightly recessed to prevent accidental press
- Or require long-press (3 seconds) to power off
```

## Complete Input BOM

| Component | Part Number | Qty | Unit Price | Total |
|-----------|-------------|-----|------------|-------|
| Page Forward Button | Omron B3F-4055 | 1 | $0.25 | $0.25 |
| Page Back Button | Omron B3F-4055 | 1 | $0.25 | $0.25 |
| Navigation Buttons | Omron B3F-4005 | 4 | $0.15 | $0.60 |
| Function Buttons | Omron B3F-4005 | 3 | $0.15 | $0.45 |
| Button Caps (3D printed) | Custom STL | 8 | $0.05 | $0.40 |
| **Input Total (MVP)** | | | | **$1.95** |
| | | | | |
| **Optional - Bluetooth** | | | | |
| ESP32-WROOM-32 Module | Espressif | 1 | $2.50 | $2.50 |
| Antenna (PCB trace) | - | - | - | - |
| **Bluetooth Total** | | | | **$2.50** |

## Software Requirements

### Button Debouncing
```c
// Basic debounce in software
#define DEBOUNCE_MS 50

bool read_button(int gpio) {
    if (gpio_read(gpio) == PRESSED) {
        delay_ms(DEBOUNCE_MS);
        if (gpio_read(gpio) == PRESSED) {
            return true;
        }
    }
    return false;
}
```

### Long-Press Detection
```c
// Detect long press for power button
if (button_pressed_duration(POWER_BUTTON) > 3000) {
    power_off();
}
```

### Gesture Support (Future)
- Double-tap for bookmarks
- Hold page button for fast scroll
- Simultaneous press for combos (e.g., both page buttons = bookmark)

## UI Flow Examples

### Library Search with On-Screen Keyboard

```
1. User presses "Home" → Library view
2. User presses "Menu" → Shows search option
3. User selects "Search"
4. On-screen keyboard appears
5. Navigate with Up/Down/Left/Right
6. Press OK to type letter
7. Results filter in real-time
8. Press "Done" or "Menu" to exit keyboard
9. Select book from filtered results
```

### Quick Navigation During Reading

```
Reading page:
- Right button → Next page
- Left button → Previous page
- Menu → Options (bookmark, jump to page, settings)
- Home → Exit to library
- Up/Down → Scroll if text doesn't fit
```

### Bookmark Creation

```
Option 1: Menu-based
- Press Menu → Select "Add Bookmark" → Confirm

Option 2: Quick gesture (if implemented)
- Press both page buttons simultaneously → Bookmark added

Option 3: Dedicated button (if added in future)
- Single button press → Bookmark
```

## Accessibility Considerations

### One-Handed Use
- All essential buttons (page turn) on one side
- Device can be flipped 180° for left-handed users
- Buttons large enough for low dexterity

### Visual Impairment
- Tactile button differentiation (different shapes/sizes)
- Raised dots on primary buttons (like F/J keys)
- Audio feedback option (with speaker/headphones)

### Motor Impairment
- Bluetooth foot pedal support
- Adjustable button sensitivity (software)
- Large button targets

## Future Enhancements

### Phase 2 Features:
1. ✨ Bluetooth support (wireless page turner)
2. ✨ WiFi for book downloads
3. ✨ Capacitive touch overlay (optional upgrade)
4. ✨ Haptic feedback on button press
5. ✨ Voice control (via Bluetooth headset)

### Phase 3 Features:
1. 🚀 Predictive text for faster search
2. 🚀 Gesture shortcuts (double-tap, swipe)
3. 🚀 Customizable button mapping
4. 🚀 Phone app as remote control

## Summary & Recommendations

### MVP Input System:
- **8 physical buttons** (2 page + 4 nav + 2 function)
- **On-screen keyboard** for text entry
- **No Bluetooth** (keep it simple)
- **Cost**: ~$2 for all input hardware
- **Assembly**: Simple through-hole buttons

### Future Upgrades:
- **Add ESP32 module** for Bluetooth + WiFi
- **Optional external keyboard** support
- **Wireless page turner** compatibility

This design prioritizes:
✅ Excellent one-handed reading experience
✅ Simple, reliable hardware
✅ Low cost
✅ Easy assembly
✅ Future expandability

The on-screen keyboard is good enough for searching your library, and you can always add Bluetooth later if you want faster text entry or wireless page turning!
