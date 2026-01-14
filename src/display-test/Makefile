# Makefile for E-Paper Display Test Application
# Cross-compilation for Raspberry Pi Zero W using Buildroot toolchain

# Application name
TARGET = display-test

# Source files
SRCS = main.c epd_driver.c
OBJS = $(SRCS:.c=.o)
HEADERS = epd_driver.h font.h

# Buildroot toolchain configuration
# When building within Buildroot, these are set automatically
# For standalone cross-compilation, set BUILDROOT_OUTPUT to your buildroot output directory

# Default to host compiler if not cross-compiling
CROSS_COMPILE ?=

# If building with Buildroot, toolchain is provided by Buildroot
# Otherwise, use the host compiler for testing on x86/x64
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip

# Compiler flags
CFLAGS = -Wall -Wextra -O2 -std=gnu99
CFLAGS += -D_GNU_SOURCE

# Linker flags
LDFLAGS =

# Libraries
LIBS =

# Build type detection
BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE),debug)
    CFLAGS += -g -DDEBUG
    STRIP = true  # Don't strip in debug mode
else
    CFLAGS += -DNDEBUG
endif

# Phony targets
.PHONY: all clean install help

# Default target
all: $(TARGET)

# Build the application
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	@echo "Stripping debug symbols..."
	$(STRIP) $@
	@echo "Build complete: $@"
	@echo ""
	@echo "To install to target device:"
	@echo "  make install DESTDIR=/path/to/rootfs"
	@echo "  or copy $(TARGET) to /usr/bin/ on the target"

# Compile source files
%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete."

# Install to target filesystem
# Usage: make install DESTDIR=/path/to/rootfs
DESTDIR ?=
INSTALL_DIR = $(DESTDIR)/usr/bin

install: $(TARGET)
	@echo "Installing $(TARGET) to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	install -m 0755 $(TARGET) $(INSTALL_DIR)/
	@echo "Installation complete."

# Help target
help:
	@echo "E-Paper Display Test Application Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the application (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install to target filesystem (use DESTDIR=...)"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CROSS_COMPILE - Cross-compiler prefix (e.g., arm-linux-gnueabihf-)"
	@echo "  BUILD_TYPE    - 'release' (default) or 'debug'"
	@echo "  DESTDIR       - Destination root directory for install"
	@echo ""
	@echo "Examples:"
	@echo "  Native build (for testing on x86):"
	@echo "    make"
	@echo ""
	@echo "  Cross-compile for Raspberry Pi:"
	@echo "    make CROSS_COMPILE=arm-linux-gnueabihf-"
	@echo ""
	@echo "  Debug build:"
	@echo "    make BUILD_TYPE=debug"
	@echo ""
	@echo "  Install to Buildroot output:"
	@echo "    make install DESTDIR=../../buildroot/output/target"
	@echo ""
	@echo "When building as a Buildroot package, use the package infrastructure"
	@echo "which handles cross-compilation automatically."

# Dependency tracking
-include $(OBJS:.o=.d)

%.d: %.c
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< -o $@
