#!/bin/bash
#
# E-Reader Build Script
# Reproducible build wrapper for the e-reader project
#
# This script provides a consistent build environment and tracks build metrics.
# It should be run from the project root directory.
#

set -e  # Exit on error
set -u  # Error on undefined variables

# Color output for better readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILDROOT_DIR="${PROJECT_ROOT}/buildroot"
OUTPUT_DIR="${BUILDROOT_DIR}/output"
IMAGES_DIR="${OUTPUT_DIR}/images"
LOG_DIR="${PROJECT_ROOT}/build-logs"
BUILD_LOG="${LOG_DIR}/build-$(date +%Y%m%d-%H%M%S).log"

# Create log directory
mkdir -p "${LOG_DIR}"

# Print colored message
print_msg() {
    local color=$1
    shift
    echo -e "${color}$*${NC}"
}

print_header() {
    echo ""
    print_msg "${BLUE}" "========================================="
    print_msg "${BLUE}" "$*"
    print_msg "${BLUE}" "========================================="
    echo ""
}

# Check prerequisites
check_prerequisites() {
    print_header "Checking Prerequisites"

    local missing_tools=()

    # Check for required tools
    for tool in make gcc g++ patch perl python3 rsync wget cpio unzip bc; do
        if ! command -v $tool &> /dev/null; then
            missing_tools+=($tool)
        fi
    done

    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_msg "${RED}" "Error: Missing required tools: ${missing_tools[*]}"
        print_msg "${YELLOW}" "On Ubuntu/Debian, install with:"
        print_msg "${YELLOW}" "  sudo apt-get install build-essential libncurses5-dev git wget cpio python3 unzip rsync bc"
        exit 1
    fi

    print_msg "${GREEN}" "✓ All prerequisites found"
}

# Check Buildroot
check_buildroot() {
    print_header "Checking Buildroot"

    if [ ! -f "${BUILDROOT_DIR}/Makefile" ]; then
        print_msg "${RED}" "Error: Buildroot not found in ${BUILDROOT_DIR}"
        print_msg "${YELLOW}" "Run 'make download-buildroot' first"
        exit 1
    fi

    print_msg "${GREEN}" "✓ Buildroot found"
}

# Display build configuration
show_config() {
    print_header "Build Configuration"

    echo "Project Root:    ${PROJECT_ROOT}"
    echo "Buildroot Dir:   ${BUILDROOT_DIR}"
    echo "Output Dir:      ${OUTPUT_DIR}"
    echo "Build Log:       ${BUILD_LOG}"
    echo ""
}

# Perform the build
do_build() {
    print_header "Starting Build Process"

    local start_time=$(date +%s)

    print_msg "${YELLOW}" "This may take 1-2 hours on first build..."
    print_msg "${YELLOW}" "Build output is being logged to: ${BUILD_LOG}"
    echo ""

    cd "${PROJECT_ROOT}"

    # Run make and capture output
    if make build 2>&1 | tee "${BUILD_LOG}"; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        local duration_min=$((duration / 60))
        local duration_sec=$((duration % 60))

        print_header "Build Successful!"

        print_msg "${GREEN}" "Build completed in ${duration_min}m ${duration_sec}s"

        return 0
    else
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        local duration_min=$((duration / 60))
        local duration_sec=$((duration % 60))

        print_header "Build Failed"

        print_msg "${RED}" "Build failed after ${duration_min}m ${duration_sec}s"
        print_msg "${YELLOW}" "Check the build log: ${BUILD_LOG}"

        return 1
    fi
}

# Verify build artifacts
verify_output() {
    print_header "Verifying Build Artifacts"

    local all_ok=true

    # Check for expected output files
    local expected_files=(
        "sdcard.img"
        "zImage"
        "rootfs.ext4"
        "bcm2708-rpi-zero-w.dtb"
    )

    for file in "${expected_files[@]}"; do
        if [ -f "${IMAGES_DIR}/${file}" ]; then
            local size=$(du -h "${IMAGES_DIR}/${file}" | cut -f1)
            print_msg "${GREEN}" "✓ ${file} (${size})"
        else
            print_msg "${RED}" "✗ ${file} - NOT FOUND"
            all_ok=false
        fi
    done

    echo ""

    if $all_ok; then
        print_msg "${GREEN}" "All expected artifacts found!"
        echo ""
        print_msg "${BLUE}" "SD card image ready at:"
        print_msg "${BLUE}" "  ${IMAGES_DIR}/sdcard.img"
        echo ""
        print_msg "${YELLOW}" "Flash to SD card with:"
        print_msg "${YELLOW}" "  sudo dd if=${IMAGES_DIR}/sdcard.img of=/dev/sdX bs=4M status=progress"
        print_msg "${YELLOW}" "  (Replace /dev/sdX with your SD card device)"
        echo ""
        return 0
    else
        print_msg "${RED}" "Some expected artifacts are missing!"
        return 1
    fi
}

# Extract build metrics from log
extract_metrics() {
    print_header "Build Metrics"

    if [ ! -f "${BUILD_LOG}" ]; then
        print_msg "${YELLOW}" "Build log not found, skipping metrics"
        return
    fi

    # Count warnings
    local warnings=$(grep -i "warning:" "${BUILD_LOG}" | wc -l)

    # Count errors (excluding the final error if any)
    local errors=$(grep -i "error:" "${BUILD_LOG}" | wc -l)

    echo "Warnings: ${warnings}"
    echo "Errors:   ${errors}"

    if [ $warnings -gt 0 ]; then
        print_msg "${YELLOW}" ""
        print_msg "${YELLOW}" "Warning summary (first 10):"
        grep -i "warning:" "${BUILD_LOG}" | head -10 | sed 's/^/  /'
    fi

    echo ""
}

# Main execution
main() {
    print_header "E-Reader Build Script"

    # Check we're in the right directory
    if [ ! -f "${PROJECT_ROOT}/Makefile" ]; then
        print_msg "${RED}" "Error: This script must be run from the project root"
        print_msg "${YELLOW}" "Current directory: $(pwd)"
        print_msg "${YELLOW}" "Expected: ${PROJECT_ROOT}"
        exit 1
    fi

    # Run checks
    check_prerequisites
    check_buildroot
    show_config

    # Perform build
    if do_build; then
        verify_output
        extract_metrics

        print_header "Build Complete!"
        print_msg "${GREEN}" "Your e-reader system is ready to flash!"
        exit 0
    else
        extract_metrics

        print_header "Build Failed"
        print_msg "${RED}" "Please check the log file for details:"
        print_msg "${RED}" "  ${BUILD_LOG}"
        exit 1
    fi
}

# Run main function
main "$@"
