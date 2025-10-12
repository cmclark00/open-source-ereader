# E-Ink Display Comparison for Trade Paperback E-Reader

Quick reference guide comparing e-ink displays suitable for a trade paperback-sized device.

## Size Context: Physical Books

| Book Type | Typical Size | Best Display Match |
|-----------|--------------|-------------------|
| Mass Market Paperback | 4.25" x 6.87" (108 x 175mm) | 4.3"-5" e-ink |
| Trade Paperback | 5.5" x 8.5" (140 x 216mm) | 6"-7" e-ink |
| Hardcover Novel | 6" x 9" (152 x 229mm) | 7"-7.8" e-ink |

## Display Options Ranked

### 🥇 Best Choice: 6" High-Res E-Ink

**Model**: Waveshare 6" E-Paper HAT (1448x1072)
- **PPI**: 265 (very crisp, exceeds printed books)
- **Refresh**: 450ms full, 280ms partial
- **Technology**: E-Ink Carta 1200 (latest)
- **Active Area**: 122.4mm x 90.6mm
- **Cost**: $50-55
- **Good Display part**: GDEY060F51

**Why this is best**:
- Text clarity matches high-quality paperback printing
- Fast enough refresh for natural page turns (0.3-0.5s)
- Perfect single-column text size
- Carta 1200 has excellent contrast (16:1)
- Industry standard size (same as Kindle Paperwhite)

**Reading experience**: Feels like a premium paperback

---

### 🥈 Budget Alternative: 6" Standard E-Ink

**Model**: Good Display 6" (800x600)
- **PPI**: 167 (acceptable for text)
- **Refresh**: 800ms full, 500ms partial
- **Technology**: E-Ink Pearl or Carta (older)
- **Active Area**: 122.4mm x 90.6mm
- **Cost**: $25-35
- **Part**: GDEW060T7, GDEW060C01

**Why consider this**:
- Much cheaper ($20-25 savings)
- Same physical size as high-res version
- 167 PPI acceptable for novels (better than early Kindles)
- Easier to drive (simpler waveforms)

**Reading experience**: Feels like a standard paperback
**Trade-off**: Text slightly less crisp, slower page turns

---

### 🥉 Compact Option: 5.83" E-Ink

**Model**: Waveshare 5.83" E-Paper HAT (648x480)
- **PPI**: 150
- **Refresh**: 2000ms full, 500ms partial
- **Technology**: E-Ink Spectra or monochrome
- **Active Area**: 118.8mm x 88.26mm
- **Cost**: $25-30

**Why consider this**:
- More affordable
- Lighter device
- Lower power consumption
- Easier to hold one-handed

**Reading experience**: Feels like a small paperback
**Trade-off**: Smaller text, not great for PDFs, slower refresh

---

### 🏅 Premium Option: 7.8" E-Ink (Future Version)

**Model**: Waveshare 7.8" (1872x1404)
- **PPI**: 300 (matches print)
- **Refresh**: 500ms full, 300ms partial
- **Technology**: E-Ink Carta 1100
- **Active Area**: 158mm x 118.5mm
- **Cost**: $80-100

**Why this is premium**:
- Perfect for PDFs and technical books
- Larger text for accessibility
- 300 PPI is print quality
- More comfortable for long reading sessions

**Reading experience**: Feels like a hardcover book
**Trade-off**: Expensive, larger/heavier, exceeds budget

---

## Refresh Rate Deep Dive

### What refresh rates mean for reading:

| Refresh Type | Time | Use Case | Perception |
|-------------|------|----------|------------|
| **Full Refresh** | 400-2000ms | Every 5-10 pages | Brief black flash, eliminates ghosting |
| **Partial Refresh** | 200-500ms | Normal page turns | Smooth transition, natural feel |
| **Fast Mode (A2)** | 100-200ms | Scrolling, menus | Instant, but 1-bit (no grays) |
| **Ultra-fast** | <100ms | Animations | Not suitable for reading |

### Our recommended refresh strategy:

```
Reading a novel:
├─ Turn page: Partial refresh (280ms)
├─ Turn page: Partial refresh (280ms)
├─ Turn page: Partial refresh (280ms)
├─ Turn page: Partial refresh (280ms)
└─ Turn page: Full refresh (450ms) ← Clear ghosting

Navigating library:
└─ Fast mode (120ms) ← Responsive UI

Reading PDFs:
└─ Full refresh always (450ms) ← Best quality
```

### Comparison to physical books:

- Turning a physical page: ~300-500ms
- Our partial refresh: 280ms
- **Result**: Feels natural, very close to physical book

### Comparison to other e-readers:

| Device | Refresh Time | Notes |
|--------|--------------|-------|
| Kindle Paperwhite | 300-450ms | Similar to our target |
| Kobo Clara | 400-500ms | Slightly slower |
| reMarkable 2 | 21ms | Special low-latency display, $$$$ |
| Our 6" high-res | 280-450ms | Competitive with commercial |

## Text Clarity: PPI Comparison

