#!/bin/sh

set -u
set -e

# E-Reader post-build script
# This script runs after the root filesystem is built but before it's packaged

# Add a console on tty1 for HDMI debugging (optional)
if [ -e ${TARGET_DIR}/etc/inittab ]; then
    grep -qE '^tty1::' ${TARGET_DIR}/etc/inittab || \
	sed -i '/GENERIC_SERIAL/a\
tty1::respawn:/sbin/getty -L  tty1 0 vt100 # HDMI console' ${TARGET_DIR}/etc/inittab
# systemd doesn't use /etc/inittab, enable getty.tty1.service instead
elif [ -d ${TARGET_DIR}/etc/systemd ]; then
    mkdir -p "${TARGET_DIR}/etc/systemd/system/getty.target.wants"
    ln -sf /lib/systemd/system/getty@.service \
       "${TARGET_DIR}/etc/systemd/system/getty.target.wants/getty@tty1.service"
fi

# Create directory for e-books
mkdir -p "${TARGET_DIR}/home/ereader/books"

# Set hostname
echo "ereader" > "${TARGET_DIR}/etc/hostname"

# Add welcome message
cat > "${TARGET_DIR}/etc/motd" << 'EOF'
  _____        ____                _
 | ____|      |  _ \ ___  __ _  __| | ___ _ __
 |  _| _____ _| |_) / _ \/ _` |/ _` |/ _ \ '__|
 | |__|_____(_)  _ <  __/ (_| | (_| |  __/ |
 |_____|      |_| \_\___|\__,_|\__,_|\___|_|

Welcome to Open Source E-Reader
Serial console ready on ttyAMA0 (115200 baud)
EOF

echo "E-Reader post-build script completed successfully"
