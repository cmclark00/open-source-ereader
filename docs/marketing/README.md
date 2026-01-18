---
type: note
title: Marketing Materials Overview
created: 2026-01-17
tags:
  - marketing
  - overview
related:
  - "[[DEMO_CONTENT_GUIDE]]"
  - "[[MARKETING_COPY]]"
  - "[[PRESS_KIT]]"
---

# Marketing Materials - Overview

This directory contains comprehensive marketing materials, demo content guidelines, and promotional resources for the Open Source E-Reader project.

## Directory Contents

### Core Documentation

**[[DEMO_CONTENT_GUIDE]]** (`DEMO_CONTENT_GUIDE.md`)
- Complete guide for creating demo videos, photos, and GIFs
- Shot lists with timing and technical specifications
- Equipment recommendations and filming tips
- Post-production workflow and optimization techniques
- **Use this when:** You're ready to create actual demo content with assembled hardware

**[[MARKETING_COPY]]** (`MARKETING_COPY.md`)
- Pre-written taglines, descriptions, and messaging
- Feature descriptions (technical and user-friendly versions)
- Use case narratives and storytelling templates
- FAQ content for common questions
- Community messaging templates
- **Use this when:** Writing social media posts, documentation, or promotional materials

**[[PRESS_KIT]]** (`PRESS_KIT.md`)
- Complete press kit with fact sheets and specifications
- Target audience analysis and use cases
- Comparison charts with commercial e-readers
- Sample headlines and boilerplate text
- Media asset inventory and licensing information
- **Use this when:** Reaching out to media or preparing launch announcements

## Current Status

### ✅ Completed
- [x] Comprehensive demo content creation guide (1000+ lines)
- [x] Marketing copy templates and messaging framework
- [x] Press kit with technical specifications and media resources
- [x] Social media content templates (Twitter, Reddit, Hacker News, etc.)
- [x] Video shot list with timing and narration scripts
- [x] Photography guidelines with required shots
- [x] GIF creation instructions and optimization techniques

### ⏳ Pending (Requires Hardware)
- [ ] Actual demo videos (boot sequence, reading experience, WiFi features)
- [ ] Professional product photography (hero shot, component layout, wiring details)
- [ ] Animated GIFs (page turning, menu navigation, settings)
- [ ] Screenshots from actual device (UI screens, boot process)

### 📋 TODO (Before Launch)
- [ ] Record all video content per shot list in DEMO_CONTENT_GUIDE.md
- [ ] Take all required photos per photography guide
- [ ] Create and optimize animated GIFs
- [ ] Update README.md with hero image and embedded GIFs
- [ ] Upload videos to YouTube/Vimeo and embed in documentation
- [ ] Prepare social media posts using templates in MARKETING_COPY.md
- [ ] Assemble complete press kit ZIP with all media assets
- [ ] Update PRESS_KIT.md with actual download links

## Media Asset Organization

When creating actual media content, organize files as follows:

```
docs/
├── images/
│   ├── hero-ereader.jpg                 # Main README header image
│   ├── components-layout.jpg            # Pre-assembly component layout
│   ├── wiring-detail.jpg               # GPIO/SPI wiring close-up
│   ├── display-closeup.jpg             # Text rendering quality
│   ├── ereader-front.jpg               # Assembled device - front
│   ├── ereader-side.jpg                # Assembled device - side
│   ├── ereader-back.jpg                # Assembled device - back
│   ├── ereader-in-hand.jpg             # Scale and ergonomics
│   ├── comparison-size.jpg             # Size comparison
│   ├── comparison-display.jpg          # vs commercial e-reader
│   ├── ui-*.jpg                        # Screenshots of UI screens
│   ├── boot-*.jpg                      # Boot sequence screenshots
│   ├── boot-sequence.gif               # Animated boot (15s)
│   ├── page-turning.gif                # Animated page navigation
│   ├── menu-navigation.gif             # Animated library browsing
│   └── settings-demo.gif               # Animated settings adjustment
├── videos/
│   ├── ereader-demo-full-v1.mp4        # Complete 2-4 minute demo
│   ├── ereader-demo-boot-v1.mp4        # Boot sequence showcase
│   ├── ereader-demo-reading-v1.mp4     # Reading experience
│   └── ereader-demo-wifi-v1.mp4        # WiFi features demonstration
└── marketing/
    ├── README.md                        # This file
    ├── DEMO_CONTENT_GUIDE.md           # Video/photo creation guide
    ├── MARKETING_COPY.md               # Copy templates and messaging
    ├── PRESS_KIT.md                    # Press kit and media resources
    └── press-kit.zip                   # Downloadable press kit (TBD)
```

## Quick Reference

### For Social Media Posts
→ See **MARKETING_COPY.md** - Section "Social Media Content"
- Twitter/X announcement templates
- Reddit post title and body
- Hacker News "Show HN" format
- Hackaday.io project description

### For Demo Videos
→ See **DEMO_CONTENT_GUIDE.md** - Section "Video Demo Production"
- 9 scenes with specific shot descriptions
- Timing breakdown (0:00 to 3:00)
- Narration scripts for each scene
- Technical specifications (1080p, 30fps, MP4)

### For Photography
→ See **DEMO_CONTENT_GUIDE.md** - Section "Photography Guide"
- Required photos checklist (hero shot, components, wiring, etc.)
- Camera settings and lighting recommendations
- Composition guidelines

