# Buildroot Directory

This directory contains the Buildroot build system source code.

## Setup

The Buildroot source is not included in git to keep the repository small. To download and set up Buildroot:

### Option 1: Using Make (Recommended)

```bash
# From the project root
make download-buildroot
```

### Option 2: Manual Download

```bash
cd buildroot
curl -L -o buildroot-2024.02.2.tar.gz https://buildroot.org/downloads/buildroot-2024.02.2.tar.gz
tar -xzf buildroot-2024.02.2.tar.gz --strip-components=1
rm buildroot-2024.02.2.tar.gz
```

## Version

**Buildroot Version:** 2024.02.2 (May 7, 2024)

This is the latest stable release in the 2024.02.x LTS series.

## Configuration

The E-Reader custom configuration is located at:
- `configs/ereader_rpi0w_defconfig` - Main Buildroot configuration
- `board/ereader/` - Board-specific files and scripts

## Building

Once Buildroot is downloaded:

```bash
# Load the e-reader configuration
make config

# Build the system (first build takes 1-2 hours)
make build

# The output will be in buildroot/output/images/sdcard.img
```

## More Information

See the main project README.md for complete build instructions.
