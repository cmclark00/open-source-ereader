---
type: research
title: E-Book Libraries for Embedded Linux
created: 2026-01-14
tags:
  - libraries
  - epub
  - pdf
  - embedded
  - raspberry-pi
related:
  - "[[PHASE_04_PLANNING]]"
  - "[[EREADER_DESIGN]]"
---

# E-Book Libraries for Embedded Linux (Raspberry Pi Zero W)

This document evaluates EPUB and PDF libraries suitable for the Raspberry Pi Zero W e-reader project, considering the platform's constraints: ARM architecture, 512MB RAM, 700MHz CPU, and no GPU acceleration.

## Executive Summary

**Recommended Choices:**

1. **EPUB:** Custom implementation using **libzip** + XML parsing (libxml2)
   - Rationale: Minimal dependencies, full control, lightweight
   - Alternative: EPUB3Processor (if full validation needed)

2. **PDF:** **MuPDF** (with text extraction mode)
   - Rationale: Smallest footprint (~2MB), fastest, optimized for embedded systems
   - Alternative: Poppler (if broader format support needed)

## Platform Constraints

### Raspberry Pi Zero W Specifications
- **CPU:** ARM1176JZF-S @ 700MHz (single core)
- **RAM:** 512MB
- **Storage:** MicroSD (limited I/O speed)
- **Display:** 400×300 e-paper (no GPU acceleration)
- **OS:** Custom Buildroot Linux (minimal userspace)

### Design Goals
- Boot time: <30 seconds
- Memory usage: <100MB for entire application
- Battery life: Multi-day reading sessions
- Library size: Minimal (prefer static linking where possible)

## EPUB Libraries

EPUB files are ZIP archives containing XHTML/HTML content, CSS, images, and metadata files (`META-INF/container.xml`, `content.opf`, `toc.ncx`).

### Option 1: libzip + Custom Parser (RECOMMENDED)

**Overview:**
Build a custom EPUB parser using libzip for archive handling and libxml2 for XML parsing.

**Pros:**
- **Minimal footprint:** libzip (~100KB), libxml2 (~1.5MB)
- **Full control:** Parse only what we need (no unused features)
- **Already in Buildroot:** Both libraries available as packages
- **Proven:** Used by Kobo eReader (commercial product)
- **License:** BSD (libzip), MIT (libxml2) - no restrictions

**Cons:**
- **Development time:** Must implement EPUB structure parsing
- **Limited validation:** No built-in EPUB spec compliance checking
- **Maintenance:** We own the parsing code

**Memory Footprint:**
- Library size: ~1.6MB total (static linking)
- Runtime memory: ~5-10MB per book (depends on book size)

**Implementation Notes:**
1. Use libzip to extract `META-INF/container.xml` → find `content.opf` path
2. Parse `content.opf` (XML) to get spine (reading order) and manifest (file list)
3. Extract XHTML chapters in order
4. Strip HTML tags, extract plain text with basic entity handling
5. Feed to existing text_renderer for pagination

**Buildroot Integration:**
```
BR2_PACKAGE_LIBZIP=y
BR2_PACKAGE_LIBXML2=y
BR2_PACKAGE_LIBXML2_READER=y
```

### Option 2: EPUB3Processor

**Overview:**
A fast, portable EPUB 2.x and 3 validating parser written in pure C (ANSI 99 standard).

**Pros:**
- **Pure C:** ANSI 99 compliant, highly portable
- **Validation:** Full EPUB 2.x and 3 spec compliance checking
- **Tested on ARM:** iOS (ARM) testing done
- **Fast:** Designed for performance
- **License:** Not specified in repository (requires investigation)

**Cons:**
- **Not in Buildroot:** Would need custom package
- **Dependency:** Requires libxml2
- **Overkill:** Full validation may be unnecessary for reading
- **Maintenance:** External dependency (last updated status unknown)
- **Unknown memory footprint:** No embedded benchmarks available

