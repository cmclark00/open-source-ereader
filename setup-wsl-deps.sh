#!/bin/bash
# WSL2 Build Dependencies Installation Script
# Run this in WSL2 to install all required build dependencies

set -e

echo "=========================================="
echo "E-Reader Buildroot - WSL2 Setup Script"
echo "=========================================="
echo ""
echo "This script will install all required build dependencies for Buildroot."
echo "You will be prompted for your sudo password."
echo ""

# Check if running in WSL
if ! grep -qi microsoft /proc/version 2>/dev/null; then
    echo "WARNING: This doesn't appear to be WSL. Continuing anyway..."
    echo ""
fi

# Update package lists
echo "Step 1: Updating package lists..."
sudo apt-get update

# Install build dependencies
echo ""
echo "Step 2: Installing build dependencies..."
echo "This may take a few minutes..."
sudo apt-get install -y \
    build-essential \
    libncurses5-dev \
    git \
    wget \
    cpio \
    python3 \
    unzip \
    rsync \
    bc \
    file \
    bzip2 \
    gzip \
    perl \
    patch

# Verify installation
echo ""
echo "Step 3: Verifying installation..."
echo ""

MISSING=0

for tool in make gcc g++ git wget python3 rsync bc; do
    if command -v $tool &> /dev/null; then
        echo "✓ $tool: $(command -v $tool)"
    else
        echo "✗ $tool: NOT FOUND"
        MISSING=1
    fi
done

echo ""
if [ $MISSING -eq 0 ]; then
    echo "=========================================="
    echo "SUCCESS! All dependencies installed."
    echo "=========================================="
    echo ""
    echo "You can now build the e-reader system by running:"
    echo "  ./scripts/build.sh"
    echo ""
    echo "Expected build time: 1-2 hours on first build"
    exit 0
else
    echo "=========================================="
    echo "ERROR: Some dependencies are missing."
    echo "=========================================="
    echo ""
    echo "Please check the errors above and try:"
    echo "  sudo apt-get install -y build-essential libncurses5-dev git wget cpio python3 unzip rsync bc file"
    exit 1
fi
