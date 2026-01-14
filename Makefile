# E-Reader Project Makefile
# Wrapper for common Buildroot commands

# Directories
BUILDROOT_DIR := buildroot
CONFIG_DIR := configs
BOARD_DIR := board/ereader
OUTPUT_DIR := $(BUILDROOT_DIR)/output

# Configuration
DEFCONFIG := ereader_rpi0w_defconfig

# Default target
.PHONY: all
all: help

# Help target
.PHONY: help
help:
	@echo "E-Reader Build System"
	@echo "====================="
	@echo ""
	@echo "Configuration targets:"
	@echo "  config          - Load e-reader defconfig"
	@echo "  menuconfig      - Interactive configuration menu"
	@echo "  savedefconfig   - Save current config as defconfig"
	@echo ""
	@echo "Build targets:"
	@echo "  build           - Build complete system image"
	@echo "  linux-rebuild   - Rebuild Linux kernel only"
	@echo "  clean           - Clean build artifacts"
	@echo "  distclean       - Remove all build files and config"
	@echo ""
	@echo "Information targets:"
	@echo "  show-info       - Show build configuration info"
	@echo "  list-packages   - List all available packages"
	@echo ""
	@echo "Output will be in: $(OUTPUT_DIR)/images/sdcard.img"
	@echo ""

# Load the e-reader configuration
.PHONY: config
config:
	@echo "Loading e-reader configuration..."
	$(MAKE) -C $(BUILDROOT_DIR) BR2_DEFCONFIG=../$(CONFIG_DIR)/$(DEFCONFIG) defconfig
	@echo "Configuration loaded. Run 'make build' to build the system."

# Interactive configuration
.PHONY: menuconfig
menuconfig:
	@echo "Opening Buildroot menuconfig..."
	$(MAKE) -C $(BUILDROOT_DIR) menuconfig

# Save current configuration
.PHONY: savedefconfig
savedefconfig:
	@echo "Saving configuration to $(CONFIG_DIR)/$(DEFCONFIG)..."
	$(MAKE) -C $(BUILDROOT_DIR) BR2_DEFCONFIG=../$(CONFIG_DIR)/$(DEFCONFIG) savedefconfig

# Build the complete system
.PHONY: build
build:
	@echo "Building e-reader system..."
	@echo "This may take 1-2 hours on first build..."
	$(MAKE) -C $(BUILDROOT_DIR)
	@echo ""
	@echo "Build complete!"
	@echo "SD card image: $(OUTPUT_DIR)/images/sdcard.img"
	@echo ""
	@echo "To flash to SD card:"
	@echo "  Linux: sudo dd if=$(OUTPUT_DIR)/images/sdcard.img of=/dev/sdX bs=4M status=progress"
	@echo "  Windows: Use Win32DiskImager or Rufus"
	@echo ""

# Rebuild Linux kernel
.PHONY: linux-rebuild
linux-rebuild:
	@echo "Rebuilding Linux kernel..."
	$(MAKE) -C $(BUILDROOT_DIR) linux-rebuild

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	$(MAKE) -C $(BUILDROOT_DIR) clean

# Complete clean
.PHONY: distclean
distclean:
	@echo "Removing all build files..."
	$(MAKE) -C $(BUILDROOT_DIR) distclean

# Show build information
.PHONY: show-info
show-info:
	@echo "Buildroot Information"
	@echo "===================="
	$(MAKE) -C $(BUILDROOT_DIR) show-info

# List available packages
.PHONY: list-packages
list-packages:
	@echo "Available Buildroot packages:"
	@$(MAKE) -C $(BUILDROOT_DIR) list-defconfigs | grep -E "^  "

# Print output directory for scripts
.PHONY: print-output-dir
print-output-dir:
	@echo $(OUTPUT_DIR)
