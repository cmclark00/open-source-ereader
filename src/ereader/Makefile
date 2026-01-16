# Makefile for E-Reader Application
#
# This Makefile builds the complete e-reader application by compiling
# and linking all modules together, including the display driver from
# Phase 1 and the button input system from Phase 2.

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2 -g -std=gnu99
LDFLAGS := -lm -lzip -lxml2

# Target binary
TARGET := ereader

# Source directories
SRC_MAIN := main.c
SRC_RENDERING := rendering/framebuffer.c rendering/text_renderer.c
SRC_BOOKS := books/book_manager.c
SRC_FORMATS := formats/format_interface.c formats/txt_reader.c formats/epub_reader.c formats/pdf_reader.c
SRC_UI := ui/menu.c ui/reader.c
SRC_SETTINGS := settings/settings_manager.c
SRC_POWER := power/power_manager.c
SRC_DISPLAY := ../display-test/epd_driver.c
SRC_BUTTON := ../button-test/button_input.c

# All source files
SOURCES := $(SRC_MAIN) $(SRC_RENDERING) $(SRC_BOOKS) $(SRC_FORMATS) $(SRC_UI) $(SRC_SETTINGS) $(SRC_POWER) $(SRC_DISPLAY) $(SRC_BUTTON)

# Object files (derived from sources)
OBJECTS := $(SOURCES:.c=.o)

# Include directories
INCLUDES := -I. -I../display-test -I../button-test -I/usr/include/libxml2

# Default target
all: $(TARGET)

# Link all object files into final binary
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files to object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete"

# Install target (for use in Buildroot package)
install:
	@echo "Installing $(TARGET) to $(DESTDIR)/usr/bin/..."
	mkdir -p $(DESTDIR)/usr/bin
	install -m 0755 $(TARGET) $(DESTDIR)/usr/bin/
	@echo "Install complete"

# Phony targets (not actual files)
.PHONY: all clean install

# Header dependencies (simplified - all objects depend on key headers)
main.o: ereader.h rendering/framebuffer.h rendering/text_renderer.h books/book_manager.h ui/menu.h ui/reader.h settings/settings_manager.h
rendering/framebuffer.o: rendering/framebuffer.h
rendering/text_renderer.o: rendering/text_renderer.h rendering/framebuffer.h rendering/font_data.h
books/book_manager.o: books/book_manager.h formats/format_interface.h
formats/format_interface.o: formats/format_interface.h formats/txt_reader.h formats/epub_reader.h formats/pdf_reader.h
formats/txt_reader.o: formats/txt_reader.h formats/format_interface.h
formats/epub_reader.o: formats/epub_reader.h formats/format_interface.h
formats/pdf_reader.o: formats/pdf_reader.h formats/format_interface.h
ui/menu.o: ui/menu.h rendering/framebuffer.h rendering/text_renderer.h books/book_manager.h formats/format_interface.h
ui/reader.o: ui/reader.h rendering/framebuffer.h rendering/text_renderer.h books/book_manager.h
settings/settings_manager.o: settings/settings_manager.h
power/power_manager.o: power/power_manager.h
../display-test/epd_driver.o: ../display-test/epd_driver.h
../button-test/button_input.o: ../button-test/button_input.h

# Help target
help:
	@echo "E-Reader Application Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the e-reader application (default)"
	@echo "  clean    - Remove all build artifacts"
	@echo "  install  - Install to DESTDIR/usr/bin (for packaging)"
	@echo "  help     - Display this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make              # Build application"
	@echo "  make clean        # Clean build"
	@echo "  make install DESTDIR=/path/to/staging  # Install for packaging"
