---
type: reference
title: WiFi Testing Guide
created: 2026-01-17
tags:
  - testing
  - wifi
  - network
  - verification
  - phase-06
related:
  - "[[WIFI_SETUP]]"
  - "[[WIFI_UI]]"
  - "[[BOOK_SOURCES]]"
  - "[[USER_GUIDE]]"
---

# WiFi Testing Guide

This document provides comprehensive test procedures for Phase 06 WiFi and network features on the E-Reader. These tests verify WiFi scanning, connection management, book downloads, time synchronization, and network performance.

## Prerequisites

Before testing WiFi features, ensure:

1. **Hardware is assembled:**
   - Raspberry Pi Zero W (WiFi required - not compatible with Pi Zero without W)
   - E-paper display and buttons properly connected
   - Antenna connected (PCB trace antenna or external antenna)
   - Power supply adequate (5V 2A recommended during WiFi use)

2. **Software is built:**
   - Buildroot rebuilt with Phase 06 WiFi configuration (see Build Instructions below)
   - SD card flashed with Phase 06 image
   - Serial console access configured (highly recommended for WiFi testing)

3. **Network environment:**
   - WiFi access point available (2.4 GHz network - Pi Zero W does not support 5 GHz)
   - Known WiFi network name (SSID) and password for WPA2-PSK authentication
   - Open network available for testing (optional but recommended)
   - Internet connectivity available for download testing

4. **System is booted:**
   - Raspberry Pi powered on and fully booted to library menu
   - Serial console connected (for monitoring logs)
   - Root login available via serial console

## Build Instructions

Before testing, you must rebuild Buildroot with WiFi support enabled:

```bash
# Navigate to buildroot directory
cd buildroot

# Load the Phase 06 configuration
make ereader_rpi0w_defconfig

# Verify WiFi packages are enabled
make menuconfig
# Check that the following are enabled:
#   - Target packages -> Hardware handling -> Firmware -> rpi-wifi-firmware
#   - Target packages -> Networking applications -> wpa_supplicant
#   - Target packages -> Networking applications -> dhcpcd
#   - Target packages -> Networking applications -> wireless tools
#   - Target packages -> Networking applications -> iw
#   - Target packages -> Networking applications -> chrony (for time sync)
#   - Target packages -> Networking applications -> wget (for downloads)

# Build the system (this will take 1-2 hours on first build)
make

# Flash to SD card (replace /dev/sdX with your SD card device)
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress && sync
```

**Important**: Ensure you're using the `configs/ereader_rpi0w_defconfig` configuration which includes all Phase 06 WiFi packages.

## Test Environment Setup

### 1. Configure WiFi Credentials (Pre-Configuration Method)

Before first boot, you can pre-configure WiFi by editing the config file on the SD card:

```bash
# Mount the SD card rootfs partition (usually partition 2)
sudo mount /dev/sdX2 /mnt

# Copy the example config and edit it
sudo cp /mnt/etc/wpa_supplicant.conf.example /mnt/etc/wpa_supplicant.conf
sudo nano /mnt/etc/wpa_supplicant.conf

# Add your network:
network={
    ssid="YourNetworkName"
    psk="YourPassword"
}

# Unmount
sudo umount /mnt
```

### 2. Serial Console Access

Connect to serial console to monitor WiFi connection logs:

```bash
# On your host computer
screen /dev/ttyUSB0 115200

# Login: root
# Password: ereader
```

## Testing Procedures

### Level 1: WiFi Hardware and Firmware Verification

Verify that the WiFi hardware is detected and firmware is loaded correctly.

#### 1.1 Check WiFi Interface Presence

```bash
# List network interfaces
ip link show

# Expected output should include:
# wlan0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
```

**Pass Criteria**: `wlan0` interface is present

#### 1.2 Verify WiFi Firmware Loading

```bash
# Check kernel messages for firmware loading
dmesg | grep -i brcm

# Expected output should include:
# brcmfmac: brcmf_fw_alloc_request: using brcm/brcmfmac43430-sdio for chip BCM43430/1
# brcmfmac: brcmf_c_preinit_dcmds: Firmware: BCM43430/1 wl0: Oct 23 2017 03:55:53 version 7.45.41.46
```

**Pass Criteria**: Firmware loads without errors, version displayed

#### 1.3 Check WiFi Driver Status

