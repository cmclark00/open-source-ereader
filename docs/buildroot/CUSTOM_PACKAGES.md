---
type: reference
title: Creating Custom Buildroot Packages
created: 2026-01-13
tags:
  - buildroot
  - packages
  - build-system
related:
  - "[[PHASE_01_LOG]]"
  - "[[README]]"
---

# Creating Custom Buildroot Packages

This document describes how to create and integrate custom packages into the E-Reader Buildroot build system. It covers the process used to integrate the `display-test` application and serves as a template for adding future custom packages.

## Table of Contents

1. [Overview](#overview)
2. [BR2_EXTERNAL Structure](#br2_external-structure)
3. [Package Components](#package-components)
4. [Step-by-Step Guide](#step-by-step-guide)
5. [Init Scripts](#init-scripts)
6. [Testing and Verification](#testing-and-verification)
7. [Troubleshooting](#troubleshooting)

## Overview

The E-Reader project uses Buildroot's **BR2_EXTERNAL** mechanism to maintain custom packages outside the main Buildroot tree. This approach:

- Keeps custom code separate from Buildroot
- Makes it easy to update Buildroot versions
- Provides a clean structure for project-specific packages
- Enables version control of custom packages alongside the project

## BR2_EXTERNAL Structure

The project root acts as a BR2_EXTERNAL tree with the following structure:

```
open-source-ereader/
├── external.desc           # BR2_EXTERNAL metadata (name, description)
├── external.mk             # Include custom package makefiles
├── Config.in               # Include custom package Config.in files
├── package/                # Custom packages directory
│   └── display-test/       # Example: display-test package
│       ├── display-test.mk # Package build definition
│       ├── Config.in       # Package configuration for menuconfig
│       └── S99display-test # Init script (optional)
└── src/                    # Source code for custom packages
    └── display-test/       # Source code for display-test
        ├── main.c
        ├── epd_driver.c
        ├── epd_driver.h
        ├── font.h
        ├── Makefile
        └── README.md
```

### Core Files

#### external.desc

Defines the BR2_EXTERNAL tree name and description:

```
name: EREADER
desc: Open Source E-Reader Project for Raspberry Pi Zero W
```

The name `EREADER` creates a variable `BR2_EXTERNAL_EREADER_PATH` that can be used in package definitions.

#### external.mk

Includes all custom package makefiles:

```makefile
include $(sort $(wildcard $(BR2_EXTERNAL_EREADER_PATH)/package/*/*.mk))
```

This automatically discovers all `.mk` files in the `package/` directory.

#### Config.in

Includes all custom package Config.in files:

```
source "$BR2_EXTERNAL_EREADER_PATH/package/display-test/Config.in"
```

Add one `source` line for each custom package.

## Package Components

Each custom package requires the following components:

### 1. Package Makefile (`<package>.mk`)

The `.mk` file defines how to build and install the package. It uses Buildroot's package infrastructure.

**Location:** `package/<package-name>/<package-name>.mk`

**Example:** `package/display-test/display-test.mk`

```makefile
################################################################################
#
# display-test
#
################################################################################

# Package metadata
DISPLAY_TEST_VERSION = 1.0
DISPLAY_TEST_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/display-test
DISPLAY_TEST_SITE_METHOD = local
DISPLAY_TEST_LICENSE = Custom
DISPLAY_TEST_LICENSE_FILES = README.md

# Build commands
# The Makefile in src/display-test is already set up for cross-compilation
define DISPLAY_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		BUILD_TYPE=release \
		-C $(@D)
endef

# Install binary to /usr/bin
define DISPLAY_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/display-test \
		$(TARGET_DIR)/usr/bin/display-test
endef

# Install init script for automatic startup
define DISPLAY_TEST_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(BR2_EXTERNAL_EREADER_PATH)/package/display-test/S99display-test \
		$(TARGET_DIR)/etc/init.d/S99display-test
endef

$(eval $(generic-package))
```

**Key Variables:**

- `<PACKAGE>_VERSION`: Package version number
- `<PACKAGE>_SITE`: Source location (URL, path, or `local`)
- `<PACKAGE>_SITE_METHOD`: How to fetch source (`wget`, `git`, `local`, etc.)
- `<PACKAGE>_LICENSE`: License identifier
- `<PACKAGE>_LICENSE_FILES`: License file path(s)
- `<PACKAGE>_DEPENDENCIES`: Other packages this depends on

**Key Functions:**

- `<PACKAGE>_BUILD_CMDS`: Commands to build the package
- `<PACKAGE>_INSTALL_TARGET_CMDS`: Commands to install to target filesystem
- `<PACKAGE>_INSTALL_INIT_SYSV`: Install SysV init script (optional)
- `<PACKAGE>_INSTALL_INIT_SYSTEMD`: Install systemd unit (optional)

**Important Notes:**

- Package name must be uppercase with underscores in variables (e.g., `DISPLAY_TEST`)
- Package directory and files must be lowercase with hyphens (e.g., `display-test`)
- Use `$(TARGET_MAKE_ENV)` and `$(TARGET_CONFIGURE_OPTS)` for cross-compilation
- Use `$(@D)` to refer to the package build directory
- Use `$(TARGET_DIR)` to refer to the target filesystem root
- Use `$(BR2_EXTERNAL_EREADER_PATH)` to refer to project root

### 2. Package Config.in

Defines the package configuration option for `menuconfig`.

**Location:** `package/<package-name>/Config.in`

**Example:** `package/display-test/Config.in`

```
config BR2_PACKAGE_DISPLAY_TEST
	bool "display-test"
	help
	  E-Paper Display Test Application for Waveshare 4.2" display.

	  This application tests the Waveshare 4.2" e-paper display
	  by initializing the display via SPI and drawing "Hello E-Reader"
	  text using a basic bitmap font. It verifies that the hardware
	  and software stack are working correctly.

	  The application uses SPI device interface (/dev/spidev0.0) and
	  GPIO sysfs interface for display control signals (RST, DC, BUSY).

	  https://github.com/yourusername/open-source-ereader
```

**Key Points:**

- `config BR2_PACKAGE_<PACKAGE>`: Configuration variable (uppercase with underscores)
- `bool "display-name"`: Display name in menuconfig
- `help`: Multi-line description (indented with two spaces or a tab)
- Include project URL at the end
- Can add `depends on` for dependencies (e.g., `depends on BR2_USE_MMU`)
- Can add `select` to automatically enable dependencies

### 3. Source Code

The actual application source code.

**Location:** `src/<package-name>/`

**Requirements:**

- Must have a Makefile that supports cross-compilation
- Makefile should respect `$(CC)`, `$(CFLAGS)`, `$(LDFLAGS)` from environment
- Should have `all`, `clean`, and optionally `install` targets

**Example Makefile snippet:**

```makefile
# Buildroot toolchain configuration
CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip

# Compiler flags
CFLAGS = -Wall -Wextra -O2 -std=gnu99
LDFLAGS =

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	$(STRIP) $@

clean:
	rm -f $(OBJS) $(TARGET)
```

### 4. Init Scripts (Optional)

For applications that should run at boot or as daemons.

**Location:** `package/<package-name>/S<NN><package-name>`

Where `<NN>` is a two-digit number determining startup order (01-99). Higher numbers run later.

**Example:** `package/display-test/S99display-test`

```bash
#!/bin/sh
#
# S99display-test - Run E-Paper Display Test on boot
#

DAEMON="display-test"
EXEC="/usr/bin/$DAEMON"
LOGFILE="/var/log/display-test.log"

start() {
	printf 'Running E-Paper Display Test: '

	if [ ! -x "$EXEC" ]; then
		echo "FAIL (executable not found)"
		return 1
	fi

	$EXEC > "$LOGFILE" 2>&1
	status=$?

	if [ "$status" -eq 0 ]; then
		echo "OK"
	else
		echo "FAIL (exit code: $status)"
	fi

	return "$status"
}

stop() {
	echo "Display test is a one-shot application, nothing to stop"
	return 0
}

restart() {
	start
}

case "$1" in
	start)
		start;;
	stop)
		stop;;
	restart|reload)
		restart;;
	*)
		echo "Usage: $0 {start|stop|restart}"
		exit 1
esac
```

**Init Script Guidelines:**

- Must be executable (`chmod +x`)
- Must support `start`, `stop`, and `restart` arguments
- Use `start-stop-daemon` for daemon processes
- Use direct execution for one-shot applications
- Redirect output to log file for debugging
- Return 0 on success, non-zero on failure

## Step-by-Step Guide

### Adding a New Custom Package

Follow these steps to add a new custom package to the build system:

#### Step 1: Create Source Code

1. Create source directory: `mkdir -p src/<package-name>`
2. Add your source code files
3. Create a Makefile that supports cross-compilation
4. Test the Makefile with native compilation

#### Step 2: Create Package Definition

1. Create package directory: `mkdir -p package/<package-name>`

2. Create `package/<package-name>/<package-name>.mk`:

```makefile
################################################################################
#
# <package-name>
#
################################################################################

<PACKAGE_NAME>_VERSION = 1.0
<PACKAGE_NAME>_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/<package-name>
<PACKAGE_NAME>_SITE_METHOD = local
<PACKAGE_NAME>_LICENSE = <license>
<PACKAGE_NAME>_LICENSE_FILES = <license-file>

define <PACKAGE_NAME>_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		-C $(@D)
endef

define <PACKAGE_NAME>_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/<binary> \
		$(TARGET_DIR)/usr/bin/<binary>
endef

$(eval $(generic-package))
```

3. Create `package/<package-name>/Config.in`:

```
config BR2_PACKAGE_<PACKAGE_NAME>
	bool "<package-display-name>"
	help
	  Package description goes here.

	  https://github.com/yourusername/project
```

#### Step 3: Add to Build System

1. Add source line to `Config.in` at project root:

```
source "$BR2_EXTERNAL_EREADER_PATH/package/<package-name>/Config.in"
```

2. The `external.mk` file automatically discovers `.mk` files, no changes needed

#### Step 4: Enable Package in Configuration

1. Edit `configs/ereader_rpi0w_defconfig`
2. Add line: `BR2_PACKAGE_<PACKAGE_NAME>=y`
3. Run `make config` to reload configuration

Alternatively, use menuconfig:

```bash
make menuconfig
# Navigate to "External options" -> "EREADER"
# Select your package
# Save and exit
make savedefconfig  # Optional: save changes to defconfig
```

#### Step 5: Build and Test

1. Build the package:

```bash
make build
```

Or rebuild just the package:

```bash
cd buildroot
make BR2_EXTERNAL=.. <package-name>-rebuild
```

2. Check the output:

```bash
ls buildroot/output/target/usr/bin/<binary>
```

3. Flash to SD card and test on hardware

## Init Scripts

Init scripts control when and how applications run at boot time.

### Init Script Numbering

Scripts are executed in numerical order based on their `S<NN>` prefix:

- `S01xxx`: Early initialization (before networking)
- `S20xxx`: Network services
- `S50xxx`: Application daemons
- `S99xxx`: Final startup tasks

For the E-Reader project:

- `S02acpid`: Power button handling (if used)
- `S99display-test`: Display test (runs last to show startup is complete)

### Daemon vs One-Shot

**Daemon Services** (run continuously):

```bash
start() {
	start-stop-daemon -S -q -p "$PIDFILE" -x "$EXEC" -- $ARGS
}

stop() {
	start-stop-daemon -K -q -p "$PIDFILE" -x "$EXEC"
}
```

**One-Shot Applications** (run once and exit):

```bash
start() {
	$EXEC > "$LOGFILE" 2>&1
	return $?
}

stop() {
	echo "Nothing to stop"
	return 0
}
```

### Installing Init Scripts

Add to the package `.mk` file:

```makefile
define <PACKAGE_NAME>_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(BR2_EXTERNAL_EREADER_PATH)/package/<package>/S<NN><package> \
		$(TARGET_DIR)/etc/init.d/S<NN><package>
endef
```

The script will automatically run at boot if installed to `/etc/init.d/S*`.

## Testing and Verification

### Verify Package is Available

```bash
cd buildroot
make BR2_EXTERNAL=.. menuconfig
# Check "External options" -> "EREADER" for your package
```

### Verify Package is Enabled

```bash
grep "BR2_PACKAGE_<PACKAGE_NAME>" configs/ereader_rpi0w_defconfig
```

Should show: `BR2_PACKAGE_<PACKAGE_NAME>=y`

### Build Single Package

```bash
cd buildroot
make BR2_EXTERNAL=.. <package-name>-rebuild
```

### Check Installed Files

```bash
ls -la buildroot/output/target/usr/bin/<binary>
ls -la buildroot/output/target/etc/init.d/S*<package>
```

### Test on Hardware

1. Flash SD card with built image
2. Boot Raspberry Pi
3. Connect serial console
4. Check boot messages for init script output
5. Check log file: `cat /var/log/<package>.log`
6. Manually run: `/usr/bin/<binary>`
7. Manually run init script: `/etc/init.d/S<NN><package> start`

## Troubleshooting

### Package Not Appearing in Menuconfig

**Problem:** Custom package doesn't show up in menuconfig.

**Solutions:**

1. Check that `Config.in` at project root has a `source` line for the package
2. Verify BR2_EXTERNAL is set: `make BR2_EXTERNAL=.. menuconfig`
3. Check `external.desc` exists at project root
4. Verify package `Config.in` syntax (indentation must be tabs or spaces, not mixed)

### Package Not Building

**Problem:** Build fails with "No rule to make target" or similar.

**Solutions:**

1. Verify `external.mk` exists and includes the package `.mk` file
2. Check package name is consistent (uppercase in variables, lowercase in filenames)
3. Verify `<PACKAGE>_SITE_METHOD = local` for local packages
4. Check `<PACKAGE>_SITE` path is correct (use `$(BR2_EXTERNAL_EREADER_PATH)`)
5. Ensure source Makefile works: `cd src/<package> && make`

### Cross-Compilation Failures

**Problem:** Build fails with architecture mismatch or library not found.

**Solutions:**

1. Use `$(TARGET_MAKE_ENV)` and `$(TARGET_CONFIGURE_OPTS)` in build commands
2. Ensure Makefile respects `CC`, `CFLAGS`, `LDFLAGS` from environment
3. Don't hardcode compiler paths (use `$(CC)` instead of `gcc`)
4. Add dependencies to `<PACKAGE>_DEPENDENCIES` for required libraries

### Binary Not Installed

**Problem:** Binary doesn't appear in `/usr/bin` after build.

**Solutions:**

1. Check `<PACKAGE>_INSTALL_TARGET_CMDS` is defined
2. Verify `$(INSTALL)` command syntax: `$(INSTALL) -D -m 0755 source target`
3. Ensure `$(TARGET_DIR)` is used for target path
4. Check build directory: `ls buildroot/output/build/<package>-<version>/`

### Init Script Not Running

**Problem:** Init script installed but doesn't run at boot.

**Solutions:**

1. Verify script is executable: `ls -la buildroot/output/target/etc/init.d/S*`
2. Check script has correct shebang: `#!/bin/sh`
3. Test script manually on target: `/etc/init.d/S<NN><package> start`
4. Check system logs: `dmesg | grep <package>` or `cat /var/log/<package>.log`
5. Ensure `<PACKAGE>_INSTALL_INIT_SYSV` is defined in `.mk` file

### "site method" Errors

**Problem:** Error about unsupported site method.

**Solution:** For local packages, must set:

```makefile
<PACKAGE>_SITE = $(BR2_EXTERNAL_EREADER_PATH)/src/<package>
<PACKAGE>_SITE_METHOD = local
```

For remote packages, use:

```makefile
<PACKAGE>_SITE = http://example.com/downloads
<PACKAGE>_SITE_METHOD = wget
```

Or for GitHub:

```makefile
<PACKAGE>_SITE = $(call github,username,repo,$(PACKAGE_VERSION))
<PACKAGE>_SITE_METHOD = git
```

## Additional Resources

- [Buildroot Manual - Adding Packages](https://buildroot.org/downloads/manual/manual.html#adding-packages)
- [Buildroot Manual - BR2_EXTERNAL](https://buildroot.org/downloads/manual/manual.html#outside-br-custom)
- [Buildroot Manual - Generic Package Infrastructure](https://buildroot.org/downloads/manual/manual.html#generic-package-tutorial)
- Project phase log: `[[PHASE_01_LOG]]`
- Project README: `[[README]]`

## Summary

Adding a custom package to the E-Reader Buildroot build requires:

1. **Source code** in `src/<package>/` with cross-compilation Makefile
2. **Package definition** in `package/<package>/<package>.mk`
3. **Config option** in `package/<package>/Config.in`
4. **External tree** setup with `external.desc`, `external.mk`, `Config.in`
5. **Configuration** update to enable package in defconfig
6. **Init script** (optional) in `package/<package>/S<NN><package>`

The `display-test` and `button-test` packages serve as complete working examples of this process.

## Examples in This Project

The project includes two complete custom package examples:

### display-test Package (Phase 1)
- **Purpose:** Tests e-paper display functionality
- **Source:** `src/display-test/`, `package/display-test/`
- **Features:** SPI communication, GPIO control, bitmap rendering, init script

### button-test Package (Phase 2)
- **Purpose:** Tests GPIO button input functionality
- **Source:** `src/button-test/`, `package/button-test/`
- **Features:** Input events, software debouncing, library API

### ereader Package (Phase 3)
- **Purpose:** Main e-reader application with text rendering and book management
- **Source:** `src/ereader/`, `package/ereader/`
- **Features:** Text rendering, pagination, menu system, bookmark persistence

## Standard Buildroot Packages Used

The project also leverages several existing Buildroot packages for EPUB and PDF support:

### Phase 4: E-Book Format Support

**EPUB Libraries:**
- **libzip** (`BR2_PACKAGE_LIBZIP=y`)
  - Purpose: ZIP archive handling for EPUB files (EPUBs are ZIP containers)
  - License: BSD 3-Clause
  - Size: ~100KB library
  - Dependencies: zlib (already enabled)

- **libxml2** (`BR2_PACKAGE_LIBXML2=y`)
  - Purpose: XML parsing for EPUB metadata and structure files
  - License: MIT
  - Size: ~1.5MB library
  - Dependencies: None (minimal configuration)

**PDF Libraries:**
- **poppler** (`BR2_PACKAGE_POPPLER=y`)
  - Purpose: PDF rendering and text extraction
  - License: GPL v2/v3
  - Size: ~5-7MB library + dependencies
  - Dependencies: fontconfig
  - Notes: Chosen over MuPDF to avoid X11 dependency

- **poppler-utils** (`BR2_PACKAGE_POPPLER_UTILS=y`)
  - Purpose: Command-line utilities including `pdftotext`
  - Contains: pdfinfo, pdftotext, pdftoppm, and others
  - Usage: Text extraction from PDF files

**Rationale:**
- libzip + libxml2 provides lightweight EPUB parsing (~1.6MB total)
- Poppler chosen over MuPDF because MuPDF requires X11 (heavyweight for embedded)
- Poppler's `pdftotext` utility enables simple text extraction from PDFs
- All libraries are available as standard Buildroot packages (no custom packages needed)
- Total memory footprint: ~20-30MB runtime for EPUB/PDF reading

For detailed research and evaluation, see `[[EBOOK_LIBRARIES]]`.

## Revision History

- 2026-01-14: Added Phase 4 e-book library documentation (libzip, libxml2, poppler)
- 2026-01-13: Added button-test and ereader package examples (Phase 2-3)
- 2026-01-13: Initial version with display-test example (Phase 1)
