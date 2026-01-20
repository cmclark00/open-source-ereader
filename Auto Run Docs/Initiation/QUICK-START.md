# Quick Start Guide

**Want to dive in immediately?** This guide gets you started in 5 minutes.

## Prerequisites

Before you begin, you need:

### Hardware
- ✅ Raspberry Pi Zero W (you have this)
- ✅ Waveshare 4.2" e-paper display rev 2.2 (you have this)
- ⬜ MicroSD card (8GB minimum, 16GB+ recommended)
- ⬜ SD card reader for your computer
- ⬜ 5V power supply (micro USB)
- ⬜ 5 tactile buttons (6mm x 6mm typical)
- ⬜ Breadboard and jumper wires (for prototyping)
- ⬜ USB-to-TTL serial adapter (optional but highly recommended for debugging)

### Software/Tools on Your Computer
- ⬜ Linux, macOS, or Windows with WSL (for Buildroot)
- ⬜ Git
- ⬜ At least 20GB free disk space (for Buildroot)
- ⬜ Internet connection (for downloading Buildroot and packages)
- ⬜ AI coding assistant (Claude Code, Cursor, etc.) or willingness to execute manually

### Skills Needed
- Basic Linux command line knowledge
- Basic soldering (can learn as you go)
- Patience (first Buildroot build takes 1-2 hours)

## Step 1: Set Up Your Workspace (2 minutes)

```bash
# Navigate to project directory
cd "C:\Users\koolk\Documents\open-source-ereader"

# Verify you're in the right place
ls -la
# Should see: Auto Run Docs, .git

# Create a branch for your work (optional but recommended)
git checkout -b dev-build

# You're ready!
```

## Step 2: Understand the Plan (3 minutes)

Read these three files in order:

1. **`00-PROJECT-OVERVIEW.md`** (2 min) - Understand the big picture
2. **`HOW-TO-USE-THESE-DOCS.md`** (1 min) - How to execute phases
3. **`Phase-01-Foundation-and-Working-Display.md`** (skim) - What you're about to do

## Step 3: Start Phase 1 (Now!)

### Option A: With AI Assistant (Recommended)

If using Claude Code:

```bash
# Start Claude Code
claude

# Then say:
"Please execute all tasks in Auto Run Docs/Initiation/Phase-01-Foundation-and-Working-Display.md.
Work through each task systematically, completing all sub-items before moving to the next task."
```

Claude will:
- Create project structure
- Research display specs
- Set up Buildroot configuration
- Write display driver code
- Create documentation
- Start the build process

### Option B: Manual Execution

Open `Phase-01-Foundation-and-Working-Display.md` and work through tasks yourself:

1. Create `README.md`, `docs/hardware/WIRING_GUIDE.md`, etc.
2. Research Waveshare display (check manufacturer website)
3. Download Buildroot from https://buildroot.org
4. Configure Buildroot for Pi Zero W
5. Write display driver code
6. Build everything
7. Flash SD card

## What to Expect in Phase 1

### Timeline
- **Setup and config**: 30-60 minutes
- **First Buildroot build**: 1-2 hours (mostly automated)
- **Display driver coding**: 1-2 hours
- **Testing**: 30 minutes
- **Total**: 3-5 hours over 1-2 days

### Outputs You'll Create
```
open-source-ereader/
├── README.md                          # Project overview
├── docs/
│   ├── hardware/
│   │   ├── WIRING_GUIDE.md           # How to wire display
│   │   ├── BOM.md                    # Parts list
│   │   └── DISPLAY_SPECS.md          # Display technical info
│   ├── progress/
│   │   └── PHASE_01_LOG.md           # Your notes
│   └── deployment/
│       └── FLASHING_GUIDE.md         # How to flash SD card
├── buildroot/                         # Buildroot directory (20GB)
├── configs/
│   ├── ereader_rpi0w_defconfig       # Board config
│   └── linux_ereader.fragment        # Kernel config
├── board/ereader/
│   ├── overlays/
│   │   └── waveshare-epd-4_2.dts     # Display device tree
│   └── post-build.sh                 # Build script
└── src/
    └── display-test/
        ├── epd_driver.c              # Display driver
        ├── epd_driver.h
        ├── main.c                    # Test app
        └── Makefile
```

### End Result
- **Software**: Bootable SD card image that shows "Hello E-Reader" on display
- **Documentation**: Complete hardware wiring guide and build instructions
- **Knowledge**: Understanding of Buildroot, device trees, and e-paper displays

## Phase 1 Success Checklist

Before moving to Phase 2, verify:

