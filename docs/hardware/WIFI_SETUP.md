# WiFi Setup Guide

This document describes the WiFi hardware setup, firmware requirements, and configuration for the Raspberry Pi Zero W e-reader.

## Hardware Overview

The Raspberry Pi Zero W includes built-in WiFi hardware:

- **WiFi Chip:** Broadcom BCM43430 (brcmfmac43430)
- **WiFi Standards:** 802.11 b/g/n (2.4 GHz only)
- **Interface:** SDIO (Secure Digital Input Output)
- **Antenna:** PCB trace antenna (built into the Pi Zero W board)
- **Typical Range:** 10-30 meters indoors (depending on obstacles and interference)

## Antenna Considerations

### Built-in Antenna
The Pi Zero W has a PCB trace antenna built into the board. This antenna:
- Is located near the wireless chip on the board
- Provides adequate performance for most indoor use cases
- Has no external connector, so external antennas cannot be easily added
- Performance may be reduced when enclosed in metal cases

### Improving WiFi Performance

If you experience weak WiFi signal:

1. **Positioning:** Orient the e-reader so the WiFi chip side faces toward the access point
2. **Case Material:** Use plastic or wood cases instead of metal, which can block RF signals
3. **Interference:** Keep away from sources of 2.4 GHz interference (microwaves, Bluetooth devices)
4. **Access Point:** Ensure your WiFi router is within reasonable range and not heavily loaded

### WiFi Power Management

The e-reader enables WiFi power management by default to conserve battery:
- WiFi will sleep when not actively transmitting/receiving data
- Connection may take 1-2 seconds to "wake up" when starting a download
- This is normal behavior and helps extend battery life

## Firmware Requirements

The BCM43430 WiFi chip requires firmware to operate. This firmware is automatically included in the system image.

### Firmware Files

The following firmware files are required and included in `/lib/firmware/brcm/`:

- `brcmfmac43430-sdio.bin` - Main WiFi firmware binary
- `brcmfmac43430-sdio.txt` - WiFi configuration file (NVRAM settings)

These files are provided by the `BR2_PACKAGE_RPI_WIFI_FIRMWARE` Buildroot package and are automatically installed during the build process.

### Verifying Firmware Installation

To verify the firmware is correctly installed, boot the e-reader and check:

```bash
# Check if firmware files exist
ls -l /lib/firmware/brcm/brcmfmac43430*

# Check if WiFi driver loaded successfully
dmesg | grep brcmfmac

# Expected output should show:
# brcmfmac: brcmf_fw_alloc_request: using brcm/brcmfmac43430-sdio for chip BCM43430/1
# brcmfmac: brcmf_c_preinit_dcmds: Firmware: BCM43430/1 wl0: ...
```

## Kernel Driver Configuration

The WiFi functionality requires several kernel options to be enabled. These are configured in `configs/linux_ereader.fragment`:

### Core Wireless Subsystem
- `CONFIG_WIRELESS=y` - Wireless networking support
- `CONFIG_CFG80211=y` - cfg80211 wireless configuration API
- `CONFIG_MAC80211=y` - Generic IEEE 802.11 stack

### Broadcom Driver
- `CONFIG_BRCMFMAC=y` - Broadcom FullMAC WLAN driver
- `CONFIG_BRCMFMAC_SDIO=y` - SDIO bus interface for brcmfmac

### SDIO Support
- `CONFIG_MMC=y` - MMC/SD/SDIO support
- `CONFIG_MMC_BCM2835=y` - BCM2835 SDIO controller

### Cryptographic Support
WPA/WPA2 encryption requires:
- `CONFIG_CRYPTO_AES=y` - AES encryption
- `CONFIG_CRYPTO_CCM=y` - CCM mode (WPA2)
- `CONFIG_CRYPTO_GCM=y` - GCM mode
- `CONFIG_CRYPTO_CMAC=y` - CMAC authentication

All required options are automatically included when building with the provided configuration.

## Userspace Components

Several userspace packages are required for WiFi functionality:

### WPA Supplicant
**Purpose:** Handles WPA/WPA2 authentication and encryption

**Packages:**
- `BR2_PACKAGE_WPA_SUPPLICANT=y` - Main WPA supplicant daemon
- `BR2_PACKAGE_WPA_SUPPLICANT_CLI=y` - Command-line interface for control
- `BR2_PACKAGE_WPA_SUPPLICANT_PASSPHRASE=y` - Support for passphrase authentication