### PPI vs. Reading Experience

| PPI | Quality | Use Case | Example |
|-----|---------|----------|---------|
| **300+** | Print quality | Any content | Hardcover books, our 7.8" premium |
| **250-300** | Excellent | Novels, comics | Our recommended 6" (265 PPI) |
| **200-250** | Very good | Novels | High-end displays |
| **150-200** | Good | Text-heavy novels | Our budget 6" (167 PPI) |
| **100-150** | Acceptable | Large text only | Our 5.83" (150 PPI) |
| **<100** | Poor | Not recommended | Cheap displays |

### Typical print books:
- Paperback novels: ~300-350 PPI
- Newspapers: ~200 PPI
- Mass market paperbacks: ~250-300 PPI

**Our 6" high-res at 265 PPI**: Indistinguishable from printed paperback at normal reading distance.

## Power Consumption Comparison

E-ink only uses power when refreshing:

| Display | Full Refresh | Partial Refresh | Static Display |
|---------|--------------|-----------------|----------------|
| 6" high-res | 100mA @ 450ms | 80mA @ 280ms | 0mA |
| 6" standard | 90mA @ 800ms | 70mA @ 500ms | 0mA |
| 5.83" | 80mA @ 2000ms | 60mA @ 500ms | 0mA |

**Battery life calculation** (3000mAh battery):

```
Assumptions:
- 30 minutes reading per day
- 1 page turn per minute = 30 page turns/day
- Partial refresh: 80mA for 280ms

Energy per page turn: 80mA × 0.28s = 22.4 mAs = 0.00622 mAh
Daily energy: 30 turns × 0.00622 mAh = 0.187 mAh
Days on battery: 3000 mAh / 0.187 mAh = 16,000+ days

Actual battery life (accounting for system power):
- System idle: 50mA average
- Reading time: 0.5 hours/day
- System active energy: 50mA × 0.5h = 25 mAh/day
- Display energy: ~0.2 mAh/day
- Total: ~25 mAh/day
- Battery life: 3000/25 = 120 days (4 months!)

More realistic (standby drain):
- Deep sleep: 5mA
- Sleep 23.5h/day: 5mA × 23.5h = 117.5 mAh/day
- Active: 25 mAh/day
- Total: ~142.5 mAh/day
- Battery life: 3000/142.5 = 21 days (3 weeks)
```

**Conclusion**: Display choice has minimal impact on battery life. CPU/system power dominates.

## Final Recommendation

### For $50 budget MVP: **6" Standard Res (800x600)**
- Good Display GDEW060T7
- $30-35
- 167 PPI (good enough)
- Perfect paperback size
- Leaves budget for other components

### For $60-70 build: **6" High Res (1448x1072)**
- Waveshare 6" HAT or GDEY060F51
- $50-55
- 265 PPI (excellent)
- Fast refresh (280ms partial)
- Best reading experience

### For ultra-budget (<$40): **5.83" Display**
- Waveshare 5.83" HAT
- $25-30
- 150 PPI (acceptable)
- Smaller but cheaper
- Good for novels only

## Where to Buy

### US/Europe (Fast shipping):
1. **Waveshare Official Store** (Amazon/eBay)
   - Fast shipping (2-7 days)
   - Good documentation
   - Higher prices (+20-30%)

2. **Digikey/Mouser** (for Good Display parts)
   - Reliable
   - Fast shipping
   - Stock can be limited

### Global (Best prices):
1. **AliExpress** (direct from China)
   - Cheapest prices
   - 2-4 week shipping
   - Good Display official store available

2. **Buydisplay.com**
   - Direct from manufacturer
   - Bulk pricing available
   - Custom options

### Recommended strategy:
1. **For prototyping**: Buy Waveshare from Amazon (fast, returnable)
2. **For production**: Buy Good Display from AliExpress/direct (bulk pricing)

## Display Driver Notes

All these displays use similar interfaces:
- **SPI** (Serial Peripheral Interface)
- **4-wire** or **3-wire** mode
- **Busy pin** to indicate refresh complete
- **Reset pin** for initialization

### Driver availability:
- Waveshare: Excellent examples (C, Python, Arduino)
- Good Display: Good documentation, reference code
- Linux fbdev drivers: Some available, may need adaptation

### Development approach:
1. Start with Waveshare examples
2. Port to Linux framebuffer driver
3. Implement partial update
4. Optimize waveforms for speed

## Summary Table

| Display | Size | PPI | Refresh | Cost | Best For |
|---------|------|-----|---------|------|----------|
| Waveshare 6" HR | 6.0" | 265 | 280ms | $50 | Premium reading |
| Good Display 6" | 6.0" | 167 | 500ms | $30 | Budget build |
| Waveshare 5.83" | 5.83" | 150 | 500ms | $28 | Compact/cheap |
| Waveshare 7.8" | 7.8" | 300 | 300ms | $90 | PDFs/premium |

**Winner for trade paperback feel**: 6" display (either resolution) hits the sweet spot for size, readability, and cost.
