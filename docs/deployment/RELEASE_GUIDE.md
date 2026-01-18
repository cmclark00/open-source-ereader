---
type: reference
title: Release Build and Distribution Guide
created: 2026-01-18
tags:
  - deployment
  - release
  - distribution
  - build-process
related:
  - "[[BUILD_GUIDE]]"
  - "[[FLASHING_GUIDE]]"
  - "[[BUILD_PROCESS]]"
---

# Release Build and Distribution Guide

This guide documents the complete process for creating release artifacts for the Open Source E-Reader project. Follow these steps to prepare official releases for public distribution.

**Audience:** Project maintainers preparing official releases
**Prerequisites:** Complete development environment with Buildroot configured
**Version:** v1.0.0

---

## Table of Contents

1. [Release Checklist](#release-checklist)
2. [Version Number Management](#version-number-management)
3. [Building the Final Image](#building-the-final-image)
4. [Including Sample Books](#including-sample-books)
5. [Optimizing Default Configuration](#optimizing-default-configuration)
6. [Compressing and Checksumming](#compressing-and-checksumming)
7. [Creating Release Notes](#creating-release-notes)
8. [GitHub Release Process](#github-release-process)
9. [Verification and Testing](#verification-and-testing)
10. [Troubleshooting](#troubleshooting)

---

## Release Checklist

Before creating a release, ensure all of the following are complete:

### Pre-Release Requirements

- [ ] All planned features for this version are complete and tested
- [ ] All critical bugs are fixed
- [ ] Documentation is up-to-date
- [ ] Version numbers are updated in all files
- [ ] CHANGELOG.md is updated with release notes
- [ ] All tests pass (hardware and software)
- [ ] Code review is complete
- [ ] README.md reflects current state

### Build Requirements

- [ ] Clean Buildroot build from scratch completes successfully
- [ ] Final image boots correctly on Pi Zero W
- [ ] All features work on final image
- [ ] Sample books are included
- [ ] Default configuration is optimized
- [ ] Image size is reasonable (<500 MB)

### Distribution Requirements

- [ ] Image is compressed with gzip or xz
- [ ] SHA256 checksum is generated
- [ ] Release notes are written
- [ ] Assets are uploaded to GitHub Releases
- [ ] Documentation links are verified
- [ ] Community is notified (forums, social media)

---

## Version Number Management

The version number must be updated in multiple locations before building a release.

### Version Format

This project uses [Semantic Versioning](https://semver.org/):

```
MAJOR.MINOR.PATCH

Examples:
- 1.0.0 - First public release
- 1.1.0 - New features added (backward compatible)
- 1.0.1 - Bug fixes only
- 2.0.0 - Breaking changes
```

### Files to Update

**1. Source Code: `src/ereader/ereader.h`**

Update the version macros:

```c
/*
 * Application version information
 */
#define EREADER_VERSION_MAJOR   1
#define EREADER_VERSION_MINOR   0
#define EREADER_VERSION_PATCH   0
#define EREADER_VERSION_STRING  "1.0.0"
```

**2. Buildroot Package: `package/ereader/ereader.mk`**

Update the package version:

```makefile
EREADER_VERSION = 1.0.0
```

**3. Changelog: `CHANGELOG.md`**

Add release entry at the top:

```markdown
## [1.0.0] - 2026-01-18

### Summary
First public release with complete feature set.

### Added
- Feature 1
- Feature 2
...
```

**4. README.md**

Update the version badge/status if present:

```markdown
**Current Version:** 1.0.0
```

### Verification

After updating, verify all versions are consistent:

```bash
# Check source code version
grep "EREADER_VERSION" src/ereader/ereader.h

# Check package version
grep "EREADER_VERSION" package/ereader/ereader.mk

# Check changelog
head -20 CHANGELOG.md
```

---

## Building the Final Image

### Clean Build Process

**CRITICAL:** Always perform a clean build for releases to avoid stale artifacts.

#### Step 1: Clean Previous Build

```bash
cd buildroot
make clean
```

**Note:** This removes all build artifacts but preserves downloaded source packages.

For a completely fresh build (including re-downloading sources):

```bash
make distclean
```

#### Step 2: Configure Buildroot

Load the e-reader configuration:

```bash
make ereader_rpi0w_defconfig
```

Verify configuration:

```bash
make menuconfig
# Review settings, then exit and save
```

#### Step 3: Verify Version Embedding

Ensure the version will be embedded correctly:

```bash
# Check that ereader package will be rebuilt
make ereader-dirclean

# Verify package version
make ereader-show-info | grep VERSION
```

Expected output:
```
EREADER_VERSION = 1.0.0
```

#### Step 4: Full Build

Start the full build process:

```bash
make -j$(nproc)
```

**Build time:** 1-2 hours on first build, 30-60 minutes on subsequent builds.

**Expected output location:** `output/images/sdcard.img`

#### Step 5: Verify Build Success

Check that the image was created:

```bash
ls -lh output/images/sdcard.img
```

Expected size: 100-300 MB (before compression)

---

## Including Sample Books

Sample books provide a better out-of-box experience for users.

### Recommended Sample Books

Include 3-5 public domain books from Project Gutenberg:

1. **Small TXT file** (~100 KB) - Fast test
2. **Medium EPUB** (~500 KB) - Common use case
3. **Large PDF** (~2-5 MB) - Stress test

### Example Book Selection

```
Alice's Adventures in Wonderland (Lewis Carroll)
- EPUB: ~200 KB
- Public domain, widely known
- Good typography test

The Adventures of Sherlock Holmes (Arthur Conan Doyle)
- EPUB: ~300 KB
- Multi-chapter structure
- Tests navigation

Pride and Prejudice (Jane Austen)
- PDF: ~800 KB
- Classic literature
- Tests PDF rendering
```

### Adding Books to Image

**Option 1: Modify Rootfs Overlay (Recommended)**

Create books directory in overlay:

```bash
# From project root
mkdir -p board/ereader/rootfs-overlay/books

# Download sample books
cd board/ereader/rootfs-overlay/books

# Download from Project Gutenberg
wget https://www.gutenberg.org/ebooks/11.epub.images -O alice-in-wonderland.epub
wget https://www.gutenberg.org/ebooks/1342.epub.images -O pride-and-prejudice.epub
wget https://www.gutenberg.org/files/1661/1661-0.txt -O sherlock-holmes.txt
```

**Option 2: Post-Build Script**

Create a post-build script:

```bash
# board/ereader/post-build.sh
#!/bin/bash
set -e

TARGET_DIR=$1
BOOKS_DIR="${TARGET_DIR}/books"

# Create books directory
mkdir -p "${BOOKS_DIR}"

# Copy sample books
cp "${BR2_EXTERNAL_EREADER_PATH}/sample-books/"*.epub "${BOOKS_DIR}/"
cp "${BR2_EXTERNAL_EREADER_PATH}/sample-books/"*.pdf "${BOOKS_DIR}/"
cp "${BR2_EXTERNAL_EREADER_PATH}/sample-books/"*.txt "${BOOKS_DIR}/"

# Set permissions
chmod 644 "${BOOKS_DIR}"/*
```

Add to defconfig:

```
BR2_ROOTFS_POST_BUILD_SCRIPT="$(BR2_EXTERNAL_EREADER_PATH)/board/ereader/post-build.sh"
```

**Option 3: Add After Image Creation**

Mount the image and add books:

```bash
# Create mount point
sudo mkdir -p /mnt/ereader

# Mount the image (partition 2 is rootfs)
sudo mount -o loop,offset=$((8192*512)) output/images/sdcard.img /mnt/ereader

# Copy books
sudo mkdir -p /mnt/ereader/books
sudo cp sample-books/* /mnt/ereader/books/

# Unmount
sudo umount /mnt/ereader
```

### License Compliance

All sample books MUST be:
- Public domain (published before 1928 in US)
- Properly attributed
- Include Project Gutenberg header if sourced from there

Create `books/README.txt` with attribution:

```
Sample Books Included with Open Source E-Reader

These books are in the public domain and sourced from Project Gutenberg:
https://www.gutenberg.org

1. Alice's Adventures in Wonderland by Lewis Carroll
   - Project Gutenberg eBook #11

2. Pride and Prejudice by Jane Austen
   - Project Gutenberg eBook #1342

3. The Adventures of Sherlock Holmes by Arthur Conan Doyle
   - Project Gutenberg eBook #1661

For the complete Project Gutenberg License, visit:
https://www.gutenberg.org/policy/license.html
```

---

## Optimizing Default Configuration

Ensure the default configuration provides the best user experience.

### Settings File

Edit `board/ereader/rootfs-overlay/etc/ereader/settings.conf`:

```ini
# Display Settings
font_size=16          # Medium size, good readability
margin_left=10        # Comfortable margins
margin_right=10
margin_top=15
margin_bottom=15
line_spacing=4        # Slightly loose for readability

# Power Settings
sleep_timeout=300     # 5 minutes
auto_sleep=1          # Enable automatic sleep
battery_warning=15    # Warn at 15% battery

# Display Settings
full_refresh_pages=10 # Full refresh every 10 pages
invert_colors=0       # Normal black-on-white

# Network Settings
wifi_enabled=0        # Start with WiFi off (save power)
time_sync=1           # Enable time sync when WiFi connects
```

### Boot Optimization

Verify fast boot configuration in `configs/ereader_rpi0w_defconfig`:

```
# Minimal kernel modules
# Fast init system
# No unnecessary services
# Pre-generated SSH keys (if needed)
```

### First Boot Experience

Test the first boot experience:

1. Flash fresh image to SD card
2. Boot Pi Zero W with display attached
3. Time boot process (target: <12 seconds)
4. Verify sample books appear
5. Test navigation and reading
6. Verify all features work

---

## Compressing and Checksumming

### Image Compression

Compress the image for distribution:

```bash
cd buildroot/output/images

# Option 1: gzip compression (faster, larger)
gzip -9 -k sdcard.img
# Creates: sdcard.img.gz
# Compression ratio: ~50-60%

# Option 2: xz compression (slower, smaller - RECOMMENDED)
xz -9 -k sdcard.img
# Creates: sdcard.img.xz
# Compression ratio: ~30-40%
```

**Recommendation:** Use xz compression for releases. The smaller size reduces download time significantly.

### Checksums

Generate SHA256 checksums for integrity verification:

```bash
# Generate checksum
sha256sum sdcard.img.xz > sdcard.img.xz.sha256

# Verify format
cat sdcard.img.xz.sha256
```

Expected output format:
```
a1b2c3d4e5f6... sdcard.img.xz
```

### Rename for Release

Use descriptive filenames:

```bash
# Format: ereader-v{VERSION}-{BOARD}-{DATE}.img.xz
mv sdcard.img.xz ereader-v1.0.0-rpi0w-20260118.img.xz
mv sdcard.img.xz.sha256 ereader-v1.0.0-rpi0w-20260118.img.xz.sha256
```

### File Size Verification

Check final sizes:

```bash
ls -lh ereader-v1.0.0-rpi0w-20260118.*
```

Expected sizes:
- Uncompressed: 100-300 MB
- Compressed (xz): 30-120 MB
- SHA256 file: ~100 bytes

---

## Creating Release Notes

### CHANGELOG.md Format

The CHANGELOG.md should follow [Keep a Changelog](https://keepachangelog.com/) format:

```markdown
## [1.0.0] - 2026-01-18

### Summary
Brief overview of this release (2-3 sentences).

### Added
- New feature 1
- New feature 2

### Changed
- Modified behavior 1
- Updated component 2

### Fixed
- Bug fix 1
- Bug fix 2

### Known Issues
- Known limitation 1
- Known limitation 2
```

### GitHub Release Notes

Create more detailed release notes for GitHub:

**Template:**

```markdown
# Open Source E-Reader v1.0.0

**Release Date:** January 18, 2026
**Type:** Major Release

## What's New

This is the first public release of the Open Source E-Reader project! 🎉

Build your own Kindle-like e-reader using a $15 Raspberry Pi Zero W and a $30 e-paper display.

### Key Features

- 📚 **Multi-format support** - Read EPUB, PDF, and TXT files
- ⚡ **Fast boot** - 10-second startup from power-on to library
- 🔋 **Long battery life** - Week-long reading on a single charge
- 📡 **WiFi connectivity** - Download books from Project Gutenberg
- 🔒 **Privacy-respecting** - No tracking, no DRM, no cloud dependency
- 🛠️ **100% open source** - All code, hardware designs, and documentation

### What's Included

- Pre-built SD card image (ready to flash and use)
- Complete build instructions
- Hardware assembly guide
- Bill of materials with supplier links
- 3D printable case designs
- Comprehensive documentation

### System Requirements

**Hardware:**
- Raspberry Pi Zero W
- Waveshare 4.2" e-paper display (Rev 2.2)
- 5 GPIO buttons
- 8GB+ microSD card
- 5V power supply or 18650 battery

**Total Cost:** ~$60

## Downloads

### SD Card Image

**File:** `ereader-v1.0.0-rpi0w-20260118.img.xz`
**Size:** ~80 MB compressed, ~200 MB uncompressed
**SHA256:** See `.sha256` file

**Quick Start:**
```bash
# Extract image
unxz ereader-v1.0.0-rpi0w-20260118.img.xz

# Flash to SD card (Linux/macOS)
sudo dd if=ereader-v1.0.0-rpi0w-20260118.img of=/dev/sdX bs=4M status=progress
sync
```

Or use [balenaEtcher](https://www.balena.io/etcher/) (Windows/macOS/Linux)

### Source Code

Use the "Source code (zip)" or "Source code (tar.gz)" links below, or:

```bash
git clone https://github.com/cmclark00/open-source-ereader.git
cd open-source-ereader
git checkout v1.0.0
```

## Documentation

- **[Build Guide](docs/BUILD_GUIDE.md)** - Step-by-step assembly
- **[User Guide](docs/USER_GUIDE.md)** - How to use your e-reader
- **[Bill of Materials](docs/hardware/BOM.md)** - Parts list and suppliers
- **[Flashing Guide](docs/deployment/FLASHING_GUIDE.md)** - SD card preparation

## What's Working

✅ All core features implemented and tested:
- E-paper display driver
- Book library management
- EPUB, PDF, TXT format support
- Navigation and page turning
- Bookmark persistence
- WiFi connectivity
- Settings menu
- Search functionality
- Battery monitoring
- Power management

## Known Limitations

- Screen size: 4.2" (smaller than commercial 6" e-readers)
- Resolution: 400×300 pixels
- Boot time: ~10 seconds (vs 3-5 for commercial devices)
- Display refresh: Full refresh only (no partial updates)
- PDF rendering: Text extraction (no complex layout)

## Upgrading

**From:** N/A (first release)

**Breaking Changes:** None

## Contributing

We welcome contributions! See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for:
- Development environment setup
- Coding standards
- Pull request process
- How to report bugs

## Support

- 📖 [Documentation](docs/)
- 🐛 [Issue Tracker](https://github.com/cmclark00/open-source-ereader/issues)
- 💬 [Discussions](https://github.com/cmclark00/open-source-ereader/discussions)

## License

- Software: GPL v3
- Hardware: CERN OHL
- Documentation: CC BY-SA 4.0

See [LEGAL.md](docs/LEGAL.md) for details.

## Acknowledgments

Thanks to:
- Waveshare for e-paper displays and documentation
- Buildroot Project for embedded Linux tools
- Raspberry Pi Foundation for accessible hardware
- All contributors and testers

---

**Ready to build your own e-reader?** Download the image above and check out the [Build Guide](docs/BUILD_GUIDE.md)!
```

---

## GitHub Release Process

### Prerequisites

- GitHub CLI (`gh`) installed, or use web interface
- Repository is pushed to GitHub
- All files are committed
- Tag is created

### Step 1: Create Git Tag

```bash
# Create annotated tag
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push tag to remote
git push origin v1.0.0
```

### Step 2: Create Release (CLI Method)

```bash
# Navigate to repository
cd /path/to/open-source-ereader

# Create release with gh CLI
gh release create v1.0.0 \
  --title "Open Source E-Reader v1.0.0" \
  --notes-file RELEASE_NOTES.md \
  buildroot/output/images/ereader-v1.0.0-rpi0w-20260118.img.xz \
  buildroot/output/images/ereader-v1.0.0-rpi0w-20260118.img.xz.sha256
```

### Step 3: Create Release (Web Method)

1. Go to repository on GitHub
2. Click "Releases" → "Draft a new release"
3. Choose tag: `v1.0.0`
4. Set title: "Open Source E-Reader v1.0.0"
5. Paste release notes (markdown formatted)
6. Upload files:
   - `ereader-v1.0.0-rpi0w-20260118.img.xz`
   - `ereader-v1.0.0-rpi0w-20260118.img.xz.sha256`
7. Check "Set as latest release"
8. Click "Publish release"

### Step 4: Verify Release

Visit the release page and verify:
- [ ] Release title is correct
- [ ] Tag points to correct commit
- [ ] Release notes are formatted properly
- [ ] Image file is attached
- [ ] SHA256 file is attached
- [ ] Download links work
- [ ] Files download correctly

---

## Verification and Testing

Before announcing the release, perform final verification.

### Image Verification

**1. Download and Verify Checksum**

```bash
# Download from GitHub
wget https://github.com/cmclark00/open-source-ereader/releases/download/v1.0.0/ereader-v1.0.0-rpi0w-20260118.img.xz
wget https://github.com/cmclark00/open-source-ereader/releases/download/v1.0.0/ereader-v1.0.0-rpi0w-20260118.img.xz.sha256

# Verify checksum
sha256sum -c ereader-v1.0.0-rpi0w-20260118.img.xz.sha256
```

Expected output: `ereader-v1.0.0-rpi0w-20260118.img.xz: OK`

**2. Extract and Flash**

```bash
# Extract
unxz ereader-v1.0.0-rpi0w-20260118.img.xz

# Flash to SD card
sudo dd if=ereader-v1.0.0-rpi0w-20260118.img of=/dev/sdX bs=4M status=progress
sync
```

**3. Hardware Testing**

Test on actual hardware:

- [ ] Boots successfully
- [ ] Display initializes
- [ ] Sample books appear
- [ ] All buttons work
- [ ] Can open and read books
- [ ] Can navigate menus
- [ ] WiFi connects (test on known network)
- [ ] Settings save and persist
- [ ] Power management works
- [ ] Version number shows "1.0.0"

**4. First-Time User Testing**

Ideally, have someone unfamiliar with the project test:

- Follow build guide from scratch
- Identify unclear instructions
- Report any issues
- Provide feedback on documentation

### Documentation Verification

- [ ] All links in README.md work
- [ ] All links in documentation work
- [ ] Build guide is accurate
- [ ] BOM part numbers are correct
- [ ] Wiring diagrams match current hardware
- [ ] No references to old versions

---

## Troubleshooting

### Build Issues

**Problem:** Build fails with "ereader-1.0.0.tar.gz not found"

**Solution:** The local site method doesn't use tarballs. Verify:
```makefile
EREADER_SITE_METHOD = local
```

---

**Problem:** Old version number appears in built binary

**Solution:** Force rebuild of ereader package:
```bash
make ereader-dirclean
make ereader-rebuild
```

---

**Problem:** Image size is too large (>500 MB)

**Solution:** Check for unnecessary packages:
```bash
# Review package list
make graph-size

# Remove large packages not needed:
# - Development tools
# - Debug symbols
# - Documentation
```

---

### Compression Issues

**Problem:** xz compression is very slow

**Solution:** Use fewer compression levels or gzip:
```bash
# Faster xz (less compression)
xz -6 -k sdcard.img

# Or use gzip
gzip -9 -k sdcard.img
```

---

**Problem:** Cannot verify SHA256 checksum

**Solution:** Ensure format matches:
```bash
# Format should be: <hash> <filename>
cat sdcard.img.xz.sha256

# Regenerate if needed
sha256sum sdcard.img.xz > sdcard.img.xz.sha256
```

---

### GitHub Release Issues

**Problem:** "tag does not exist" error

**Solution:** Ensure tag is pushed:
```bash
git push origin v1.0.0
```

---

**Problem:** File upload fails (file too large)

**Solution:** GitHub has 2 GB limit per file. Compress image:
```bash
# Should be well under 2 GB
ls -lh ereader-*.img.xz
```

---

### Testing Issues

**Problem:** Image boots but shows old version

**Solution:** Verify version was embedded:
```bash
# Check built binary
strings output/target/usr/bin/ereader | grep "1.0.0"

# Should show: "1.0.0"
```

---

**Problem:** Sample books don't appear

**Solution:** Verify books were included:
```bash
# Mount image and check
sudo mount -o loop,offset=$((8192*512)) output/images/sdcard.img /mnt/ereader
ls -la /mnt/ereader/books/
sudo umount /mnt/ereader
```

---

## Best Practices

### Release Frequency

- **Major releases (X.0.0):** Annually or when significant features added
- **Minor releases (1.X.0):** Quarterly or when new features ready
- **Patch releases (1.0.X):** As needed for critical bug fixes

### Release Timing

- Avoid releasing on Fridays (no support over weekend)
- Announce releases early in the week
- Allow time for community testing before announcement
- Don't rush releases to meet arbitrary deadlines

### Communication

- Post release announcement on:
  - GitHub Releases
  - Project website (if applicable)
  - Reddit (/r/raspberry_pi, /r/ereader)
  - Hackaday.io
  - Twitter/X
  - Hackster.io

- Include:
  - What's new
  - Download link
  - Documentation link
  - Photos/videos
  - Call to action

### Version Control

- Tag all releases in git
- Never delete released tags
- Never force-push to release tags
- Keep release branches for long-term support

### Documentation

- Update documentation BEFORE release
- Verify all links work
- Include migration guide for breaking changes
- Document known issues clearly

---

## Appendix A: Release Checklist Template

Copy this checklist for each release:

```markdown
# Release v1.0.0 Checklist

## Pre-Release
- [ ] All features complete
- [ ] All tests pass
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Version numbers updated (ereader.h, ereader.mk)

## Build
- [ ] Clean build successful
- [ ] Version embedded correctly
- [ ] Sample books included
- [ ] Image size acceptable (<500 MB)
- [ ] Configuration optimized

## Package
- [ ] Image compressed (xz)
- [ ] SHA256 generated
- [ ] Files renamed with version
- [ ] Release notes written

## Test
- [ ] Image flashes correctly
- [ ] Hardware boots
- [ ] All features work
- [ ] Version shows correctly
- [ ] Documentation accurate

## Distribute
- [ ] Git tag created and pushed
- [ ] GitHub release created
- [ ] Files uploaded
- [ ] Downloads verified
- [ ] Links work

## Announce
- [ ] GitHub Discussions post
- [ ] Reddit post
- [ ] Twitter announcement
- [ ] Community notified
```

---

## Appendix B: Automation Script

Future improvement: Automate release process with script.

**Example: `scripts/create-release.sh`**

```bash
#!/bin/bash
# Release automation script (future improvement)

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 1.0.0"
    exit 1
fi

set -e

echo "Creating release v${VERSION}..."

# Update version numbers
sed -i "s/EREADER_VERSION_STRING.*/EREADER_VERSION_STRING  \"${VERSION}\"/" src/ereader/ereader.h
sed -i "s/EREADER_VERSION = .*/EREADER_VERSION = ${VERSION}/" package/ereader/ereader.mk

# Build
cd buildroot
make clean
make ereader_rpi0w_defconfig
make -j$(nproc)

# Package
cd output/images
DATE=$(date +%Y%m%d)
FILENAME="ereader-v${VERSION}-rpi0w-${DATE}.img"
cp sdcard.img "${FILENAME}"
xz -9 "${FILENAME}"
sha256sum "${FILENAME}.xz" > "${FILENAME}.xz.sha256"

echo "Release v${VERSION} created:"
ls -lh "${FILENAME}.xz"*
```

---

## Appendix C: Sample Books Sources

### Project Gutenberg

Best source for public domain books:
- https://www.gutenberg.org

**Popular books for testing:**
- Alice's Adventures in Wonderland (#11)
- Pride and Prejudice (#1342)
- Sherlock Holmes (#1661)
- A Christmas Carol (#46)
- The Great Gatsby (#64317)

### Other Sources

- **Internet Archive:** https://archive.org/details/texts
- **Open Library:** https://openlibrary.org
- **Standard Ebooks:** https://standardebooks.org (high quality EPUB)

**License Verification:** Always verify public domain status before including.

---

## Revision History

| Version | Date       | Changes                        |
|---------|------------|--------------------------------|
| 1.0     | 2026-01-18 | Initial release guide created  |

---

*For questions about the release process, open an issue on GitHub or consult [[CONTRIBUTING]].*