**Usage:**
```bash
# Start wpa_supplicant
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf

# Interactive control
wpa_cli
```

### Wireless Tools
**Purpose:** Basic WiFi management and diagnostics

**Packages:**
- `BR2_PACKAGE_WIRELESS_TOOLS=y` - Legacy wireless tools (iwconfig, iwlist)
- `BR2_PACKAGE_IW=y` - Modern nl80211 based CLI (iw command)

**Usage:**
```bash
# Scan for networks (modern)
iw dev wlan0 scan

# Scan for networks (legacy)
iwlist wlan0 scan

# Check link status
iw dev wlan0 link
iwconfig wlan0
```

### DHCP Client
**Purpose:** Automatically obtain IP address from router

**Package:**
- `BR2_PACKAGE_DHCPCD=y` - DHCP client daemon

**Usage:**
```bash
# Start DHCP client
dhcpcd wlan0

# Check assigned IP
ip addr show wlan0
```

### DNS Resolver
**Purpose:** Resolve domain names to IP addresses

**Packages:**
- `BR2_PACKAGE_BIND=y` - BIND DNS server/resolver
- `BR2_PACKAGE_BIND_TOOLS=y` - DNS query tools (dig, nslookup)

## Network Interface Management

### Bringing Up WiFi Interface

The WiFi interface `wlan0` is created when the brcmfmac driver loads:

```bash
# Check if interface exists
ip link show wlan0

# Bring interface up
ip link set wlan0 up

# Verify interface is up
ip link show wlan0
# Should show state UP
```

### Checking WiFi Hardware

```bash
# List WiFi interfaces
iw dev

# Get interface info
iw dev wlan0 info

# Get WiFi capabilities
iw list
```

## Configuration Files

### wpa_supplicant.conf

Location: `/etc/wpa_supplicant.conf`

This file stores WiFi network credentials. A template is provided in `/etc/wpa_supplicant.conf.example`.

**Example configuration:**
```
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1
country=US

network={
    ssid="YourNetworkName"
    psk="YourNetworkPassword"
    key_mgmt=WPA-PSK
    priority=1
}
```

### Network Startup Script

An init script is provided to automatically start WiFi on boot:

- **Location:** `/etc/init.d/S40network`
- **Function:** Brings up wlan0, starts wpa_supplicant, runs DHCP client

## First-Time Setup

For first-time WiFi setup, you have two options:

### Option 1: Pre-Configure on SD Card

Before booting the device, edit the configuration on the SD card:

1. Insert SD card into computer
2. Mount the root partition
3. Edit `/etc/wpa_supplicant.conf`
4. Add your network credentials
5. Safely eject SD card
6. Boot e-reader - WiFi should connect automatically

### Option 2: Configure via Serial Console

If you have serial console access:

1. Boot the e-reader
2. Login via serial (root / ereader)
3. Edit `/etc/wpa_supplicant.conf`:
   ```bash
   vi /etc/wpa_supplicant.conf
   ```
4. Add your network credentials
5. Restart networking:
   ```bash
   /etc/init.d/S40network restart
   ```

### Option 3: Use wpa_passphrase Tool

Generate encrypted PSK:

```bash
# Generate configuration entry
wpa_passphrase "YourSSID" "YourPassword" >> /etc/wpa_supplicant.conf

# Start wpa_supplicant
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf

# Start DHCP
dhcpcd wlan0
```

## Troubleshooting

### WiFi Interface Not Found

**Symptoms:** `wlan0` interface does not exist

**Checks:**
1. Verify firmware files are present:
   ```bash
   ls -l /lib/firmware/brcm/brcmfmac43430*
   ```

2. Check if driver loaded:
   ```bash
   dmesg | grep brcmfmac
   lsmod | grep brcmfmac
   ```

3. Manually load driver:
   ```bash
   modprobe brcmfmac
   ```

### Cannot Scan Networks

**Symptoms:** `iw dev wlan0 scan` fails or shows no networks

**Checks:**
1. Verify interface is up:
   ```bash
   ip link set wlan0 up
   ```

2. Check for hardware RF kill:
   ```bash
   rfkill list
   # If blocked, unblock:
   rfkill unblock wifi
   ```

3. Verify you're in range of a 2.4 GHz network (5 GHz not supported)

### Cannot Connect to Network

