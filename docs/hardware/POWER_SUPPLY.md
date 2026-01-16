# Power Supply and Battery Monitoring

This document describes power supply options and battery monitoring capabilities for the open-source e-reader project.

## Table of Contents

1. [Power Supply Options](#power-supply-options)
2. [Battery Monitoring Hardware](#battery-monitoring-hardware)
3. [Software Integration](#software-integration)
4. [Hardware Setup Guides](#hardware-setup-guides)
5. [Troubleshooting](#troubleshooting)

---

## Power Supply Options

The Raspberry Pi Zero W can be powered in several ways for portable e-reader use:

### Option 1: USB Power (No Battery Monitoring)

**Simplest option - no additional hardware required**

- **Setup**: Connect 5V 2A micro-USB power supply directly to Pi Zero W
- **Pros**:
  - No additional hardware needed
  - Reliable, stable power
  - Perfect for development/testing
- **Cons**:
  - Not portable (requires wall outlet)
  - No battery monitoring capability
- **Software Behavior**: Battery monitor will run in "dummy mode" and always report 100%

### Option 2: USB Power Bank (No Battery Monitoring)

**Portable, but limited monitoring**

- **Setup**: Connect USB power bank (10,000+ mAh recommended) to Pi Zero W
- **Typical Runtime**: 10-20 hours depending on power bank capacity
- **Pros**:
  - Very simple, no soldering required
  - Long battery life with large power banks
  - Can swap power banks easily
- **Cons**:
  - No software battery monitoring (can't read remaining charge)
  - Power banks may auto-shutoff with Pi's low current draw
  - Bulkier than integrated battery solution
- **Software Behavior**: Battery monitor runs in "dummy mode"
- **Recommendation**: Use power banks that support low-current devices

### Option 3: Battery HAT/UPS with I2C Monitoring

**Best option for integrated battery with software monitoring**

This is the recommended approach for a production e-reader device.

#### Recommended Hardware Options

##### **Waveshare UPS HAT (C)** ⭐ Recommended

- **Product**: Waveshare UPS HAT (C) for Raspberry Pi Zero
- **Features**:
  - Built-in Li-po battery switching charger with path management
  - Voltage boost chip for stable 5V output
  - I2C voltage/current monitor (supports real-time monitoring)
  - Multi-battery protection: over-charge, over-discharge, over-current, short-circuit
  - Recharging and regulated output can work simultaneously
  - Compatible with Pi Zero W and Pi Zero 2W
- **Battery Runtime**: 6-7 hours with Pi Zero WH (standby mode, no peripherals)
- **I2C Communication**: Built-in monitoring via I2C bus
- **Cost**: ~$20-30 USD
- **Where to Buy**:
  - [Waveshare Official](https://www.waveshare.com/ups-hat-c.htm)
  - [The Pi Hut](https://thepihut.com/products/uninterruptible-power-supply-ups-hat-for-raspberry-pi-zero)
  - [Amazon](https://www.amazon.com/Waveshare-UPS-HAT-Uninterruptible-Protection/dp/B093SYMRQG)

**Software Support**: Our battery monitor automatically detects and supports INA219-based power monitors

##### **WatangTech Pi Zero UPS HAT (A)**

- **Product**: WatangTech Pi Zero UPS HAT with INA219
- **Features**:
  - Built-in 1000mAh Li-ion battery with protection circuits
  - INA219 chip for accurate voltage/current/power readings via I2C
  - LED indicators for power and charging status
  - Integrated battery with over-charge, over-discharge, short-circuit protection
- **Battery Runtime**: ~4-6 hours with 1000mAh battery
- **I2C Communication**: INA219 at default address 0x40
- **Cost**: ~$25-35 USD
- **Where to Buy**: [Amazon](https://www.amazon.com/WatangTech-Zero-UPS-HAT-Uninterruptible/dp/B0FS1F7CSQ)

**Software Support**: Fully supported by our INA219 backend

##### **DFRobot UPS HAT for Raspberry Pi Zero**

- **Product**: DFRobot SKU: DFR0528
- **Features**:
  - Solar charging support (5V-24V input)
  - Battery monitoring via I2C
  - LED indicators
  - Low-power UPS expansion board
- **Cost**: ~$20-30 USD
- **Where to Buy**: [DFRobot](https://www.dfrobot.com/)

**Software Support**: Check I2C chip compatibility (likely INA219 or compatible)

#### I2C Communication Details

Battery HATs with I2C monitoring typically use one of these chips:

- **INA219**: Power monitor IC, measures voltage, current, and power
  - Default I2C addresses: 0x40, 0x41, 0x44, 0x45
  - 12-bit ADC resolution
  - ±3.2A current sensing range (with 0.1Ω shunt resistor)
  - Fully supported by our software

- **LC709203F**: Battery fuel gauge IC (some advanced HATs)
  - I2C address: 0x0B
  - More accurate battery percentage calculation
  - Temperature compensation
  - Not yet implemented (TODO for future)

### Option 4: DIY Battery Solution with ADC Monitoring

**For advanced users who want custom battery configuration**

#### Hardware Required

1. **Battery**: Single-cell LiPo/Li-ion (3.7V nominal, 2000-5000mAh recommended)
2. **Charging/Protection Circuit**: TP4056 charging module or similar
3. **Boost Converter**: 3.7V to 5V boost (e.g., Adafruit PowerBoost 1000C)
4. **ADC Chip** (for voltage monitoring): MCP3008 or MCP3002
5. **Voltage Divider**: Two resistors (equal values, e.g., 10kΩ each) for 1:1 division

#### Wiring Diagram

```
LiPo Battery (3.7V)
    |
    ├─→ TP4056 Charger Module (charges battery via USB)
    |
    ├─→ Boost Converter (3.7V → 5V) → Pi Zero W (5V power input)
    |
    └─→ Voltage Divider (R1 + R2) → MCP3008 ADC CH0 → Pi SPI
```

#### Voltage Divider Calculation

To safely read battery voltage (0-4.2V) with Pi's 3.3V ADC reference:

- **R1 = 10kΩ** (battery positive to divider midpoint)
- **R2 = 10kΩ** (divider midpoint to ground)
- **Divider ratio**: 2.0 (halves the voltage)
- **Max voltage at ADC**: 4.2V ÷ 2 = 2.1V (safe for 3.3V ADC)

Connect divider midpoint to MCP3008 channel 0.

#### MCP3008 Wiring to Pi Zero W

| MCP3008 Pin | Pi Zero W GPIO | Function |
|-------------|----------------|----------|
| VDD (16)    | 3.3V (Pin 1)   | Power    |
| VREF (15)   | 3.3V (Pin 1)   | Reference voltage |
| AGND (14)   | GND (Pin 9)    | Ground   |
| DGND (9)    | GND (Pin 9)    | Ground   |
| CLK (13)    | GPIO 11 (Pin 23) | SPI Clock |
| DOUT (12)   | GPIO 9 (Pin 21) | SPI MISO |
| DIN (11)    | GPIO 10 (Pin 19) | SPI MOSI |
| CS (10)     | GPIO 8 (Pin 24) | SPI CE0  |
| CH0 (1)     | Voltage Divider | Battery voltage input |

#### Software Configuration

Configure battery monitor for MCP3008:

```c
battery_config_t config = battery_monitor_get_default_config();
config.adc_channel = 0;  /* MCP3008 channel 0 */
config.voltage_divider_ratio = 2.0;  /* 1:1 resistor divider */
config.voltage_full = 4200.0;   /* 4.2V = 100% for LiPo */
config.voltage_empty = 3200.0;  /* 3.2V = 0% (don't go below this) */
config.voltage_critical = 3300.0; /* 3.3V = critical, shutdown soon */

battery_monitor_state_t *battery = battery_monitor_init(&config);
```

**Software Support**: MCP3008 backend is fully implemented with auto-detection

---

## Battery Monitoring Hardware

### Supported Hardware Backends

Our battery monitoring software (`src/ereader/power/battery_monitor.c`) supports:

| Backend | Hardware | Detection Method | Features |
|---------|----------|------------------|----------|
| **INA219** | I2C power monitor | Auto-detect at 0x40, 0x41, 0x44, 0x45 | Voltage, current, power, charging detection |
| **MCP3008** | 10-bit SPI ADC | Detect /dev/spidev0.0 | Voltage only (via divider) |
| **MCP3002** | 10-bit SPI ADC | Detect /dev/spidev0.0 | Voltage only (via divider) |
| **Dummy Mode** | None | Fallback if no hardware | Always reports 100% |

### Auto-Detection Priority

1. **First**: Check I2C bus for INA219 (addresses 0x40, 0x41, 0x44, 0x45)
2. **Second**: Check SPI bus for MCP3008/MCP3002
3. **Fallback**: Use dummy mode (no hardware)

### Battery Status Information

The battery monitor provides:

- **Voltage** (mV): Current battery voltage
- **Current** (mA): Current draw (INA219 only)
- **Power** (mW): Power consumption (INA219 only)
- **Percentage** (0-100%): Estimated battery charge level
- **Is Charging**: Boolean flag (INA219 only, detects negative current)
- **Is Critical**: Boolean flag (voltage below critical threshold)

---

## Software Integration

### Basic Usage

```c
#include "ereader/power/battery_monitor.h"

/* Initialize with default config (auto-detects hardware) */
battery_monitor_state_t *battery = battery_monitor_init(NULL);

/* Main loop */
while (running) {
    /* Update battery status (respects update interval, default 60s) */
    battery_monitor_update(battery);

    /* Get current status */
    battery_status_t status = battery_monitor_get_status(battery);

    if (status.is_valid) {
        printf("Battery: %d%% (%.0fmV)\n",
               status.percentage,
               status.voltage_mv);

        /* Check if critically low */
        if (battery_monitor_is_critical(battery)) {
            printf("WARNING: Battery critically low! Saving and shutting down...\n");
            /* Save state and initiate shutdown */
        }

        /* Check if charging */
        if (battery_monitor_is_charging(battery)) {
            printf("Battery is charging\n");
        }
    }

    /* Your application logic here */
    sleep(1);
}

/* Cleanup */
battery_monitor_cleanup(battery);
```

### Custom Configuration

```c
battery_config_t config = battery_monitor_get_default_config();

/* Customize for your battery type */
config.voltage_full = 4200.0;      /* 4.2V for single-cell LiPo */
config.voltage_empty = 3200.0;     /* 3.2V minimum safe voltage */
config.voltage_critical = 3300.0;  /* Warn/shutdown at 3.3V */
config.update_interval_ms = 30000; /* Update every 30 seconds */

/* For MCP3008 with voltage divider */
config.adc_channel = 0;
config.voltage_divider_ratio = 2.0;  /* 1:1 resistor divider */

battery_monitor_state_t *battery = battery_monitor_init(&config);
```

### Display Battery Icon in Status Bar

To display battery percentage in the e-reader status bar:

```c
/* In reader.c status bar rendering */
battery_status_t bat_status = battery_monitor_get_status(app->battery);

if (bat_status.is_valid) {
    char bat_str[16];

    /* Show percentage and charging indicator */
    if (battery_monitor_is_charging(app->battery)) {
        snprintf(bat_str, sizeof(bat_str), "BAT:%d%%+", bat_status.percentage);
    } else {
        snprintf(bat_str, sizeof(bat_str), "BAT:%d%%", bat_status.percentage);
    }

    /* Render in status bar */
    text_renderer_draw_string(renderer, bat_str, x, y);
}
```

---

## Hardware Setup Guides

### Setup Guide: Waveshare UPS HAT (C)

**Step 1**: Attach UPS HAT to Pi Zero W
- Align the 40-pin header with Pi Zero W GPIO
- Press down firmly to ensure good connection

**Step 2**: Connect battery (if not included)
- Use JST connector to attach 3.7V LiPo battery to HAT
- Polarity: Red = positive, Black = negative

**Step 3**: Enable I2C on Raspberry Pi
```bash
# On Raspberry Pi, edit /boot/config.txt
sudo nano /boot/config.txt

# Add or uncomment:
dtparam=i2c_arm=on

# Reboot
sudo reboot
```

**Step 4**: Test I2C detection
```bash
# Install i2c-tools
sudo apt-get install i2c-tools

# Scan I2C bus
sudo i2cdetect -y 1

# Should show device at 0x40 (or similar)
```

**Step 5**: Run e-reader application
- Battery monitor will auto-detect INA219
- Check logs for "Found INA219 at I2C address 0x40"

### Setup Guide: MCP3008 ADC (DIY Solution)

**Step 1**: Wire MCP3008 to Pi Zero W (see wiring table above)

**Step 2**: Build voltage divider circuit
```
Battery(+) ──[10kΩ R1]── Midpoint ──[10kΩ R2]── GND
                            |
                            └───→ MCP3008 CH0
```

**Step 3**: Enable SPI on Raspberry Pi
```bash
# Edit /boot/config.txt
sudo nano /boot/config.txt

# Add or uncomment:
dtparam=spi=on

# Reboot
sudo reboot
```

**Step 4**: Test SPI device
```bash
# Check if SPI device exists
ls -l /dev/spidev0.0

# Should show: /dev/spidev0.0
```

**Step 5**: Configure battery monitor
```c
battery_config_t config = battery_monitor_get_default_config();
config.adc_channel = 0;
config.voltage_divider_ratio = 2.0;  /* For 1:1 divider (10k:10k) */
```

**Step 6**: Run e-reader application
- Battery monitor will detect MCP3008 on SPI bus
- Check logs for "Found SPI device, assuming MCP3008 ADC"

---

## Troubleshooting

### Battery Monitor Reports "No hardware detected"

**Symptoms**: Console shows "Battery monitor: No hardware detected, using dummy mode"

**Causes & Solutions**:

1. **I2C not enabled**
   - Solution: Edit `/boot/config.txt`, add `dtparam=i2c_arm=on`, reboot

2. **SPI not enabled**
   - Solution: Edit `/boot/config.txt`, add `dtparam=spi=on`, reboot

3. **Hardware not connected**
   - Solution: Check physical connections, ensure HAT is properly seated

4. **Wrong I2C address**
   - Solution: Run `sudo i2cdetect -y 1` to find actual address
   - Manually configure: `config.i2c_address = 0x41;` (or detected address)

5. **Permissions issue**
   - Solution: Run as root (`sudo ./ereader`) or add user to `i2c` and `spi` groups:
     ```bash
     sudo usermod -a -G i2c,spi $USER
     sudo reboot
     ```

### Battery Percentage Always Shows 100% or 0%

**Symptoms**: Percentage stuck at extreme values despite voltage reading correctly

**Causes & Solutions**:

1. **Wrong voltage thresholds**
   - Solution: Adjust `voltage_full` and `voltage_empty` for your battery type
   - LiPo: full=4200mV, empty=3200mV
   - Li-ion: full=4100mV, empty=3000mV

2. **Voltage divider ratio incorrect** (MCP3008 only)
   - Solution: Measure actual resistor values, calculate real ratio
   - Formula: `ratio = (R1 + R2) / R2`

### Battery Monitor Can't Detect Charging Status

**Symptoms**: `is_charging` always returns false

**Cause**: This feature requires INA219 or fuel gauge chip (not available with ADC)

**Solution**:
- Use UPS HAT with INA219 for charging detection
- Or: Accept that ADC-based monitoring can't detect charging

### E-Reader Doesn't Shut Down on Critical Battery

**Symptoms**: Battery drops below critical voltage but device stays on

**Cause**: Application not checking `battery_monitor_is_critical()`

**Solution**: Add shutdown logic to main.c:
```c
if (battery_monitor_is_critical(app->battery)) {
    printf("Critical battery! Shutting down...\n");

    /* Save all state */
    book_manager_cleanup(app->book_manager);
    settings_manager_save(app->settings);

    /* Initiate system shutdown */
    system("sudo shutdown -h now");
}
```

### Battery Voltage Readings Fluctuate Wildly

**Symptoms**: Voltage jumps around, percentage unstable

**Causes & Solutions**:

1. **Poor ADC connections** (MCP3008)
   - Solution: Check soldering, ensure solid connections
   - Add 0.1µF capacitor between ADC VDD and GND

2. **No filtering/debouncing**
   - Solution: Increase update interval to reduce noise
     ```c
     config.update_interval_ms = 120000; // 2 minutes
     ```

3. **Voltage divider resistors mismatched**
   - Solution: Use precision resistors (1% tolerance or better)

---

## Battery Life Estimates

### Pi Zero W Power Consumption

| Mode | Current Draw | Notes |
|------|-------------|-------|
| **Idle** (no display updates) | ~120-150mA | WiFi disabled, CPU idle |
| **Reading** (occasional page turns) | ~150-200mA | E-paper updates, button polling |
| **WiFi Active** | ~200-300mA | Downloading books, sync |
| **Deep Sleep** (future feature) | ~10-30mA | Display off, CPU suspended |

### Estimated Runtime

| Battery Capacity | Expected Runtime (Reading) |
|-----------------|---------------------------|
| 1000mAh        | 5-7 hours                 |
| 2000mAh        | 10-14 hours               |
| 3000mAh        | 15-20 hours               |
| 5000mAh        | 25-35 hours               |

*Assumptions: Average 175mA draw, 85% battery efficiency, no WiFi usage*

---

## Future Improvements

### Planned Features

- [ ] Support for LC709203F fuel gauge (more accurate percentages)
- [ ] Support for MAX17048 fuel gauge
- [ ] Battery calibration routine (learn actual capacity)
- [ ] Historical battery usage tracking
- [ ] Low-battery warnings at configurable thresholds
- [ ] Deep sleep mode integration (reduce idle power to <30mA)
- [ ] Solar charging support (for DFRobot HAT)

### Hardware Wishlist

- Custom PCB integrating battery, charging, boost, and monitoring
- Capacitive power button (soft power on/off)
- USB-C charging instead of micro-USB
- Wireless charging pad integration

---

## References and Resources

### Hardware Datasheets

- [INA219 Datasheet](https://www.ti.com/lit/ds/symlink/ina219.pdf) - Texas Instruments power monitor
- [MCP3008 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/21295d.pdf) - Microchip 10-bit ADC
- [TP4056 Datasheet](https://www.olimex.com/Products/Breadboarding/BB-PWR-3400/resources/TP4056.pdf) - LiPo charger IC

### Product Links

- [Waveshare UPS HAT (C)](https://www.waveshare.com/wiki/UPS_HAT_(C)) - Official wiki with specs
- [GitHub: rpi-battery-monitor](https://github.com/kmcallister/rpi-battery-monitor) - Reference implementation
- [RasPi.TV Battery Monitoring Guide](https://raspi.tv/tag/raspberry-pi-monitoring-its-own-battery-voltage)

### Related Documentation

- [WIRING_GUIDE.md](WIRING_GUIDE.md) - General GPIO and hardware wiring
- [BOM.md](BOM.md) - Bill of materials for e-reader project
- [DEVICE_TREE.md](DEVICE_TREE.md) - Linux device tree configuration

---

## License

This documentation is part of the open-source e-reader project and is provided under the same license as the project.

## Support

For issues with battery monitoring:
1. Check this documentation's troubleshooting section
2. Review logs for detection messages
3. Test hardware with `i2cdetect` or SPI loopback tests
4. File an issue on GitHub with hardware details and logs
