#!/bin/bash

# SD Card Flashing Script for E-Reader Project
# This script safely writes the system image to an SD card
# Usage: sudo ./scripts/flash-sdcard.sh /dev/sdX  (Linux)
#        sudo ./scripts/flash-sdcard.sh /dev/diskN (Mac)

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
IMAGE_PATH="$PROJECT_ROOT/buildroot/output/images/sdcard.img"
MIN_IMAGE_SIZE=100000000  # 100 MB minimum

# Detect OS
OS_TYPE="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS_TYPE="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS_TYPE="mac"
else
    echo -e "${RED}Error: Unsupported operating system: $OSTYPE${NC}"
    echo "This script supports Linux and macOS only."
    exit 1
fi

# Print banner
echo -e "${BLUE}╔════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   E-Reader SD Card Flashing Script            ║${NC}"
echo -e "${BLUE}║   Project: Open Source E-Reader               ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
echo

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}Error: This script must be run as root${NC}"
    echo "Please run with sudo: sudo $0 $1"
    exit 1
fi

# Check arguments
if [ $# -ne 1 ]; then
    echo -e "${RED}Error: Missing device argument${NC}"
    echo
    echo "Usage:"
    echo "  Linux: sudo $0 /dev/sdX    (where X is b, c, d, etc.)"
    echo "  Mac:   sudo $0 /dev/diskN  (where N is 2, 3, 4, etc.)"
    echo
    echo "Examples:"
    echo "  sudo $0 /dev/sdb      # Linux - second disk"
    echo "  sudo $0 /dev/mmcblk0  # Linux - SD card reader"
    echo "  sudo $0 /dev/disk2    # Mac - second disk"
    echo
    echo -e "${YELLOW}To find your SD card device:${NC}"
    if [ "$OS_TYPE" == "linux" ]; then
        echo "  lsblk                  # List block devices"
        echo "  sudo fdisk -l          # List all disks"
        echo "  dmesg | tail           # Check recent device messages"
    else
        echo "  diskutil list          # List all disks"
    fi
    exit 1
fi

TARGET_DEVICE="$1"

# Validate device path
if [ ! -e "$TARGET_DEVICE" ]; then
    echo -e "${RED}Error: Device does not exist: $TARGET_DEVICE${NC}"
    exit 1
fi

# Check if it's a partition instead of a disk
if [[ "$TARGET_DEVICE" =~ [0-9]$ ]] && [[ "$TARGET_DEVICE" != *"mmcblk"* ]]; then
    echo -e "${YELLOW}Warning: You specified what appears to be a partition: $TARGET_DEVICE${NC}"
    echo "You should specify the disk device, not a partition."
    echo "Examples:"
    echo "  Correct: /dev/sdb (not /dev/sdb1)"
    echo "  Correct: /dev/disk2 (not /dev/disk2s1)"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Verify image exists
if [ ! -f "$IMAGE_PATH" ]; then
    echo -e "${RED}Error: SD card image not found${NC}"
    echo "Expected: $IMAGE_PATH"
    echo
    echo "You need to build the image first:"
    echo "  cd $PROJECT_ROOT"
    echo "  ./scripts/build.sh"
    exit 1
fi

# Check image size
IMAGE_SIZE=$(stat -f%z "$IMAGE_PATH" 2>/dev/null || stat -c%s "$IMAGE_PATH" 2>/dev/null)
if [ "$IMAGE_SIZE" -lt "$MIN_IMAGE_SIZE" ]; then
    echo -e "${RED}Error: Image file is too small (${IMAGE_SIZE} bytes)${NC}"
    echo "Expected at least $MIN_IMAGE_SIZE bytes"
    echo "The image may be corrupted. Try rebuilding."
    exit 1
fi

# Display device information
echo -e "${BLUE}═══ Device Information ═══${NC}"
if [ "$OS_TYPE" == "linux" ]; then
    lsblk "$TARGET_DEVICE" || true
    echo
    sudo fdisk -l "$TARGET_DEVICE" 2>/dev/null | head -n 5 || true
else
    diskutil info "$TARGET_DEVICE" | grep -E "(Device Node|Media Name|Total Size|Removable Media)" || true
fi
echo

# Display image information
echo -e "${BLUE}═══ Image Information ═══${NC}"
echo "Image path: $IMAGE_PATH"
echo "Image size: $(numfmt --to=iec-i --suffix=B $IMAGE_SIZE 2>/dev/null || echo "$IMAGE_SIZE bytes")"
if command -v sha256sum &> /dev/null; then
    echo "SHA256:     $(sha256sum "$IMAGE_PATH" | cut -d' ' -f1)"
elif command -v shasum &> /dev/null; then
    echo "SHA256:     $(shasum -a 256 "$IMAGE_PATH" | cut -d' ' -f1)"
fi
echo

# Final confirmation
echo -e "${YELLOW}╔════════════════════════════════════════════════╗${NC}"
echo -e "${YELLOW}║              ⚠️  WARNING  ⚠️                    ║${NC}"
echo -e "${YELLOW}╚════════════════════════════════════════════════╝${NC}"
echo -e "${RED}This will COMPLETELY ERASE all data on: $TARGET_DEVICE${NC}"
echo -e "${RED}This operation CANNOT be undone!${NC}"
echo
read -p "Are you ABSOLUTELY SURE you want to continue? Type 'yes' to proceed: " -r
echo
if [ "$REPLY" != "yes" ]; then
    echo -e "${GREEN}Aborted. No changes made.${NC}"
    exit 0
fi

# Unmount any mounted partitions
echo -e "${BLUE}═══ Unmounting partitions ═══${NC}"
if [ "$OS_TYPE" == "linux" ]; then
    # Unmount all partitions on the device
    for partition in ${TARGET_DEVICE}*; do
        if [ "$partition" != "$TARGET_DEVICE" ]; then
            if mountpoint -q "$partition" 2>/dev/null; then
                echo "Unmounting: $partition"
                sudo umount "$partition" || true
            fi
        fi
    done
    # Also check if the device itself is mounted
    if mountpoint -q "$TARGET_DEVICE" 2>/dev/null; then
        echo "Unmounting: $TARGET_DEVICE"
        sudo umount "$TARGET_DEVICE" || true
    fi
else
    # Mac: Unmount disk (not eject, we need the device available)
    echo "Unmounting: $TARGET_DEVICE"
    sudo diskutil unmountDisk "$TARGET_DEVICE" || true
fi
echo -e "${GREEN}✓ Unmount complete${NC}"
echo

# Write the image
echo -e "${BLUE}═══ Writing image to SD card ═══${NC}"
echo "This may take several minutes..."
echo

if [ "$OS_TYPE" == "linux" ]; then
    # Linux: Use dd with status=progress
    sudo dd if="$IMAGE_PATH" of="$TARGET_DEVICE" bs=4M status=progress conv=fsync
    DD_EXIT=$?
else
    # Mac: Use dd with rdisk for faster writes, display progress via SIGINFO
    echo "Press Ctrl+T to view progress during write"
    sudo dd if="$IMAGE_PATH" of="/dev/r${TARGET_DEVICE#/dev/}" bs=4m
    DD_EXIT=$?
fi

if [ $DD_EXIT -ne 0 ]; then
    echo -e "${RED}Error: dd command failed with exit code $DD_EXIT${NC}"
    exit 1
fi

echo
echo -e "${GREEN}✓ Write complete${NC}"
echo

# Sync to ensure all data is written
echo -e "${BLUE}═══ Syncing data ═══${NC}"
echo "Ensuring all data is written to SD card..."
sync
echo -e "${GREEN}✓ Sync complete${NC}"
echo

# Eject the device
echo -e "${BLUE}═══ Ejecting SD card ═══${NC}"
if [ "$OS_TYPE" == "linux" ]; then
    sudo eject "$TARGET_DEVICE" 2>/dev/null || echo "Device ejected (manual removal safe)"
else
    sudo diskutil eject "$TARGET_DEVICE" || echo "Device ejected (manual removal safe)"
fi
echo -e "${GREEN}✓ SD card is safe to remove${NC}"
echo

# Success message
echo -e "${GREEN}╔════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║          ✓ FLASHING COMPLETE ✓                 ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════╝${NC}"
echo
echo "Next steps:"
echo "  1. Remove the SD card from your computer"
echo "  2. Insert it into your Raspberry Pi Zero W"
echo "  3. Ensure the e-paper display is connected (see docs/hardware/WIRING_GUIDE.md)"
echo "  4. Connect 5V power supply to the Pi"
echo "  5. Wait ~30-35 seconds for boot and display initialization"
echo "  6. You should see 'Hello E-Reader' on the display"
echo
echo "For detailed boot and verification instructions, see:"
echo "  docs/deployment/FLASHING_GUIDE.md"
echo
echo -e "${BLUE}═══ Troubleshooting ═══${NC}"
echo "Serial console:"
echo "  - Connect USB-to-serial adapter (3.3V only!)"
echo "  - Pi GPIO Pin 6 (GND) → Adapter GND"
echo "  - Pi GPIO Pin 8 (TX) → Adapter RX"
echo "  - Pi GPIO Pin 10 (RX) → Adapter TX"
if [ "$OS_TYPE" == "linux" ]; then
    echo "  - sudo screen /dev/ttyUSB0 115200"
else
    echo "  - sudo screen /dev/tty.usbserial-* 115200"
fi
echo
echo "Login credentials (development only!):"
echo "  Username: root"
echo "  Password: ereader"
echo
echo -e "${GREEN}Happy hacking!${NC}"