```bash
# Check if driver is loaded
lsmod | grep brcmfmac

# Check interface status
iw dev wlan0 info
```

**Pass Criteria**: `brcmfmac` module loaded, interface in managed mode

### Level 2: WiFi Scanning and Network Detection

Test the ability to scan for available networks.

#### 2.1 Manual WiFi Scan (Command Line)

```bash
# Bring interface up
ip link set wlan0 up

# Trigger scan
iw dev wlan0 scan | grep -E "SSID|signal|freq"

# Expected output shows available networks with:
# - SSID names
# - Signal strength (dBm)
# - Frequency (MHz)
```

**Pass Criteria**:
- At least one network detected
- Signal strength values reasonable (-30 to -90 dBm)
- SSID correctly decoded

#### 2.2 WiFi Scan via Init Script

```bash
# Use the network init script scan function
/etc/init.d/S40network scan

# Check scan log
tail -20 /var/log/network.log
```

**Pass Criteria**:
- Networks listed with SSIDs and signal levels
- Log shows "Found X networks"
- No error messages in log

#### 2.3 WiFi Scan via UI

1. Boot to library menu
2. Press MENU button
3. Navigate to "Settings" and press SELECT
4. Navigate to "WiFi Settings" and press SELECT
5. Press SELECT on "Scan Networks"
6. Observe scanning screen (should show "Scanning...")
7. Verify network list appears with:
   - SSID names
   - Signal bars (####---- format, 8 levels)
   - Lock icon (*) for secured networks

**Pass Criteria**:
- Scan completes within 5-10 seconds
- At least one known network appears
- Signal strength displayed correctly
- Security indicators correct (open vs. encrypted)

### Level 3: WiFi Connection and Authentication

Test connecting to WiFi networks with different security configurations.

#### 3.1 Connect to WPA2-PSK Network (Command Line)

```bash
# Create wpa_supplicant config
cat > /etc/wpa_supplicant.conf << EOF
ctrl_interface=/var/run/wpa_supplicant
update_config=1
country=US

network={
    ssid="YourNetworkName"
    psk="YourPassword"
}
EOF

# Start connection
/etc/init.d/S40network start

# Monitor connection status
/etc/init.d/S40network status

# Check logs
tail -f /var/log/network.log
```

**Pass Criteria**:
- wpa_supplicant connects successfully
- DHCP assigns IP address
- Logs show "Connected to YourNetworkName"
- Status shows IP address, signal strength

#### 3.2 Connect to Open Network (Command Line)

```bash
# Create config for open network
cat > /etc/wpa_supplicant.conf << EOF
ctrl_interface=/var/run/wpa_supplicant
update_config=1
country=US

network={
    ssid="OpenNetworkName"
    key_mgmt=NONE
}
EOF

# Restart network
/etc/init.d/S40network restart
```

**Pass Criteria**: Connection successful without password

#### 3.3 Connect via UI (Saved Network)

Pre-configure a network, then test connection via UI:

1. Navigate to WiFi Settings
2. Press SELECT on "Scan Networks"
3. Navigate to a saved network
4. Press SELECT
5. Choose "Connect" option
6. Observe "Connecting..." screen
7. Verify success screen shows:
   - "Connected successfully!"
   - SSID name
   - IP address

**Pass Criteria**:
- Connection completes within 30 seconds
- Success message displayed
- Status shows connected state

#### 3.4 Connect via UI (New Network with Password Entry)

Test on-device password entry using text input system:

1. Navigate to WiFi Settings
2. Scan for networks
3. Select a WPA2-PSK network not in saved list
4. Choose "Enter password" option
5. Use UP/DOWN to navigate character list
6. Press SELECT to add characters
7. Enter complete password
8. Navigate to "DONE" and press SELECT
9. Verify connection attempt

**Pass Criteria**:
- Text input system functional
- Password entry works (characters added correctly)
- BACKSPACE removes characters
- Connection succeeds with correct password
- Error message displayed if password wrong

#### 3.5 Authentication Failure Handling

Test with wrong password:

```bash
# Create config with wrong password
network={
    ssid="YourNetwork"
    psk="WrongPassword123"
}

# Attempt connection
/etc/init.d/S40network restart
```

**Pass Criteria**:
- Connection fails gracefully
- Error logged: "Authentication failure"
- System does not hang or crash
- UI shows "Wrong password" error

### Level 4: WiFi Disconnection and Management

Test disconnection and network switching.

#### 4.1 Manual Disconnection

```bash
# Check current connection
/etc/init.d/S40network status

# Disconnect
/etc/init.d/S40network stop

# Verify disconnection
ip addr show wlan0 | grep "inet "
# Should show no IP address
```

**Pass Criteria**: Clean disconnection, IP released

#### 4.2 Disconnect via UI

1. Navigate to WiFi Settings
2. Verify "Status" shows connected
3. Press SELECT on "Disconnect"
4. Verify disconnection message
5. Check status shows "Not connected"

**Pass Criteria**: UI correctly shows disconnected state

#### 4.3 Network Switching

1. Connect to Network A
2. Scan networks
3. Select Network B
4. Verify automatic disconnection from A
5. Verify connection to B

**Pass Criteria**: Seamless switching, no manual disconnect needed

### Level 5: Signal Strength and Connection Stability

Test signal strength reporting and connection stability.

#### 5.1 Signal Strength Accuracy

```bash
# Get signal strength
iw dev wlan0 link | grep signal

# Expected: signal: -XX dBm
```

**Pass Criteria**:
- Signal reported in valid range (-30 to -90 dBm)
- Stronger signals have less negative values
- UI signal bars match dBm level per WIFI_UI.md spec:
  - 8 bars: -30 to -44 dBm (excellent)
  - 6 bars: -45 to -59 dBm (good)
  - 4 bars: -60 to -74 dBm (fair)
  - 2 bars: -75 to -89 dBm (weak)
  - 1 bar: -90 dBm or worse (very weak)

#### 5.2 Connection Persistence

```bash
# Monitor connection over time
while true; do
    echo "$(date): $(iw dev wlan0 link | grep signal)"
    sleep 10
done
```

**Pass Criteria**:
- Connection remains stable over 5+ minutes
- Signal fluctuates normally (±5 dBm)
- No unexpected disconnections

#### 5.3 Weak Signal Handling

Move device to edge of WiFi range and test:

**Pass Criteria**:
- Connection maintained until signal < -90 dBm
- Graceful disconnection if signal lost
- Automatic reconnection when back in range

### Level 6: Book Download Testing

Test downloading books from Project Gutenberg.

#### 6.1 Manual Download via wget

```bash
# Test basic download
wget -O /books/test.txt https://www.gutenberg.org/ebooks/84.txt.utf-8

# Verify file
ls -lh /books/test.txt
head -20 /books/test.txt
```

**Pass Criteria**:
- Download completes successfully
- File size reasonable (typically 100KB - 1MB for text books)
- File contains readable text
- Book appears in library on next scan

#### 6.2 Download Manager API Test

Create a test program or use the download manager directly:

```c
// Test download_manager.c functions
download_options_t opts = {
    .timeout_seconds = 30,
    .max_file_size = 10 * 1024 * 1024,  // 10 MB
    .overwrite_existing = false
};

download_error_t result = download_file(
    "https://www.gutenberg.org/ebooks/84.txt.utf-8",
    "/books/frankenstein.txt",
    &opts,
    &progress_callback
);
```

**Pass Criteria**:
- Download starts successfully
- Progress callback reports percentage
- Download completes without errors
- File validation passes (TXT format detected)
- Disk space checked before download

#### 6.3 Online Library Browser UI Test

1. Navigate to Settings
2. Select "Online Library"
3. Browse book catalog
4. Select a book (e.g., "Pride and Prejudice")
5. View book details:
   - Title, Author, Genre, Size
6. Press SELECT to download
7. Observe progress screen showing:
   - Download percentage (0-100%)
   - Download speed (KB/s)
   - ETA (estimated time)
8. Wait for download completion
9. Verify success message
10. Return to library
11. Verify book appears in local library

**Pass Criteria**:
- Catalog displays 100 books across 8 genres
- Book details accurate
- Download starts within 2 seconds
- Progress updates every second
- Download speed reasonable (10-100 KB/s typical for Pi Zero W)
- Success message displayed
- Book immediately available in library

#### 6.4 Download Error Handling

Test error scenarios:

**Network Timeout:**
1. Start download
2. Disconnect WiFi during download
3. Verify timeout error displayed

**Disk Full:**
```bash
# Fill disk to near capacity
dd if=/dev/zero of=/books/filler.dat bs=1M count=100

# Attempt large download
# Should fail with "Disk full" error
```

**Invalid URL:**
1. Manually test download_file() with invalid URL
2. Verify DNS/connection error reported

**Pass Criteria**:
- All errors handled gracefully
- Error messages descriptive
- No crashes or hangs
- User can retry or cancel

### Level 7: Time Synchronization Testing

Test NTP time sync via chrony.

#### 7.1 Chrony Service Status

```bash
# Check if chronyd is running
/etc/init.d/S41chronyd status

# Check chrony tracking
chronyc tracking

# Expected output shows:
# Reference ID, Stratum, System time offset, etc.
```

**Pass Criteria**:
- chronyd service running
- Tracking shows valid NTP server
- System time offset < 1 second

#### 7.2 Manual Time Sync

```bash
# Trigger immediate sync
/etc/init.d/S41chronyd sync

# Check result
chronyc sources

# Verify time is correct
date
```

**Pass Criteria**:
- Sync completes within 5 seconds
- System time matches real time
- NTP sources reachable

#### 7.3 Automatic Sync on WiFi Connect

```bash
# Monitor time sync log
tail -f /var/log/timesync.log

# Connect to WiFi
/etc/init.d/S40network start

# Observe automatic time sync trigger
```

**Pass Criteria**:
- Time sync triggered automatically within 10 seconds of WiFi connection
- Log shows successful sync
- System time corrected if was wrong

#### 7.4 Time Sync API Test

Test time_sync.c functions:

```c
// Initialize time sync
time_sync_init();

// Trigger sync
time_sync_trigger();

// Check status
time_sync_status_t status = time_sync_get_status();
// status.state should be TIME_SYNC_SYNCED
// status.offset should be < 1000 ms
```

**Pass Criteria**:
- API functions return expected values
- Sync state transitions correctly
- Error handling works

### Level 8: Network Performance Testing

Measure and document WiFi performance characteristics.

#### 8.1 Download Speed Test

```bash
# Download large file and measure speed
time wget -O /tmp/test.dat https://www.gutenberg.org/files/84/84-0.txt

# Calculate speed from output
```

**Expected Performance**:
- Download speed: 50-200 KB/s (typical for Pi Zero W on 2.4 GHz)
- Latency: 20-100 ms to external servers
- Connection time: 5-15 seconds

**Pass Criteria**: Speed within expected range for Pi Zero W

#### 8.2 Ping Test (Latency)

```bash
# Test latency to common servers
ping -c 10 8.8.8.8
ping -c 10 www.gutenberg.org

# Average should be < 100ms on good connection
```

**Pass Criteria**: Average ping < 150 ms

#### 8.3 DNS Resolution Speed

```bash
# Test DNS lookup speed
time nslookup www.gutenberg.org
```

**Pass Criteria**: Resolution < 2 seconds

#### 8.4 Multiple Download Test

Download 5 books sequentially and measure:
- Total time
- Average download speed
- Connection stability

**Pass Criteria**: All downloads complete successfully

### Level 9: Battery Impact Testing

Measure power consumption during WiFi usage.

#### 9.1 Idle WiFi Power Consumption

```bash
# Measure current draw with WiFi connected but idle
# (Requires ammeter in power line)

# Expected: ~120-150 mA (compared to ~80 mA without WiFi)
```

#### 9.2 Active Download Power Consumption

```bash
# Measure current during active download
# Expected: ~200-300 mA peak during WiFi transmission
```

**Documentation**: Record measurements in performance log

### Level 10: Edge Cases and Stress Testing

Test unusual scenarios and error conditions.

#### 10.1 Hidden Network Connection

```bash
# Configure hidden network
network={
    ssid="HiddenNetwork"
    scan_ssid=1
    psk="password"
}
```

**Pass Criteria**: Can connect to hidden networks with scan_ssid=1

#### 10.2 Special Characters in SSID/Password

Test networks with:
- Spaces in SSID
- Special characters (!@#$%^&*) in password
- Unicode characters (if supported)

**Pass Criteria**: Proper escaping and handling

#### 10.3 Multiple Saved Networks

Configure 5+ networks with priorities:

```bash
network={
    ssid="Home"
    psk="password1"
    priority=10
}

network={
    ssid="Work"
    psk="password2"
    priority=5
}
```

**Pass Criteria**: Connects to highest priority available network

#### 10.4 Rapid Connection/Disconnection

```bash
# Cycle connect/disconnect 10 times
for i in {1..10}; do
    /etc/init.d/S40network start
    sleep 5
    /etc/init.d/S40network stop
    sleep 2
done
```

**Pass Criteria**:
- No memory leaks
- No crashes
- All cycles complete successfully

#### 10.5 Long-Running Stability

Keep WiFi connected for 24 hours:
- Monitor logs for errors
- Check memory usage
- Verify connection remains stable

**Pass Criteria**: No disconnections, no memory growth

## Test Results Documentation

### Test Summary Template

Use this template to document test results:

```markdown
## WiFi Test Results - [Date]

**Hardware**: Raspberry Pi Zero W
**Software**: Phase 06 Build [Git commit hash]
**Network**: [Router model, 2.4GHz]
**Signal Strength**: -XX dBm

### Level 1: Hardware Verification
- [ ] WiFi interface detected: PASS/FAIL
- [ ] Firmware loaded: PASS/FAIL
- [ ] Driver status: PASS/FAIL

### Level 2: WiFi Scanning
- [ ] Manual scan: PASS/FAIL (X networks found)
- [ ] Init script scan: PASS/FAIL
- [ ] UI scan: PASS/FAIL

### Level 3: WiFi Connection
- [ ] WPA2-PSK connection: PASS/FAIL
- [ ] Open network connection: PASS/FAIL
- [ ] UI saved network: PASS/FAIL
- [ ] UI password entry: PASS/FAIL
- [ ] Auth failure handling: PASS/FAIL

### Level 4: Disconnection
- [ ] Manual disconnect: PASS/FAIL
- [ ] UI disconnect: PASS/FAIL
- [ ] Network switching: PASS/FAIL

### Level 5: Signal Strength
- [ ] Signal accuracy: PASS/FAIL (actual: -XX dBm)
- [ ] Connection stability: PASS/FAIL
- [ ] Weak signal handling: PASS/FAIL

### Level 6: Book Downloads
- [ ] Manual wget: PASS/FAIL
- [ ] Download manager API: PASS/FAIL
- [ ] Online library UI: PASS/FAIL (X books downloaded)
- [ ] Error handling: PASS/FAIL

### Level 7: Time Synchronization
- [ ] Chrony status: PASS/FAIL
- [ ] Manual sync: PASS/FAIL
- [ ] Auto sync: PASS/FAIL
- [ ] API test: PASS/FAIL

### Level 8: Performance
- Download speed: XXX KB/s
- Latency: XX ms
- DNS resolution: X.X seconds
- Multiple downloads: PASS/FAIL

### Level 9: Battery Impact
- Idle consumption: XXX mA
- Active download: XXX mA
- Impact: +XX% vs. no WiFi

### Level 10: Edge Cases
- [ ] Hidden networks: PASS/FAIL
- [ ] Special characters: PASS/FAIL
- [ ] Multiple networks: PASS/FAIL
- [ ] Rapid cycling: PASS/FAIL
- [ ] 24h stability: PASS/FAIL

### Issues Found
1. [Issue description]
2. [Issue description]

### Performance Summary
- **WiFi Scan Time**: X seconds
- **Connection Time**: X seconds
- **Download Speed**: XXX KB/s average
- **Time Sync Accuracy**: ±X seconds

### Recommendations
- [Recommendation 1]
- [Recommendation 2]
```

## Performance Benchmarks

Expected performance characteristics for Phase 06:

| Metric | Target | Acceptable Range |
|--------|--------|-----------------|
| WiFi Scan Time | 3-5 sec | 2-10 sec |
| Connection Time (WPA2) | 8-12 sec | 5-20 sec |
| Connection Time (Open) | 3-5 sec | 2-10 sec |
| Download Speed | 100 KB/s | 50-200 KB/s |
| Ping Latency | 30 ms | 20-100 ms |
| DNS Resolution | 0.5 sec | 0.2-2 sec |
| Time Sync Accuracy | ±0.5 sec | ±2 sec |
| Book Download (500KB) | 5-10 sec | 3-20 sec |
| UI Scan→Connect | 15-20 sec | 10-30 sec |

## Troubleshooting Common Issues

### WiFi Interface Not Found

**Symptom**: `ip link` does not show wlan0

**Solutions**:
1. Verify BR2_PACKAGE_RPI_WIFI_FIRMWARE=y in defconfig
2. Check kernel config for CONFIG_BRCMFMAC=y
3. Verify device tree overlay loaded: `dtoverlay -l`
4. Check dmesg for firmware loading errors

### Firmware Loading Fails

**Symptom**: `dmesg | grep brcm` shows "Failed to load firmware"

**Solutions**:
1. Verify firmware files in /lib/firmware/brcm/
2. Check BR2_PACKAGE_RPI_FIRMWARE in Buildroot config
3. Rebuild with `make linux-reconfigure && make`

### Cannot Connect to WPA2 Network

**Symptom**: wpa_supplicant fails authentication

**Solutions**:
1. Verify password correct (use wpa_passphrase to test)
2. Check router uses WPA2-PSK (not WPA3)
3. Verify 2.4 GHz network (Pi Zero W doesn't support 5 GHz)
4. Check logs: `tail /var/log/network.log`

### DHCP Timeout

**Symptom**: Connected to AP but no IP address

**Solutions**:
1. Verify dhcpcd is running: `ps | grep dhcpcd`
2. Manually request IP: `dhcpcd wlan0`
3. Check router DHCP server enabled
4. Try static IP configuration

### Slow Download Speeds

**Symptom**: Downloads slower than 50 KB/s

**Solutions**:
1. Check signal strength (should be > -75 dBm)
2. Move closer to router
3. Verify router not congested
4. Check for interference (microwaves, other 2.4 GHz devices)

### Time Sync Fails

**Symptom**: chronyd cannot reach NTP servers

**Solutions**:
1. Verify internet connectivity: `ping 8.8.8.8`
2. Check DNS working: `nslookup pool.ntp.org`
3. Try manual sync: `/etc/init.d/S41chronyd sync`
4. Check firewall not blocking NTP (port 123 UDP)

## Integration Testing

After individual component testing, perform full integration test:

1. **Fresh Boot Test**:
   - Flash fresh image
   - Configure WiFi via SD card edit
   - Boot device
   - Verify auto-connection
   - Verify time sync
   - Download book from UI
   - Read book
   - Verify bookmark saves with correct timestamp

2. **First-Time User Experience**:
   - Boot without WiFi config
   - Navigate to WiFi Settings via UI
   - Scan networks
   - Connect using password entry
   - Download book
   - Verify smooth workflow

3. **Multi-Day Test**:
   - Leave connected for multiple days
   - Download books periodically
   - Verify no degradation
   - Check logs for errors

## Regression Testing

Verify Phase 06 changes didn't break existing features:

- [ ] Display still works (Phase 01)
- [ ] Buttons still work (Phase 02)
- [ ] Book library browsing works (Phase 03)
- [ ] Text rendering works (Phase 03)
- [ ] Bookmarks save/load correctly (Phase 03)
- [ ] Settings menu accessible (Phase 05)
- [ ] Boot time not significantly increased (<30 seconds)

## Test Coverage Checklist

Ensure all Phase 06 features are tested:

- [x] WiFi scanning (manual, script, UI)
- [x] WPA2-PSK connection
- [x] Open network connection
- [x] Password entry UI
- [x] Disconnection
- [x] Network switching
- [x] Signal strength reporting
- [x] Saved networks list
- [x] Book download (wget, API, UI)
- [x] Download progress tracking
- [x] Download error handling
- [x] Online library browsing
- [x] Time synchronization
- [x] Auto sync on connect
- [x] Performance benchmarks
- [x] Battery impact measurement
- [x] Edge cases
- [x] Stability testing

## Next Steps

After completing WiFi testing:

1. Document all results in test report
2. Fix any critical bugs found
3. Update USER_GUIDE.md with tested procedures
4. Mark Phase 06 as complete
5. Plan Phase 07 (release preparation)

## References

- [[WIFI_SETUP]] - WiFi hardware and configuration details
- [[WIFI_UI]] - WiFi UI design specification
- [[BOOK_SOURCES]] - Online book source documentation
- [[USER_GUIDE]] - End user WiFi setup instructions
- Project Gutenberg: https://www.gutenberg.org/
- chrony documentation: https://chrony.tuxfamily.org/
