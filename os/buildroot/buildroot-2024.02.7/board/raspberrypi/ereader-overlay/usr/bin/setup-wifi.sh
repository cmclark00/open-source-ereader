#!/bin/sh
#
# WiFi Setup Helper Script
# Easy way to configure WiFi credentials
#

CONF_FILE="/etc/wpa_supplicant/wpa_supplicant.conf"

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <ssid> [password]"
    echo ""
    echo "Examples:"
    echo "  $0 MyNetwork MyPassword123    # WPA/WPA2 network"
    echo "  $0 OpenNetwork                # Open network (no password)"
    echo ""
    echo "Current WiFi status:"
    ifconfig wlan0 2>/dev/null || echo "wlan0 not available"
    exit 1
fi

SSID="$1"
PASSWORD="$2"

echo "Configuring WiFi..."
echo "SSID: $SSID"

# Backup existing config
cp "$CONF_FILE" "${CONF_FILE}.bak"

# Create new config
cat > "$CONF_FILE" << EOF
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1

network={
    ssid="$SSID"
EOF

if [ -n "$PASSWORD" ]; then
    echo "    psk=\"$PASSWORD\"" >> "$CONF_FILE"
    echo "    key_mgmt=WPA-PSK" >> "$CONF_FILE"
    echo "Password: (set)"
else
    echo "    key_mgmt=NONE" >> "$CONF_FILE"
    echo "Password: (none - open network)"
fi

cat >> "$CONF_FILE" << EOF
    priority=1
}
EOF

echo ""
echo "WiFi configured! Restarting network..."

# Restart network interface
killall wpa_supplicant 2>/dev/null
ifdown wlan0 2>/dev/null
sleep 1
ifup wlan0

echo ""
echo "Waiting for connection..."
sleep 5

# Show status
ifconfig wlan0
echo ""
echo "WiFi setup complete!"
echo "If you don't have an IP address, try: ifdown wlan0 && ifup wlan0"
