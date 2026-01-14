# Phase 04: EPUB and PDF Support

This phase expands the e-reader to support industry-standard e-book formats: EPUB and PDF. By the end of this phase, users can read most common e-book files, not just plain text, with proper formatting and navigation.

## Tasks

- [x] Research EPUB and PDF libraries for embedded Linux:
  - Create `docs/research/EBOOK_LIBRARIES.md` with front matter (type: research, tags: [libraries, epub, pdf]):
    - Evaluate libraries: libepub, epub2txt, poppler (PDF), mupdf
    - Compare: memory footprint, dependencies, rendering quality, license
    - Consider Pi Zero W constraints (ARM, 512MB RAM, no GPU acceleration)
    - Document recommended choices with rationale
  - Link to performance benchmarks if available
  - **Completed:** Created comprehensive research document evaluating all major EPUB and PDF libraries. Recommended libzip+libxml2 for EPUB (custom parser, 1.6MB footprint) and MuPDF for PDF (2MB footprint, text extraction mode). Full comparison matrix, memory budgets, and implementation strategy documented.

- [x] Add EPUB library to Buildroot:
  - Research if chosen EPUB library already has Buildroot package (check package/*)
  - If not, create `package/libepub/libepub.mk` (or similar) package:
    - Download source, configure, compile, install
    - Handle cross-compilation for ARM
  - If yes, simply enable existing package in defconfig
  - Add to `configs/ereader_rpi0w_defconfig`
  - Document in `docs/buildroot/CUSTOM_PACKAGES.md`
  - **Completed:** Added `BR2_PACKAGE_LIBZIP=y` and `BR2_PACKAGE_LIBXML2=y` to defconfig. Both packages already exist in Buildroot. libzip handles ZIP archives (EPUB is a ZIP container), libxml2 handles XML parsing for EPUB metadata. Total footprint: ~1.6MB. Documented in CUSTOM_PACKAGES.md.

- [x] Add PDF library to Buildroot:
  - Evaluate mupdf vs poppler trade-offs (size vs features)
  - Add chosen library to Buildroot (create package or enable existing)
  - Consider lightweight alternatives if full PDF is too heavy (pdf2txt → render as text)
  - Add to `configs/ereader_rpi0w_defconfig`
  - Document decision in `docs/research/EBOOK_LIBRARIES.md`
  - **Completed:** Added `BR2_PACKAGE_POPPLER=y` and `BR2_PACKAGE_POPPLER_UTILS=y` to defconfig. Chose Poppler over MuPDF because MuPDF's Buildroot package requires X11 (unsuitable for embedded). Poppler only requires fontconfig and provides pdftotext utility for text extraction. Footprint: ~5-7MB. Decision documented in EBOOK_LIBRARIES.md and CUSTOM_PACKAGES.md.

- [x] Implement EPUB parser and renderer:
  - Create `src/ereader/formats/epub_reader.c` with functions:
    - Open and validate EPUB file (ZIP container)
    - Parse META-INF/container.xml and content.opf
    - Extract HTML/XHTML chapters in reading order
    - Strip HTML tags and extract plain text (Phase 4 does simple rendering)
    - Handle basic HTML entities (& &lt; &gt; etc)
  - Write `src/ereader/formats/epub_reader.h`
  - Integrate with existing text_renderer from Phase 3
  - Handle errors gracefully (corrupt EPUB, missing files)
  - **Completed:** Created complete EPUB parser using libzip and libxml2. Implemented ZIP validation, container.xml parsing, OPF metadata/spine parsing, HTML tag stripping, and entity decoding. Supports EPUB 2 and basic EPUB 3 formats. Extracts plain text from all chapters with proper error handling for corrupt files. Updated Makefile with libzip and libxml2 linking. Total implementation: ~850 lines of C code in formats/epub_reader.c and formats/epub_reader.h

- [ ] Implement PDF renderer:
  - Create `src/ereader/formats/pdf_reader.c` with functions:
    - Open PDF file and get page count
    - Render PDF page to text (extract text content)
    - Or: render PDF page to bitmap if library supports (check memory limits)
    - Handle text extraction failures gracefully
  - Write `src/ereader/formats/pdf_reader.h`
  - If rendering to bitmap: integrate with framebuffer code
  - If extracting text: integrate with text_renderer
  - Note: Phase 4 focuses on basic PDF support, not perfect fidelity

- [ ] Create format detection and abstraction layer:
  - Update `src/ereader/books/book_manager.c` to:
    - Detect file format by extension (.txt, .epub, .pdf)
    - Load appropriate reader (text, EPUB, PDF)
    - Present unified interface to UI layer (get page count, render page N)
  - Create `src/ereader/formats/format_interface.h` with:
    - Generic book format interface (function pointers)
    - Common functions: open, close, get_page_count, render_page, get_metadata
  - Implement interface for each format (txt_reader, epub_reader, pdf_reader)

- [ ] Update menu system to handle all formats:
  - Extend `src/ereader/ui/menu.c` to:
    - Display file type icon or indicator ([T] [E] [P] for TXT/EPUB/PDF)
    - Show book metadata if available (title from EPUB, PDF info)
    - Handle different file sizes (EPUB can be large)
  - Update book scanning to include .epub and .pdf extensions
  - Sort books alphabetically regardless of format

- [ ] Update reader view for multi-format support:
  - Extend `src/ereader/ui/reader.c` to:
    - Use format abstraction layer (no format-specific code in UI)
    - Display format-specific metadata (chapter names for EPUB)
    - Handle format-specific page navigation (PDF page numbers vs EPUB flow)
  - Update status bar to show format type
  - Handle loading delays for complex PDFs (show "Loading..." message)

- [ ] Build and test with sample e-books:
  - Add to `board/ereader/rootfs-overlay/books/`:
    - Sample EPUB from Project Gutenberg or similar
    - Sample PDF (simple, public domain document)
  - Rebuild Buildroot with new format support
  - Create `docs/testing/FORMAT_TESTING.md` with:
    - How to test each format
    - Known limitations (complex PDFs, image-heavy EPUBs)
    - Performance notes (EPUB vs PDF rendering time)

- [ ] Document format support and limitations:
  - Update `docs/USER_GUIDE.md` with:
    - Supported formats: TXT, EPUB, PDF
    - How to add books in each format
    - Format-specific limitations (no images yet, basic HTML only, etc.)
  - Create `docs/architecture/FORMAT_SUPPORT.md` documenting:
    - How format detection works
    - How to add new formats in the future
    - Format abstraction layer design
  - Update `docs/progress/PHASE_04_LOG.md` with completion notes

- [ ] Plan Phase 5 features:
  - Create `docs/progress/PHASE_05_PLANNING.md` outlining:
    - Advanced features: bookmarks, annotations, search
    - Settings menu: font size, margins, display mode
    - Sleep mode and power management
