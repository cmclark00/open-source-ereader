---
type: reference
title: Contributing Guide
created: 2026-01-17
tags:
  - contributing
  - development
  - guidelines
  - community
related:
  - "[[DEVELOPMENT]]"
  - "[[BUILD_PROCESS]]"
  - "[[EREADER_DESIGN]]"
---

# Contributing to Open Source E-Reader

Thank you for your interest in contributing to the Open Source E-Reader project! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Environment Setup](#development-environment-setup)
- [Project Structure](#project-structure)
- [Coding Standards](#coding-standards)
- [Building and Testing](#building-and-testing)
- [Making Changes](#making-changes)
- [Pull Request Process](#pull-request-process)
- [Reporting Issues](#reporting-issues)
- [Communication](#communication)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors, regardless of experience level, background, or identity.

### Expected Behavior

- Be respectful and considerate in all interactions
- Provide constructive feedback and accept it graciously
- Focus on what is best for the community and project
- Show empathy towards other community members
- Use welcoming and inclusive language

### Unacceptable Behavior

- Harassment, trolling, or discriminatory comments
- Personal attacks or inflammatory language
- Publishing others' private information without permission
- Any conduct that would be inappropriate in a professional setting

### Enforcement

Project maintainers are responsible for enforcing the Code of Conduct. Violations may result in temporary or permanent removal from the project.

## Getting Started

### Prerequisites

Before you begin contributing, ensure you have:

1. **Basic knowledge of:**
   - C programming language
   - Git version control
   - Linux/Unix systems
   - Embedded systems (helpful but not required)

2. **Development tools:**
   - Git client
   - Text editor or IDE (VS Code, Vim, Emacs, etc.)
   - Linux environment (native or WSL2)
   - Build tools (see [Development Environment Setup](#development-environment-setup))

3. **Hardware (optional but helpful):**
   - Raspberry Pi Zero W or Pi Zero 2 W
   - Waveshare 4.2" e-paper display (Rev 2.2)
   - MicroSD card (8GB minimum)
   - USB-to-TTL serial adapter (for debugging)

### Finding Ways to Contribute

There are many ways to contribute to this project:

**For Beginners:**
- Fix typos or improve documentation
- Add code comments to improve clarity
- Report bugs with detailed reproduction steps
- Test the system on different hardware configurations
- Create sample book files or test content

**For Intermediate Contributors:**
- Implement feature requests from the issue tracker
- Improve existing features (performance, UX, reliability)
- Add unit tests for untested code
- Optimize memory usage or boot time
- Improve error handling

**For Advanced Contributors:**
- Implement major new features (see roadmap)
- Port to new hardware platforms
- Optimize display driver performance
- Implement complex format support
- Design new UI components

Check the [GitHub Issues](link-to-issues) for:
- Issues labeled `good-first-issue` for beginners
- Issues labeled `help-wanted` for all skill levels
- Issues labeled `enhancement` for new features

## Development Environment Setup

### Linux Host Setup

The e-reader build system requires a Linux environment. Choose one of:

#### Option 1: Native Linux (Recommended)

Install on Ubuntu 20.04+, Debian 11+, or Fedora 35+:

```bash
# Ubuntu/Debian - Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    git \
    libncurses5-dev \
    wget \
    cpio \
    python3 \
    unzip \
    rsync \
    bc \
    file \
    patch \
    perl \
    gdb \
    valgrind

# Fedora/RHEL - Install dependencies
sudo dnf install -y \
    gcc gcc-c++ make \
    git \
    ncurses-devel \
    wget \
    cpio \
    python3 \
    unzip \
    rsync \
    bc \
    file \
    patch \
    perl \
    gdb \
    valgrind
```

#### Option 2: Windows Subsystem for Linux 2 (WSL2)

```bash
# Install WSL2 with Ubuntu
wsl --install -d Ubuntu-22.04

# Inside WSL, install dependencies
sudo apt-get update
sudo apt-get install -y build-essential git libncurses5-dev wget \
    cpio python3 unzip rsync bc file patch perl gdb valgrind

# Important: Clone and work within WSL filesystem (/home/user/),
# NOT Windows filesystem (/mnt/c/), for better performance
cd ~
```

#### Option 3: Docker (Advanced)

```bash
# Use official Buildroot Docker image
docker pull buildroot/base:latest

# Run interactive container
docker run -it -v $(pwd):/workspace buildroot/base:latest
```

### Clone the Repository

```bash
# Clone the repository
git clone https://github.com/your-username/open-source-ereader.git
cd open-source-ereader

# Add upstream remote
git remote add upstream https://github.com/original-owner/open-source-ereader.git
git fetch upstream
```

### Download Buildroot

The project uses Buildroot to create the embedded Linux system:

```bash
# Download Buildroot (one-time setup)
make download-buildroot

# This downloads Buildroot 2024.02.x and extracts it to buildroot/
```

### Configure and Build

```bash
# Load default configuration
make config

# Optional: Review/modify configuration
make menuconfig

# Build the system (first build takes 1-2 hours)
./scripts/build.sh

# Or use make directly
make build
```

### Setting Up the Hardware (Optional)

If you have the physical hardware:

1. **Flash SD Card:**
   ```bash
   # Flash the built image to SD card
   sudo dd if=buildroot/output/images/sdcard.img of=/dev/sdX bs=4M status=progress && sync
   ```

2. **Connect Serial Console:**
   - Connect USB-to-TTL adapter: TX→GPIO 14, RX→GPIO 15, GND→GND
   - Use 115200 baud: `screen /dev/ttyUSB0 115200`

3. **Wire Display and Buttons:**
   - Follow instructions in `docs/hardware/WIRING_GUIDE.md`

### Verification

Verify your setup is working:

```bash
# Check that build outputs exist
ls -lh buildroot/output/images/sdcard.img

# Run basic tests (when implemented)
make test
```

## Project Structure

Understanding the project structure helps you navigate and contribute effectively:

```
open-source-ereader/
├── Auto Run Docs/          # Maestro agent automation tasks
│   ├── Initiation/         # Phase planning documents
│   └── Working/            # Temporary working files
│
├── board/                  # Board-specific files
│   └── ereader/
│       ├── overlays/       # Device tree overlays
│       ├── post-build.sh   # Post-build customization script
│       └── rootfs-overlay/ # Files copied to root filesystem
│
├── buildroot/              # Buildroot build system (git-ignored)
│
├── configs/                # Build configurations
│   ├── ereader_rpi0w_defconfig  # Main Buildroot configuration
│   └── linux_ereader.fragment   # Kernel configuration fragment
│
├── docs/                   # Documentation
│   ├── architecture/       # System architecture docs
│   ├── buildroot/          # Build system documentation
│   ├── deployment/         # Flashing and deployment guides
│   ├── development/        # Development guides
│   ├── hardware/           # Hardware specs and wiring
│   ├── progress/           # Phase completion logs
│   ├── research/           # Research and investigation
│   ├── testing/            # Test plans and results
│   └── BUILD_GUIDE.md      # Complete build guide
│
├── hardware/               # Hardware design files
│   └── schematics/         # Electrical schematics
│
├── package/                # Custom Buildroot packages
│   ├── display-test/       # Display test application package
│   ├── button-test/        # Button test application package
│   └── ereader/            # Main e-reader application package
│
├── scripts/                # Build and utility scripts
│   ├── build.sh            # Main build script
│   └── flash-sd.sh         # SD card flashing script (if exists)
│
├── src/                    # Source code
│   ├── button-test/        # Button input test application
│   │   ├── button_input.c/h
│   │   └── main.c
│   │
│   ├── display-test/       # Display driver test application
│   │   ├── epd_driver.c/h  # E-paper display SPI driver
│   │   ├── font.h          # Embedded bitmap font
│   │   └── main.c
│   │
│   └── ereader/            # Main e-reader application
│       ├── books/          # Book management
│       │   └── book_manager.c/h
│       ├── formats/        # Book format readers
│       │   ├── format_interface.c/h
│       │   ├── txt_reader.c/h
│       │   ├── epub_reader.c/h
│       │   └── pdf_reader.c/h
│       ├── network/        # WiFi and network features
│       │   ├── wifi_manager.c/h
│       │   ├── download_manager.c/h
│       │   └── time_sync.c/h
│       ├── power/          # Power management
│       │   ├── power_manager.c/h
│       │   └── battery_monitor.c/h
│       ├── rendering/      # Display rendering
│       │   ├── framebuffer.c/h
│       │   ├── text_renderer.c/h
│       │   └── font_data.h
│       ├── search/         # In-book search
│       │   └── search_engine.c/h
│       ├── settings/       # Settings management
│       │   └── settings_manager.c/h
│       ├── ui/             # User interface components
│       │   ├── library_browser.c/h
│       │   ├── reading_view.c/h
│       │   ├── settings_menu.c/h
│       │   ├── search_ui.c/h
│       │   └── ui_components.c/h
│       ├── ereader.h       # Main application header
│       └── main.c          # Application entry point
│
├── Config.in               # BR2_EXTERNAL Config.in
├── external.desc           # BR2_EXTERNAL description
├── external.mk             # BR2_EXTERNAL makefile
├── Makefile                # Top-level makefile
└── README.md               # Project overview
```

## Coding Standards

### C Code Style

We follow a consistent C coding style inspired by the Linux kernel style with some modifications for embedded systems.

#### General Principles

1. **Clarity over cleverness** - Write code that's easy to understand
2. **Consistency** - Follow existing patterns in the codebase
3. **Minimal dependencies** - Avoid unnecessary libraries
4. **Memory safety** - Always check allocations and bounds
5. **Error handling** - Handle all error conditions gracefully

#### Indentation and Formatting

```c
/* Use 4 spaces for indentation (NOT tabs) */
void example_function(int param)
{
    if (param > 0) {
        printf("Positive\n");
    } else {
        printf("Non-positive\n");
    }
}

/* Opening brace on same line for control structures */
for (int i = 0; i < count; i++) {
    process_item(i);
}

/* But on new line for function definitions */
int calculate_total(int *values, int count)
{
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum;
}
```

#### Naming Conventions

```c
/* Types: snake_case with _t suffix */
typedef struct {
    int width;
    int height;
} display_config_t;

/* Functions: snake_case with module prefix */
int display_init(display_config_t *config);
void display_clear(void);
int display_update(const uint8_t *framebuffer);

/* Constants: UPPER_CASE */
#define MAX_BUFFER_SIZE 1024
#define DEFAULT_TIMEOUT_MS 5000

/* Global variables: snake_case with g_ prefix (avoid if possible) */
static int g_display_initialized = 0;

/* Local variables: snake_case */
int page_number = 0;
char *book_title = NULL;

/* Enums: UPPER_CASE with module prefix */
typedef enum {
    DISPLAY_MODE_FULL_REFRESH,
    DISPLAY_MODE_PARTIAL_REFRESH,
    DISPLAY_MODE_FAST_REFRESH
} display_mode_t;
```

#### Comments

```c
/*
 * Multi-line comments use this format
 * with asterisks aligned on the left
 */

/* Single-line comments use this format */

/**
 * Function documentation uses Doxygen-style comments
 *
 * @param config Display configuration structure
 * @return 0 on success, negative error code on failure
 */
int display_init(display_config_t *config);

/* Inline comments explain WHY, not WHAT */
timeout_ms += 100;  /* Add debounce delay to prevent spurious triggers */

/* TODO/FIXME markers for future work */
// TODO: Implement partial refresh mode
// FIXME: Memory leak when display init fails
```

#### File Headers

Every source file should have a header:

```c
/*
 * book_manager.c - Book library scanning and management
 *
 * Copyright (C) 2026 Open Source E-Reader Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "book_manager.h"
#include "ereader.h"
```

#### Header Guards

```c
/* Use #ifndef guards, not #pragma once */
#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

/* Header content */

#endif /* BOOK_MANAGER_H */
```

#### Memory Management

```c
/* Always check malloc() return values */
char *buffer = malloc(size);
if (!buffer) {
    fprintf(stderr, "Failed to allocate buffer\n");
    return -ENOMEM;
}

/* Free resources on all error paths */
int load_book(const char *path)
{
    char *text = malloc(MAX_SIZE);
    if (!text) {
        return -ENOMEM;
    }

    FILE *f = fopen(path, "r");
    if (!f) {
        free(text);  /* Don't forget to free! */
        return -ENOENT;
    }

    /* ... use text and f ... */

    fclose(f);
    free(text);
    return 0;
}

/* Use consistent cleanup patterns */
int complex_function(void)
{
    int ret = 0;
    char *buffer = NULL;
    FILE *file = NULL;

    buffer = malloc(SIZE);
    if (!buffer) {
        ret = -ENOMEM;
        goto cleanup;
    }

    file = fopen("data.txt", "r");
    if (!file) {
        ret = -ENOENT;
        goto cleanup;
    }

    /* ... do work ... */

cleanup:
    if (file) fclose(file);
    if (buffer) free(buffer);
    return ret;
}
```

#### Error Handling

```c
/* Return negative errno values for errors, 0 for success */
int init_display(void)
{
    if (spi_init() < 0) {
        return -EIO;  /* I/O error */
    }

    if (gpio_setup() < 0) {
        spi_cleanup();
        return -EIO;
    }

    return 0;  /* Success */
}

/* Check return values of all functions */
int result = load_book(filename);
if (result < 0) {
    fprintf(stderr, "Failed to load book: %s\n", strerror(-result));
    return result;
}

/* Use meaningful error messages */
if (book_size > MAX_BOOK_SIZE) {
    fprintf(stderr, "Book too large: %ld bytes (max %d)\n",
            book_size, MAX_BOOK_SIZE);
    return -EFBIG;
}
```

#### Embedded Systems Best Practices

```c
/* Avoid dynamic allocation in critical paths when possible */
static char page_buffer[MAX_PAGE_SIZE];  /* Static allocation */

/* Minimize stack usage - avoid large local arrays */
/* BAD */
void process_data(void)
{
    char huge_buffer[65536];  /* 64KB on stack! */
    /* ... */
}

/* GOOD */
void process_data(void)
{
    char *buffer = malloc(65536);
    if (!buffer) return;
    /* ... */
    free(buffer);
}

/* Or use static for one-time allocations */
static char huge_buffer[65536];  /* In BSS segment */

/* Bounds checking is critical */
void copy_title(char *dest, const char *src, size_t dest_size)
{
    /* BAD: No bounds check */
    strcpy(dest, src);

    /* GOOD: Safe copy with bounds */
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  /* Ensure null termination */
}

/* Avoid busy loops - use sleep/select/poll */
/* BAD */
while (display_busy()) {
    /* Wastes CPU */
}

/* GOOD */
while (display_busy()) {
    usleep(1000);  /* Sleep 1ms between polls */
}
```

### Documentation Style

Use Markdown for all documentation with structured front matter:

```markdown
---
type: reference | guide | architecture | research
title: Document Title
created: YYYY-MM-DD
tags:
  - tag1
  - tag2
related:
  - "[[Other-Document]]"
---

# Document Title

Clear introduction paragraph.

## Section Heading

Content with examples and explanations.
```

## Building and Testing

### Quick Build

```bash
# Full build from scratch
./scripts/build.sh

# Or step-by-step
make config
make build
```

### Incremental Builds

After making changes to source code:

```bash
# Rebuild only the changed package
cd buildroot
make BR2_EXTERNAL=.. ereader-rebuild

# Or rebuild kernel
make BR2_EXTERNAL=.. linux-rebuild

# Or rebuild everything
make BR2_EXTERNAL=.. clean all
```

### Testing Your Changes

#### Unit Tests (Future)

```bash
# Run unit tests (when implemented)
make test

# Run specific test suite
make test-book-manager
make test-framebuffer
```

#### Manual Testing

```bash
# Flash to SD card
sudo dd if=buildroot/output/images/sdcard.img of=/dev/sdX bs=4M status=progress && sync

# Boot on hardware and test via serial console
screen /dev/ttyUSB0 115200

# Login: root / password: ereader
```

#### Memory Leak Detection

```bash
# Use Valgrind on desktop build (cross-compilation)
# This requires special build configuration
make valgrind-test
```

### Code Quality Checks

```bash
# Check for common issues (when scripts are available)
./scripts/check-code-style.sh src/ereader/

# Static analysis
make static-analysis
```

## Making Changes

### Branching Strategy

We use a feature branch workflow:

```bash
# Update your local repository
git checkout master
git pull upstream master

# Create a feature branch
git checkout -b feature/add-mobi-support

# Or for bug fixes
git checkout -b fix/memory-leak-in-pdf-reader
```

### Branch Naming Conventions

- `feature/description` - New features
- `fix/description` - Bug fixes
- `refactor/description` - Code refactoring
- `docs/description` - Documentation updates
- `test/description` - Test additions/improvements

### Commit Guidelines

#### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no logic change)
- `refactor`: Code refactoring
- `test`: Adding/updating tests
- `chore`: Build system, dependencies, tooling

**Examples:**

```
feat: Add MOBI format support

Implement MOBI file parsing using libmobi library. Includes:
- MOBI file detection and validation
- Text extraction from MOBI containers
- Integration with format_interface API
- Unit tests for MOBI parsing

Closes #45
```

```
fix: Memory leak in PDF text extraction

Fixed memory leak in pdf_reader.c where allocated text buffer
was not freed on error paths. Added proper cleanup in goto
error handling.

Issue: #123
```

```
docs: Update build guide with WSL2 instructions

Added detailed instructions for building on Windows using WSL2,
including common pitfalls and performance tips.
```

#### Commit Best Practices

- **One logical change per commit** - Don't mix unrelated changes
- **Write clear commit messages** - Explain WHY, not just WHAT
- **Reference issues** - Use "Fixes #123" or "Closes #45"
- **Keep commits atomic** - Each commit should build and work
- **Sign your commits** (optional but encouraged)

```bash
# Configure Git for signing
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Sign commits
git commit -s -m "feat: Add feature"
```

### Making Changes

1. **Write code** following the coding standards
2. **Test your changes** on hardware or emulator
3. **Update documentation** if needed
4. **Add tests** for new functionality
5. **Run code quality checks**

### Keeping Your Branch Updated

```bash
# Regularly sync with upstream
git fetch upstream
git rebase upstream/master

# Resolve conflicts if any
git status
# ... fix conflicts ...
git add <files>
git rebase --continue
```

## Pull Request Process

### Before Submitting

Checklist before creating a pull request:

- [ ] Code follows the project coding standards
- [ ] All tests pass (if applicable)
- [ ] Documentation is updated
- [ ] Commit messages are clear and follow conventions
- [ ] Branch is up-to-date with master
- [ ] No unnecessary files included (build artifacts, temp files)
- [ ] Code has been tested on hardware (if applicable)

### Creating a Pull Request

1. **Push your branch to your fork:**

```bash
git push origin feature/add-mobi-support
```

2. **Open a pull request on GitHub:**
   - Go to the original repository
   - Click "New Pull Request"
   - Select your branch
   - Fill out the PR template

### Pull Request Template

```markdown
## Description

Brief description of the changes.

## Type of Change

- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Changes Made

- Detailed list of changes
- What was added/removed/modified

## Testing

Describe how you tested your changes:
- [ ] Tested on actual hardware
- [ ] Tested with emulator/simulator
- [ ] Unit tests added/updated
- [ ] Manual testing performed

## Screenshots/Videos (if applicable)

Add screenshots or videos demonstrating the feature.

## Related Issues

Closes #issue_number

## Checklist

- [ ] My code follows the project's coding standards
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have updated the documentation accordingly
- [ ] My changes generate no new warnings or errors
- [ ] I have tested on hardware (or explained why not needed)
```

### Review Process

1. **Automated checks** run on your PR (when CI/CD is set up)
2. **Maintainers review** your code and provide feedback
3. **Address feedback** by pushing new commits to your branch
4. **Approval** - Once approved, a maintainer will merge

### After Your PR is Merged

```bash
# Update your local repository
git checkout master
git pull upstream master

# Delete your feature branch (optional)
git branch -d feature/add-mobi-support
git push origin --delete feature/add-mobi-support
```

## Reporting Issues

### Before Reporting

- **Search existing issues** - Your issue may already be reported
- **Check documentation** - The answer might already be documented
- **Try latest version** - The issue might be fixed

### Issue Template

When creating an issue, include:

**Bug Reports:**

```markdown
## Description

Clear description of the bug.

## Steps to Reproduce

1. Step one
2. Step two
3. Step three

## Expected Behavior

What you expected to happen.

## Actual Behavior

What actually happened.

## Environment

- Hardware: Raspberry Pi Zero W / Pi Zero 2 W
- Display: Waveshare 4.2" Rev 2.2
- Build: Commit hash or version
- SD Card: Size and class

## Logs/Screenshots

Paste relevant logs or add screenshots.

## Additional Context

Any other relevant information.
```

**Feature Requests:**

```markdown
## Feature Description

Clear description of the proposed feature.

## Use Case

Why is this feature needed? What problem does it solve?

## Proposed Implementation

If you have ideas on how to implement this, describe them.

## Alternatives Considered

Other approaches you've thought about.

## Additional Context

Any other relevant information.
```

### Priority Labels

Maintainers will apply labels:
- `critical` - System crashes, data loss
- `high` - Major functionality broken
- `medium` - Feature not working as expected
- `low` - Minor issues, cosmetic problems

## Communication

### Channels

- **GitHub Issues** - Bug reports, feature requests, discussions
- **GitHub Discussions** - General questions, ideas, community
- **Pull Requests** - Code review and technical discussions

### Getting Help

If you're stuck:

1. **Check documentation** in `docs/`
2. **Search closed issues** for similar problems
3. **Ask in GitHub Discussions**
4. **Be patient** - Maintainers are volunteers

### Response Times

This is a volunteer project. Please be patient:
- Issues: Expect response within 1-7 days
- Pull requests: Review within 1-14 days
- Complex changes may take longer

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see LICENSE file). Ensure you have the right to contribute the code.

## Recognition

Contributors are recognized in:
- Git commit history (permanent record)
- CONTRIBUTORS.md file (if exists)
- Release notes for significant contributions

Thank you for contributing to the Open Source E-Reader project! Your efforts help make this project better for everyone.

## Questions?

If you have questions about contributing, feel free to:
- Open a GitHub Discussion
- Comment on a related issue
- Reach out to maintainers

---

**Happy Contributing!** 🎉📚