**Symptoms:** wpa_supplicant starts but no connection

**Checks:**
1. Verify credentials in `/etc/wpa_supplicant.conf`
2. Check WPA supplicant logs:
   ```bash
   wpa_cli status
   wpa_cli scan_results
   ```

3. Test with different security modes (WPA-PSK vs WPA2-PSK)
4. Ensure router is using 2.4 GHz band (not 5 GHz)

### No IP Address Obtained

**Symptoms:** Connected to WiFi but no internet access

**Checks:**
1. Check if DHCP client is running:
   ```bash
   ps | grep dhcpcd
   ```

2. Manually request IP:
   ```bash
   dhcpcd -d wlan0  # Debug mode
   ```

3. Try static IP configuration:
   ```bash
   ip addr add 192.168.1.100/24 dev wlan0
   ip route add default via 192.168.1.1
   echo "nameserver 8.8.8.8" > /etc/resolv.conf
   ```

### Weak Signal or Frequent Disconnects

**Solutions:**
1. Move closer to WiFi access point
2. Reduce interference from other 2.4 GHz devices
3. Use a plastic or wooden case (not metal)
4. Orient device with WiFi chip facing access point
5. Check router settings for 2.4 GHz channel congestion

## Performance Characteristics

### Expected Performance

- **Link Speed:** Up to 72 Mbps (802.11n, single stream)
- **Typical Throughput:** 10-30 Mbps (depends on signal strength and interference)
- **Connection Time:** 3-8 seconds (from power on to IP address obtained)
- **Range:** 10-30 meters indoors

### Power Consumption

- **WiFi Active (transmitting):** ~150-200 mA additional current draw
- **WiFi Connected (idle):** ~50-80 mA with power save enabled
- **WiFi Disabled:** ~0 mA

For maximum battery life, WiFi should be disabled when not needed.

## Security Considerations

### Supported Security Modes

- **Open** - No encryption (not recommended)
- **WEP** - Legacy encryption (not recommended, insecure)
- **WPA-PSK (TKIP)** - Older WPA with pre-shared key
- **WPA2-PSK (AES)** - Modern WPA2 with pre-shared key (recommended)
- **WPA/WPA2 Mixed** - Both WPA and WPA2 supported

**Recommendation:** Use WPA2-PSK with AES encryption for best security.

### Enterprise Authentication

WPA-Enterprise (802.1X) with RADIUS authentication is supported by wpa_supplicant but requires additional configuration. See wpa_supplicant documentation for EAP configuration.

## Advanced Configuration

### Custom wpa_supplicant Options

Edit `/etc/wpa_supplicant.conf` to customize behavior:

```
# Disable scan while associated (improves stability)
ap_scan=1

# Update roaming settings
bgscan="simple:30:-65:300"

# Disable WiFi power save (increases power but improves latency)
# Add to network block:
network={
    ssid="YourNetwork"
    psk="YourPassword"
    # Disable power save
    priority=1
}
```

### Static IP Configuration

For static IP instead of DHCP, create `/etc/network/interfaces`:

```
auto wlan0
iface wlan0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1
    dns-nameservers 8.8.8.8 8.8.4.4
    wpa-conf /etc/wpa_supplicant.conf
```

### Multiple Network Profiles

wpa_supplicant supports multiple networks with priority:

```
network={
    ssid="HomeNetwork"
    psk="homepassword"
    priority=10
}

network={
    ssid="WorkNetwork"
    psk="workpassword"
    priority=5
}
```

Higher priority networks are preferred. The system will automatically connect to the best available network.

## Related Documentation

- [User Guide](../USER_GUIDE.md) - Basic WiFi usage instructions for end users
- [Phase 06 Tasks](../../Auto Run Docs/Initiation/Phase-06-WiFi-and-Network-Features.md) - Implementation roadmap
- [Buildroot Configuration](../buildroot/BUILD_PROCESS.md) - Build system details

## References

- [Raspberry Pi WiFi Documentation](https://www.raspberrypi.com/documentation/computers/configuration.html#configuring-networking)
- [wpa_supplicant Documentation](https://w1.fi/wpa_supplicant/)
- [iw Documentation](https://wireless.wiki.kernel.org/en/users/documentation/iw)
- [Linux Wireless Documentation](https://wireless.wiki.kernel.org/)
- [BCM43430 Chipset Information](https://www.cypress.com/file/298076/download)