**Source:**
- GitHub: [MedallionMediaGroup/EPUB3Processor](https://github.com/MedallionMediaGroup/EPUB3Processor)

### Option 3: Readium SDK

**Overview:**
A C++ ePub renderer SDK used by many commercial readers.

**Pros:**
- **Industry standard:** Used by major publishers
- **Full featured:** Complete EPUB 3 support
- **Well maintained:** Active development

**Cons:**
- **C++:** Heavier than C, larger runtime
- **Too heavy:** Designed for tablets/phones, not embedded
- **Complex:** Overkill for simple text extraction
- **Large footprint:** >10MB library size
- **Not suitable:** Exceeds our memory/complexity budget

**Verdict:** Not recommended for Pi Zero W

### Option 4: epub2txt / epy (CLI tools)

**Overview:**
Command-line tools for EPUB text extraction.

**Pros:**
- **Simple:** Easy to integrate via system calls
- **Lightweight:** Minimal dependencies

**Cons:**
- **External process:** Fork/exec overhead
- **Limited control:** Can't integrate tightly with UI
- **Not libraries:** Would need to wrap in our code
- **epy is Python:** Heavy runtime (Python interpreter)

**Verdict:** Not recommended (prefer library integration)

## PDF Libraries

PDF is a complex format with multiple rendering modes: raster (bitmap), vector, and text extraction.

### Option 1: MuPDF (RECOMMENDED)

**Overview:**
Lightweight, fast PDF renderer designed for embedded systems and e-readers.

**Pros:**
- **Smallest footprint:** ~2MB library size
- **Fastest:** 78% faster than Poppler on average
- **Embedded focus:** Designed for resource-constrained devices
- **Text extraction:** Can extract text without rendering bitmaps
- **Low memory:** ~50MB runtime for typical PDFs
- **License:** AGPL (or commercial license available)
- **Buildroot available:** Package exists in Buildroot
- **Proven:** Used in many commercial e-readers

**Cons:**
- **AGPL license:** Copyleft (requires disclosure if distributed)
- **Less format support:** Optimized for common PDFs (not edge cases)
- **Commercial license cost:** If AGPL not acceptable

**Memory Footprint:**
- Library size: ~2MB (static)
- Runtime memory: ~50MB typical, 100MB max
- Text extraction mode: <30MB

**Performance:**
- Rendering: ~8 seconds per 100 pages (benchmarked)
- Text extraction: Faster (no raster operations)

**Buildroot Integration:**
```
BR2_PACKAGE_MUPDF=y
```

**Implementation Strategy:**
Phase 4 should focus on **text extraction only** (no bitmap rendering):
- Extract text from each PDF page
- Feed to existing text_renderer (like TXT files)
- Later phases can add bitmap rendering if needed

### Option 2: Poppler

**Overview:**
PDF rendering library based on xpdf, widely used in desktop Linux.

**Pros:**
- **Mature:** Long history, handles quirky PDFs well
- **More output formats:** Supports Cairo backend
- **Better compatibility:** Deals with edge cases
- **GPL license:** More permissive than AGPL
- **Buildroot available:** Package exists
- **Utilities included:** pdftotext, pdfinfo, etc.

**Cons:**
- **Larger footprint:** ~5-7MB library size
- **Slower:** 14 seconds per 100 pages (78% slower than MuPDF)
- **More dependencies:** Requires Cairo, FreeType, FontConfig
- **Higher memory:** ~50-80MB runtime
- **Desktop-focused:** Not optimized for embedded

**Memory Footprint:**
- Library size: ~5-7MB (static) + dependencies
- Runtime memory: ~50-80MB typical

**Buildroot Integration:**
```
BR2_PACKAGE_POPPLER=y
BR2_PACKAGE_POPPLER_UTILS=y  # Includes pdftotext
```

**Verdict:** Good backup option if MuPDF proves insufficient

### Option 3: Xpdf

**Overview:**
Original PDF viewer (Poppler is a fork).

**Pros:**
- **Lightweight:** Smaller than Poppler
- **Stable:** Mature codebase
- **GPL license:** Permissive

**Cons:**
- **Aging:** Less active development
- **Poppler preferred:** Most projects use Poppler instead
- **Similar tradeoffs:** Not significantly better than Poppler

**Verdict:** Not recommended (Poppler is the better evolution)

### Option 4: pdf2txt (Lightweight alternatives)

**Overview:**
Simple text extraction tools.

**Pros:**
- **Minimal:** Just text extraction, no rendering
- **Fast:** No complex operations

**Cons:**
- **Limited:** May fail on complex PDFs
- **External tools:** Would need system calls

**Verdict:** Could be an option for Phase 4 text-only support

## Comparison Matrix

### EPUB Libraries

| Library | Size | Memory | Speed | License | Buildroot | Complexity | Score |
|---------|------|--------|-------|---------|-----------|------------|-------|
| **libzip + custom** | 1.6MB | 5-10MB | Fast | BSD/MIT | ✅ Yes | Medium | ⭐⭐⭐⭐⭐ |
| EPUB3Processor | ~2MB | Unknown | Fast | Unknown | ❌ No | Low | ⭐⭐⭐ |
| Readium SDK | >10MB | >50MB | Medium | BSD | ❌ No | High | ⭐ |
| epub2txt | Small | Low | Fast | Various | Varies | Low | ⭐⭐ |

### PDF Libraries

| Library | Size | Memory | Speed | License | Buildroot | Quality | Score |
|---------|------|--------|-------|---------|-----------|---------|-------|
| **MuPDF** | 2MB | 30-50MB | ⚡ Fast | AGPL | ✅ Yes | Good | ⭐⭐⭐⭐⭐ |
| Poppler | 5-7MB | 50-80MB | Medium | GPL | ✅ Yes | Excellent | ⭐⭐⭐⭐ |
| Xpdf | 3-4MB | 40-60MB | Medium | GPL | ❌ No | Good | ⭐⭐ |
| pdf2txt | <1MB | 10-20MB | ⚡ Fast | Various | Varies | Basic | ⭐⭐⭐ |

## Recommended Implementation Plan

### Phase 4 Approach

1. **EPUB Support:**
   - Use libzip + libxml2 (both in Buildroot)
   - Implement custom EPUB parser in C
   - Focus on EPUB 2 and basic EPUB 3 (most common)
   - Extract text only (no CSS, no images in Phase 4)
   - Estimated code: ~500-800 lines

2. **PDF Support:**
   - Use MuPDF with text extraction mode
   - Do NOT render bitmaps (memory constraint)
   - Extract text page-by-page
   - Feed to existing text_renderer
   - Estimated integration: ~300-500 lines

### Memory Budget

| Component | Memory |
|-----------|--------|
| Linux kernel | ~50MB |
| Minimal rootfs | ~20MB |
| E-reader app | ~15MB |
| EPUB library (libzip/xml) | ~10MB |
| PDF library (MuPDF) | ~30MB |
| Book content buffer | ~20MB |
| **Total** | **~145MB** |
| **Available** | **512MB** |
| **Margin** | **367MB** ✅ |

### Known Limitations (Phase 4)

**EPUB:**
- No images (text only)
- Basic HTML only (no complex CSS)
- No JavaScript/interactive content
- No DRM support

**PDF:**
- Text extraction only (no bitmap rendering)
- May fail on image-heavy PDFs
- No form support
- No annotations

**Future Enhancements (Phase 5+):**
- Add bitmap rendering for PDFs (if memory allows)
- Add image support for EPUB
- Improve HTML/CSS rendering

## Performance Benchmarks

### Expected Performance (Pi Zero W)

**EPUB Loading:**
- Small book (<1MB): <2 seconds
- Medium book (1-5MB): 2-5 seconds
- Large book (5-10MB): 5-10 seconds

**PDF Loading:**
- Text extraction: ~1-2 seconds per page
- 100-page PDF: ~2-3 minutes initial load (cache per page)

**Memory Usage:**
- EPUB reader: 20-30MB typical
- PDF reader: 30-50MB typical
- Well within 512MB budget

## License Considerations

### libzip
- **License:** BSD 3-Clause
- **Commercial use:** ✅ Yes
- **Source disclosure:** ❌ Not required
- **Verdict:** ✅ Acceptable

### libxml2
- **License:** MIT
- **Commercial use:** ✅ Yes
- **Source disclosure:** ❌ Not required
- **Verdict:** ✅ Acceptable

### MuPDF
- **License:** AGPL v3 (or commercial)
- **Commercial use:** ⚠️ Requires commercial license OR source disclosure
- **Source disclosure:** ✅ Required (if distributed)
- **Verdict:** ✅ Acceptable (project is open-source)

### Poppler
- **License:** GPL v2/v3
- **Commercial use:** ✅ Yes (if source disclosed)
- **Source disclosure:** ✅ Required (if distributed)
- **Verdict:** ✅ Acceptable (project is open-source)

**Note:** Since this is an open-source project, AGPL/GPL licenses are acceptable. If commercialization is planned, consider commercial MuPDF license or switch to Poppler (GPL).

## References

### EPUB Resources
- libzip: [libzip.org](https://libzip.org/)
- libzip GitHub: [github.com/nih-at/libzip](https://github.com/nih-at/libzip)
- EPUB3Processor: [github.com/MedallionMediaGroup/EPUB3Processor](https://github.com/MedallionMediaGroup/EPUB3Processor)
- Readium SDK: [github.com/readium/readium-sdk](https://github.com/readium/readium-sdk)
- EPUB programming: [MobileRead Wiki - EPub programming](https://wiki.mobileread.com/wiki/EPub_programming)

### PDF Resources
- MuPDF comparison: [Comparison of Mupdf and Other PDF Rendering Libraries](https://github.com/xiangxw/mupdf-qt/wiki/Comparison-of-Mupdf-and-Other-PDF-Rendering-Libraries)
- MuPDF vs Poppler: [Johnny Huang - Poppler vs MuPDF](https://hzqtc.github.io/2012/04/poppler-vs-mupdf.html)
- PDF Survey: [Survey of open-source PDF solutions](https://pdfa.org/wp-content/uploads/2021/06/Survey-of-OpenSource-Solutions.pdf)
- Buildroot poppler: [github.com/maximeh/buildroot/blob/master/package/poppler/poppler.mk](https://github.com/maximeh/buildroot/blob/master/package/poppler/poppler.mk)
- MuPDF ArchWiki: [wiki.archlinux.org/title/MuPDF](https://wiki.archlinux.org/title/MuPDF)

### Raspberry Pi E-Reader Projects
- Tom's Hardware: [Raspberry Pi Pico Powered Open Source E-Book Reader](https://www.tomshardware.com/news/raspberry-pi-pico-powered-open-source-ebook-reader)
- Preader project: [github.com/zephray/Preader](https://github.com/zephray/Preader)
- E-reader comparison: [37 Open-source and Free EPUB e-book Viewers and Readers](https://medevel.com/37-epub-readers/)

### Benchmark Resources
- PDF raster test: [github.com/nathanstitt/dc-pdf-raster-test](https://github.com/nathanstitt/dc-pdf-raster-test)
- Buildroot manual: [buildroot.org/downloads/manual/manual.pdf](https://buildroot.org/downloads/manual/manual.pdf)

## Next Steps

1. ✅ Research complete (this document)
2. ⬜ Add libzip and libxml2 to Buildroot config
3. ⬜ Add MuPDF to Buildroot config
4. ⬜ Implement EPUB parser (`src/ereader/formats/epub_reader.c`)
5. ⬜ Implement PDF text extractor (`src/ereader/formats/pdf_reader.c`)
6. ⬜ Create format abstraction layer (`src/ereader/formats/format_interface.h`)
7. ⬜ Update menu system for multi-format support
8. ⬜ Test with sample e-books
9. ⬜ Document limitations and usage

## Conclusion

For the Raspberry Pi Zero W e-reader project:

- **EPUB:** Use libzip + libxml2 with custom parser (lightweight, full control)
- **PDF:** Use MuPDF in text extraction mode (smallest footprint, fastest)

This approach balances memory constraints, performance, and implementation complexity while staying within the 512MB RAM budget and maintaining fast boot times.

Total estimated memory usage: ~145MB (28% of available RAM), leaving 367MB for system overhead and future features.

**Implementation complexity:** Medium (custom EPUB parser requires ~500-800 lines of C code, but we get full control and minimal footprint).

**Alternative if needed:** If MuPDF proves insufficient, Poppler is a solid fallback with broader format support at the cost of ~3MB additional footprint and ~20% slower performance.
