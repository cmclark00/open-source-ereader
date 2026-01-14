#!/bin/bash

set -e

# E-Reader post-image script
# This script runs after all filesystem images are built

BOARD_DIR="$(dirname $0)"
BOARD_NAME="$(basename ${BOARD_DIR})"
GENIMAGE_CFG="${BOARD_DIR}/genimage.cfg"
GENIMAGE_TMP="${BUILD_DIR}/genimage.tmp"

# Check if custom genimage config exists, otherwise use default generation
if [ ! -e "${GENIMAGE_CFG}" ]; then
	echo "Generating genimage configuration from template..."
	GENIMAGE_CFG="${BINARIES_DIR}/genimage.cfg"
	FILES=()

	# Collect all device tree blobs and firmware files
	for i in "${BINARIES_DIR}"/*.dtb "${BINARIES_DIR}"/rpi-firmware/*; do
		if [ -e "${i}" ]; then
			FILES+=( "${i#${BINARIES_DIR}/}" )
		fi
	done

	# Add kernel image
	KERNEL=$(sed -n 's/^kernel=//p' "${BINARIES_DIR}/rpi-firmware/config.txt")
	FILES+=( "${KERNEL}" )

	# Generate boot files list for genimage config
	BOOT_FILES=$(printf '\\t\\t\\t"%s",\\n' "${FILES[@]}")

	# Create genimage config from template
	cat > "${GENIMAGE_CFG}" << EOF
image boot.vfat {
	vfat {
		files = {
${BOOT_FILES}
		}
	}
	size = 32M
}

image sdcard.img {
	hdimage {
	}

	partition boot {
		partition-type = 0xC
		bootable = "true"
		image = "boot.vfat"
	}

	partition rootfs {
		partition-type = 0x83
		image = "rootfs.ext4"
	}
}
EOF
fi

# Create temporary rootpath for genimage
trap 'rm -rf "${ROOTPATH_TMP}"' EXIT
ROOTPATH_TMP="$(mktemp -d)"

# Clean previous genimage temp directory
rm -rf "${GENIMAGE_TMP}"

echo "Running genimage to create SD card image..."

# Run genimage to create the final SD card image
genimage \
	--rootpath "${ROOTPATH_TMP}"   \
	--tmppath "${GENIMAGE_TMP}"    \
	--inputpath "${BINARIES_DIR}"  \
	--outputpath "${BINARIES_DIR}" \
	--config "${GENIMAGE_CFG}"

echo "SD card image created successfully at ${BINARIES_DIR}/sdcard.img"

exit $?