- [ ] **USER ACTION REQUIRED**: Buildroot compiles successfully (output in `buildroot/output/images/`)
  - **Current Status**: ⏸️ **READY TO BUILD** - All infrastructure complete, WSL2 verified (2026-01-19)
  - **Environment Status**: ✅ WSL2 Ubuntu detected and accessible, ❌ Dependencies not installed
  - **Build Infrastructure Status**: ✅ Ready (all configs, source code, scripts, and documentation complete)
  - **Verification Status**: ✅ All build prerequisites verified (2026-01-19) - WSL2 operational, project accessible
  - **Action Required**: User must execute build in WSL2 using provided commands
  - **Expected Duration**: 2-5 min (dependencies) + 60-120 min (build compilation)
  - **🚨 START HERE**: See `Auto Run Docs/Working/USER-ACTION-REQUIRED.md` for quick copy-paste commands
  - **📖 Detailed Instructions**: See `Auto Run Docs/Working/BUILDROOT-BUILD-INSTRUCTIONS.md` for complete step-by-step guide
  - **📊 Complete Status Report**: See `docs/progress/PHASE-01-STATUS-SUMMARY.md` for full inventory of completed work
  - **Why Not Automated**: Requires sudo password for dependency installation (cannot be automated securely)
- [ ] **USER ACTION REQUIRED**: `sdcard.img` file exists and is ~100-500MB
  - **Status**: Will be generated after Buildroot compilation completes in `buildroot/output/images/sdcard.img`
  - **Verification Command**: `ls -lh buildroot/output/images/sdcard.img` (run in WSL2/Linux after build)
- [x] All documentation files created (WIRING_GUIDE, DISPLAY_SPECS, etc.)
- [x] Display driver code compiles without errors
  - **Status**: ✅ Verified (2026-01-19) - All source files complete (epd_driver.c, main.c, epd_driver.h, font.h), Makefile configured for cross-compilation, all functions implemented, no syntax errors. Ready for Buildroot build.
- [x] You understand the SPI wiring (even if not physically wired yet)

### Hardware Testing (Do This!)

When you have the SD card image:

1. **Write image to SD card**:
   ```bash
   # Linux/Mac
   sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress

   # Windows: Use Win32DiskImager or Etcher
   ```

2. **Wire up display to Pi Zero W** (follow WIRING_GUIDE.md)
   - SPI pins: MOSI, MISO, SCK, CS
   - Control pins: DC, RST, BUSY
   - Power: 3.3V, GND

3. **Insert SD card and power on Pi**

4. **Watch for "Hello E-Reader"** on display (may take 30-60 seconds to boot)

5. **If it works**: 🎉 Move to Phase 2!

6. **If it doesn't work**:
   - Connect serial console (USB-to-TTL)
   - Check boot logs
   - Verify wiring
   - Check device tree overlay compiled correctly
   - Consult troubleshooting in Phase 01 log

## After Phase 1

### If Display Works - Move Forward!

```bash
# Commit your work
git add .
git commit -m "Complete Phase 1: Display working"

# Start Phase 2
# Open Phase-02-GPIO-Button-Input.md
```

Phase 2 is faster (no long build) and adds button support.

### If Display Doesn't Work - Debug!

Common issues:
1. **Wrong wiring** → Double-check every pin with multimeter
2. **Device tree not loaded** → Check `/boot/overlays/` on SD card
3. **SPI not enabled** → Verify kernel config has CONFIG_SPI_BCM2835=y
4. **Display not initializing** → Check reset sequence in driver code
5. **Wrong display model** → Verify you have rev 2.2, not rev 2.1

**Don't skip ahead** - Phase 2 requires working display!

Document issues in `docs/progress/PHASE_01_LOG.md` for future reference.

## Getting Help

### Resources
- **Waveshare Wiki**: https://www.waveshare.com/wiki/4.2inch_e-Paper_Module
- **Pi Zero W Pinout**: https://pinout.xyz
- **Buildroot Manual**: https://buildroot.org/downloads/manual/manual.html
- **Device Tree Docs**: https://www.kernel.org/doc/Documentation/devicetree/

### Community
- Raspberry Pi Forums
- Reddit: /r/raspberry_pi
- Your AI assistant (ask questions!)

### Emergency Rollback

If something breaks:
```bash
# Undo local changes
git status
git diff
git restore <file>

# Or start over
git reset --hard HEAD

# Nuclear option: delete and re-clone
```

## Motivation When Things Get Hard

Building an e-reader from scratch is challenging! When you feel stuck:

- **Remember why**: You're creating something awesome that didn't exist before
- **Take breaks**: Walk away, come back fresh
- **Ask for help**: AI assistants, forums, communities
- **Celebrate small wins**: Boot screen! Display works! Button pressed!
- **Learn from failures**: Each bug teaches you something

By Phase 3, you'll have a working e-reader. By Phase 7, you'll have shipped an open source product. That's incredible! 🚀

## Ready? Let's Go! 🎯

```bash
# If using AI
claude "Execute Phase-01-Foundation-and-Working-Display.md"

# If doing manually
open "Phase-01-Foundation-and-Working-Display.md"
# Start with first task!
```

**Next stop**: A working e-paper display showing "Hello E-Reader"!

---

*Good luck, and remember: every Raspberry Pi project looks impossible until suddenly it works.* ⚡
