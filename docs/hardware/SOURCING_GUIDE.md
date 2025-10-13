# Component Sourcing Guide

Updated guide for purchasing components with current recommendations.

## Quick Start Shopping List

For fastest path to testing (within 1 week in US):

| Item | Source | Price | Shipping |
|------|--------|-------|----------|
| Orange Pi Zero LTS | Amazon | $22-25 | 2-3 days |
| 5.83" E-ink display | Amazon (Waveshare) | $28-32 | 2-3 days |
| microSD card 16GB | Amazon | $5 | 2-3 days |
| **Total** | | **$55-62** | **Fast!** |

---

## Orange Pi Zero LTS - Best Sources

### Option 1: Amazon (FASTEST - Recommended for US)

**Search terms**: "Orange Pi Zero LTS" or "Orange Pi Zero 512MB"

**Sellers to look for**:
- Shenzhen Xunlong (official)
- Orange Pi official store
- Various electronics distributors

**Typical prices**:
- Board only: $18-22
- With power adapter: $25-28
- With case: $28-32

**Pros**:
- 2-3 day Prime shipping
- Easy returns
- Customer protection
- No import hassles

**Cons**:
- ~20-30% markup over direct pricing

**What to order**:
- Orange Pi Zero LTS 512MB version
- Just the board is fine (you'll provide your own power)
- Don't need case or accessories

**Direct Amazon link format**:
Search: `Orange Pi Zero LTS 512MB`
Check: Reviews (should be 4+ stars)
Verify: 512MB RAM version (not 256MB)

---

### Option 2: AliExpress Official Store (CHEAPEST)

**Store**: Orange Pi Official Store on AliExpress

**Price**: $15-18 (board only)

**Shipping**:
- Standard: 2-4 weeks (free)
- Express: 7-10 days ($5-8)

**Pros**:
- Cheapest price
- Official store (authentic parts)
- Good buyer protection

**Cons**:
- Long shipping (standard)
- Customs possible
- Returns difficult

**When to use**: If budget is tight and you can wait

**Link**: Search "Orange Pi Zero LTS" on aliexpress.com
Filter by: Orange Pi Official Store

---

### Option 3: Direct from Orange Pi

**Website**: orangepi.com or orangepi.org

**Price**: $15-18

**Shipping**: Varies by country, 2-4 weeks typical

**Pros**:
- Direct from manufacturer
- Guaranteed authentic
- Bulk discounts available

**Cons**:
- Slow international shipping
- Minimum order quantities for some items
- Website can be confusing

**When to use**: For bulk orders (10+ boards)

---

### Alternative: Raspberry Pi Zero 2 W

If Orange Pi is out of stock:

**Where**: Any major retailer
- Amazon: $20-25
- Adafruit: $15 (if in stock)
- MicroCenter: $15 (in-store)

**Why it's good**:
- More powerful (quad-core)
- Better documentation
- WiFi/Bluetooth built-in
- Easier to get

**Why it's not ideal**:
- More expensive for production
- Uses different GPIO pinout
- Overkill for e-reader

**Use if**: Can't find Orange Pi and want to start immediately

---

## E-Ink Display - Best Sources

### For 5.83" Display (Starting/Testing)

#### Option 1: Amazon - Waveshare (FASTEST)

**Search**: "Waveshare 5.83 inch e-Paper HAT"

**Part numbers**:
- Waveshare 5.83inch e-Paper HAT (black/white)
- SKU: Multiple variants available

**Price**: $28-35

**Shipping**: 2-3 days (Prime)

**What you get**:
- Display module
- Cable (usually pre-attached)
- Sometimes includes sample code
- Mounting hardware

**Pros**:
- Fast shipping
- Easy returns
- Good documentation
- Tested and working

**Cons**:
- More expensive than direct
- May be older stock

**Recommended**: Yes, for first prototype

---

#### Option 2: Waveshare Official Website

**Website**: waveshare.com

**Price**: $25-30

**Shipping**:
- US warehouse: 5-7 days
- China warehouse: 2-3 weeks

**Pros**:
- Official source
- Good documentation
- Technical support
- Latest firmware

**Cons**:
- Slower than Amazon
- Shipping costs extra

---

#### Option 3: Buydisplay.com

**Website**: buydisplay.com

**Specialization**: E-ink displays direct from manufacturers

**Price**: $20-28 (good deals on bulk)

**Shipping**: 1-2 weeks from China warehouse

**Pros**:
- Good prices
- Wide selection
- Can request custom specs
- Technical data available

**Cons**:
- Longer shipping
- Less familiar brand
- Returns more difficult

**When to use**: For final production display or bulk orders

---

### For 6" Display (Final Build)

#### Option 1: Amazon - Waveshare 6"

**Search**: "Waveshare 6 inch e-Paper" or "GDEW060"

**Available options**:

**Standard 800x600**:
- Waveshare 6inch e-Paper HAT
- Price: $30-38
- Good availability

**High-res 1448x1072**:
- Waveshare 6inch HD e-Paper
- Price: $50-60
- Less common on Amazon

**Pros**: Fast shipping, returnable

**Cons**: Limited selection of high-res version

---

#### Option 2: Good Display (Manufacturer Direct)

**Website**: good-display.com or via AliExpress store

**Models**:
- GDEW060T7 (800x600) - $25-30
- GDEY060F51 (1448x1072) - $45-50

**Pros**:
- Direct from manufacturer (OEM for Waveshare)
- Cheaper than retail
- Can get datasheets easily
- Bulk pricing available

**Cons**:
- 2-4 week shipping
- MOQ may apply (often 1 is OK)
- Less documentation than Waveshare

**When to use**: For production runs or if comfortable with datasheets

---

#### Option 3: LCSC (Bulk Orders)

**Website**: lcsc.com

**Selection**: Various e-ink displays from multiple manufacturers

**Price**: Competitive, especially in quantity

**Shipping**: 1-2 weeks express, 3-4 weeks standard

**Pros**:
- Good for ordering all components at once
- Real component distributor
- Good stock levels
- Can get unusual sizes

**Cons**:
- Shipping costs add up for single items
- Better for complete BOM orders

---

## Recommended Purchase Strategy

### Phase 1: Quick Start Testing ($60, 1 week)

**Order from Amazon**:
1. Orange Pi Zero LTS - $22
2. Waveshare 5.83" display - $30
3. 16GB microSD card - $5
4. Micro USB cable (if needed) - $3

**Total**: ~$60 with Prime shipping

**Why**: Get started immediately, test software, validate concept

---

### Phase 2: Upgrade to Full Size ($35, 1-2 weeks)

**After software works, order**:
1. 6" e-ink display - $30-35 (Amazon or direct)

**Why**: Move to final display size once you know it works

---

### Phase 3: PCB and Components ($45, 2-3 weeks)

**Order from JLCPCB/LCSC**:
1. Custom PCB (5x) - $2 + $15 shipping
2. Electronic components kit - $20
3. Connectors and misc - $8

**Total**: ~$45

**Why**: One combined order saves on shipping

---

### Phase 4: Batteries and Final Parts ($20, 1 week)

**Order from Amazon**:
1. 2x 18650 cells (LG, Samsung, Panasonic) - $8-12
2. TP4056 charging modules - $8
3. Misc wires, connectors - $5

**Total**: ~$20

---

## Battery Sourcing (IMPORTANT)

### Recommended Brands Only:
- Samsung (INR18650-35E, 30Q)
- LG (INR18650 MJ1, HG2)
- Panasonic (NCR18650B)
- Sony (VTC6)

### Where to Buy:

#### Option 1: Specialized Battery Stores (SAFEST)

**US Stores**:
- 18650batterystore.com
- liionwholesale.com
- imrbatteries.com

**Pros**:
- Authentic cells guaranteed
- Safety tested
- Good customer service
- Proper packaging

**Cons**:
- More expensive ($4-6 per cell)

**Recommendation**: Best choice for genuine cells

---

#### Option 2: Amazon (CAUTION)

**Search**: "Samsung 30Q 18650" or "LG HG2 18650"

**WARNINGS**:
- Many counterfeits on Amazon
- Check seller reviews carefully
- Look for "Ships from and sold by Amazon"
- Avoid too-good prices (real cells are $5-8 each)

**How to spot fakes**:
- Price too low (< $4 per cell)
- No brand markings or wrong font
- Suspiciously high mAh ratings (>3500mAh is fake)
- Seller has mixed reviews

**Safer option**: Buy button-top protected cells from known sellers

---

#### Option 3: Local Vape Shops

**Pros**:
- Can inspect before buying
- Usually authentic (their reputation matters)
- Immediate availability
- Can ask questions

**Cons**:
- Higher prices
- Limited selection

**Good for**: Quick local option if needed

---

### ⚠️ DO NOT BUY BATTERIES FROM:
- eBay (too many fakes)
- Random AliExpress sellers
- Wish.com
- Ultra-cheap Amazon sellers
- Any "Fire" branded cells
- Cells claiming >3500mAh

---

## Complete Shopping Links Template

### Amazon Shopping Cart (US)

```
Search and add to cart:

1. "Orange Pi Zero LTS 512MB"
   → Look for: Official store or high-rated seller
   → Price: $20-25

2. "Waveshare 5.83 inch e-Paper HAT"
   → Look for: Waveshare seller or Amazon direct
   → Price: $28-35

3. "SanDisk 16GB microSD Card"
   → Any Class 10 or A1 rated
   → Price: $5-7

4. "Samsung 30Q 18650" (later, when ready)
   → Look for: Authentic seller, button-top protected
   → Price: $12-16 for 2 cells

Total Phase 1: ~$60
```

---

## International Sourcing

### Europe

**Orange Pi**:
- Amazon.de / Amazon.co.uk
- Aliexpress (Orange Pi store)
- Some local electronics distributors

**E-ink displays**:
- Amazon EU sites
- Waveshare EU warehouse
- Reichelt Elektronik (Germany)

**Components**:
- Digikey (Europe warehouses)
- Mouser (Europe warehouses)
- RS Components

---

### Asia-Pacific

**Orange Pi**:
- Local Taobao/JD.com (China)
- Lazada/Shopee (Southeast Asia)
- Direct from Shenzhen suppliers

**E-ink displays**:
- Taobao (many suppliers)
- Local electronics markets
- Alibaba for bulk

---

### Other Regions

**Orange Pi / Components**:
- Check local electronics distributors first
- AliExpress for direct shipping
- Digikey/Mouser (worldwide shipping, expensive)

---

## Bulk Order Discounts

### 10+ Units

**Orange Pi**: ~$15 each (direct or Aliexpress)
**Display 6"**: ~$25 each (Good Display direct)
**PCBs**: ~$1 each (JLCPCB)
**Components**: ~30% discount (LCSC bulk)

**Total per unit**: ~$55-60

---

### 100+ Units

**Orange Pi**: ~$12-14 each (MOQ negotiations)
**Display**: ~$22-25 each (factory direct)
**PCBs with assembly**: ~$10 each (PCBA service)
**Components**: ~40% discount

**Total per unit**: ~$45-50

Contact manufacturers directly for quotes

---

## Quality Verification

### When Components Arrive

**Orange Pi**:
- [ ] Check serial number on board
- [ ] Verify 512MB RAM (not 256MB)
- [ ] Test boot with simple SD card image
- [ ] Check all GPIO pins for continuity

**E-ink Display**:
- [ ] Inspect for physical damage
- [ ] Check FPC cable connections
- [ ] Verify model number matches
- [ ] Run manufacturer's test code
- [ ] Test refresh rate and ghosting

**Batteries**:
- [ ] Check for authentic markings
- [ ] Measure voltage (should be 3.6-3.7V)
- [ ] Weigh cells (should match spec ~45-48g)
- [ ] Check for damage or dents
- [ ] Test capacity with load tester (if available)

---

## Summary: Best Buy Today

**For US buyers wanting to start immediately**:

**Order NOW from Amazon**:
1. Orange Pi Zero LTS - ~$22
2. Waveshare 5.83" e-Paper - ~$30
3. SanDisk 16GB microSD - ~$5

**Total: $57 with 2-3 day delivery**

This gets you everything to start software development and validate the concept. Order other components once you've tested this!

---

## Files

**Supplier List Spreadsheet**: `/hardware/bom/suppliers.csv` (coming soon)
**Part Number Cross-Reference**: `/hardware/bom/part_numbers.md` (coming soon)

---

**Last Updated**: 2024-01-XX
**Prices are approximate and may vary**
**Always check seller reviews before purchasing**
