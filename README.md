# Open Source E-Reader Project

An affordable, open-source e-reader built on embedded Linux, designed for accessibility, customization, and community contribution.

## Project Goals

- Create a super affordable e-ink e-reader (target: under $50 in parts)
- Fully open-source hardware and software
- Run on a lightweight Linux-based OS
- Support standard e-book formats (EPUB, PDF, MOBI, TXT)
- Long battery life (weeks of reading)
- Easy to build and modify

## Project Status

This project is in the planning phase. We are currently:
- Selecting hardware components
- Designing the OS architecture
- Planning the software stack

## Repository Structure

```
open-source-ereader/
├── docs/               # Documentation
│   ├── hardware/      # Hardware specifications and BOM
│   ├── software/      # Software architecture docs
│   └── assembly/      # Build instructions
├── hardware/          # PCB designs, schematics, 3D models
├── os/               # Operating system components
│   ├── kernel/       # Kernel configs and patches
│   ├── rootfs/       # Root filesystem
│   └── buildroot/    # Buildroot configuration
├── software/         # Application software
│   ├── reader-app/   # E-book reading application
│   ├── ui/          # User interface framework
│   └── drivers/     # Custom drivers
└── tools/           # Build scripts and utilities
```

## Hardware Overview (Preliminary)

### Core Components
- **Display**: E-ink display (6" recommended)
- **Processor**: Low-power ARM SoC (e.g., Allwinner, Rockchip, or i.MX6)
- **Memory**: 256MB-512MB RAM minimum
- **Storage**: microSD card or 4-8GB eMMC
- **Battery**: Li-Po battery with charging circuit
- **Input**: Physical buttons and/or capacitive touch

## Software Stack (Preliminary)

- **Bootloader**: U-Boot
- **Kernel**: Linux kernel (mainline or vendor with minimal patches)
- **Init System**: BusyBox init or systemd-minimal
- **Display Server**: Direct framebuffer or minimal Wayland
- **Reading App**: Custom Qt/GTK app or adapt existing open-source readers
- **Document Rendering**: MuPDF, Poppler, or FBReader libraries

## Getting Started

Documentation is being actively developed. Check the `docs/` directory for the latest information.

## Contributing

This project welcomes contributions! Areas where we need help:
- Hardware design and component selection
- Kernel and driver development
- UI/UX design
- Documentation
- Testing

## License

TBD - Likely GPL v3 for software, CERN-OHL or similar for hardware

## Resources

- [E-ink Display Technology](https://en.wikipedia.org/wiki/E_Ink)
- [Buildroot Documentation](https://buildroot.org/docs.html)
- [Embedded Linux Wiki](https://elinux.org/)