### For Press Outreach
→ See **PRESS_KIT.md** - Complete media resource
- One-paragraph project summary
- Technical specifications table
- Cost breakdown
- Key messages for different media types

### For Feature Descriptions
→ See **MARKETING_COPY.md** - Section "Feature Descriptions"
- Technical, user-friendly, and marketing versions
- Each major feature (boot time, e-paper, buttons, WiFi, battery, etc.)

## Creating Demo Content Workflow

When you have assembled hardware and are ready to create demo content:

### Step 1: Preparation (1-2 hours)
1. Read **DEMO_CONTENT_GUIDE.md** completely
2. Gather equipment (camera, tripod, lighting)
3. Set up photography/video area with proper background and lighting
4. Prepare device (fully charged, sample books loaded)
5. Test camera settings and framing

### Step 2: Photography (2-3 hours)
1. Follow photography checklist in DEMO_CONTENT_GUIDE.md
2. Take all required photos (10+ different shots)
3. Review photos for focus, lighting, readability
4. Retake any problematic shots
5. Import and backup RAW files

### Step 3: Videography (3-5 hours)
1. Follow video shot list in DEMO_CONTENT_GUIDE.md
2. Record each scene (multiple takes recommended)
3. Record any optional B-roll footage
4. Verify all clips are in focus and properly exposed
5. Import and backup video files

### Step 4: Post-Production (4-6 hours)
1. Edit photos (crop, color correct, optimize)
2. Export web-optimized versions (JPEG 90%, sRGB)
3. Edit video (rough cut → overlays → audio → export)
4. Create animated GIFs from video clips
5. Optimize all assets for file size

### Step 5: Integration (1-2 hours)
1. Commit all media files to repository
2. Update README.md with embedded images/GIFs
3. Upload videos to YouTube/Vimeo
4. Update PRESS_KIT.md with actual asset links
5. Create press-kit.zip download

### Step 6: Distribution (1-2 hours)
1. Prepare social media posts using MARKETING_COPY.md templates
2. Schedule posts across platforms
3. Prepare press outreach emails using PRESS_KIT.md
4. Submit to relevant communities (Reddit, Hacker News, etc.)

**Total Time Estimate:** 12-20 hours spread over 2-3 days

## Quality Standards

All demo content must meet these standards:

### Photos
- ✅ Minimum 3000×2000 resolution
- ✅ In focus with proper exposure
- ✅ Display text is clearly readable
- ✅ Clean, professional background
- ✅ Licensed under CC BY-SA 4.0
- ✅ Metadata includes copyright and attribution

### Videos
- ✅ 1920×1080 (1080p) minimum resolution
- ✅ Stable footage (tripod-mounted)
- ✅ Clear audio (if narration included)
- ✅ Display content is readable
- ✅ Proper lighting without glare
- ✅ Licensed under CC BY-SA 4.0

### GIFs
- ✅ File size under 5MB (preferably under 2MB)
- ✅ Smooth looping without jarring transitions
- ✅ 10-15fps (sufficient for e-paper refresh rate)
- ✅ 640×480 to 800×600 resolution
- ✅ Optimized with gifsicle or similar

## Launch Checklist

Before announcing the project publicly:

- [ ] All documentation complete and proofread
- [ ] All media assets created and committed
- [ ] README.md includes hero image and GIFs
- [ ] Demo video uploaded to stable hosting (YouTube)
- [ ] Press kit ZIP created and available for download
- [ ] Social media accounts created (if applicable)
- [ ] Community forum/discussions enabled
- [ ] Issue templates and PR templates configured
- [ ] LICENSE files in place
- [ ] CONTRIBUTING.md and CODE_OF_CONDUCT.md reviewed
- [ ] Pre-built SD card image available for download (optional)
- [ ] GitHub release created with changelog
- [ ] All links in documentation verified and working

## Resources

### Recommended Tools

**Free/Open Source:**
- **Photo Editing:** GIMP, Darktable, RawTherapee
- **Video Editing:** DaVinci Resolve, Kdenlive, OpenShot
- **Screen Recording:** OBS Studio, SimpleScreenRecorder
- **GIF Creation:** ffmpeg, gifsicle
- **Image Optimization:** ImageOptim, Squoosh.app

**Paid (Optional):**
- **Photo Editing:** Adobe Photoshop, Affinity Photo
- **Video Editing:** Adobe Premiere Pro, Final Cut Pro

### Learning Resources

- [DaVinci Resolve Tutorial](https://www.youtube.com/watch?v=UguJiz24xCk) - Free video editing
- [Product Photography Basics](https://www.youtube.com/watch?v=TZ5d4rHj4kw) - Lighting and composition
- [ffmpeg GIF Creation Guide](https://engineering.giphy.com/how-to-make-gifs-with-ffmpeg/) - High-quality GIFs

## Support

For questions about marketing materials or demo content creation:

1. Review the relevant guide in this directory
2. Check examples in the documentation
3. Open a discussion in GitHub Discussions
4. Tag issue with "marketing" or "documentation" label

## License

All marketing materials, copy templates, and guidelines in this directory are licensed under **Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)**.

You are free to:
- Use these templates for your own projects
- Modify and adapt the content
- Share with attribution

Under the condition that:
- You provide attribution to the Open Source E-Reader project
- You license derivative works under the same license

---

**Last Updated:** 2026-01-17
**Status:** Documentation complete, awaiting hardware for media creation
**Next Step:** Record demo videos and take product photos per guidelines
