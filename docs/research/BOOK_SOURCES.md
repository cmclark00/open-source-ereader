---
type: research
title: Public Domain Book Sources for E-Reader Download
created: 2026-01-17
tags:
  - books
  - api
  - opds
  - public-domain
  - network
related:
  - "[[Phase-06-WiFi-and-Network-Features]]"
  - "[[WIFI_UI]]"
---

# Public Domain Book Sources

This document catalogs available public domain book sources, APIs, and OPDS feeds suitable for implementing the online book download feature in Phase 06. All sources listed provide legally free access to public domain books.

## Executive Summary

**Recommended Approach for Phase 06:**
1. **Primary Source**: Project Gutenberg via direct URL download (simple, reliable)
2. **API Layer**: Gutendex for metadata and search (free, JSON REST API)
3. **Alternative**: Standard Ebooks OPDS feed (high-quality editions, OPDS native)
4. **Future**: Internet Archive Books API (massive catalog, more complex)

## 1. Project Gutenberg

### Overview
- **Catalog Size**: 70,000+ free e-books
- **Formats**: TXT, EPUB, HTML, PDF, Kindle
- **License**: Public domain in the USA
- **Update Frequency**: Regular additions
- **Website**: https://www.gutenberg.org

### Access Methods

#### Direct URL Download (Recommended for MVP)
Books can be downloaded directly using a simple URL pattern:
```
https://www.gutenberg.org/ebooks/{id}.txt.utf-8
https://www.gutenberg.org/ebooks/{id}.epub.noimages
https://www.gutenberg.org/ebooks/{id}.epub.images
```

**Example**: Alice's Adventures in Wonderland (ID: 11)
```
https://www.gutenberg.org/ebooks/11.txt.utf-8
https://www.gutenberg.org/ebooks/11.epub.noimages
```

**Advantages**:
- No API key required
- Simple HTTP GET request
- Extremely reliable
- Low bandwidth overhead
- Perfect for embedded devices

**Limitations**:
- No metadata API (need to know book ID)
- No search capability
- No catalog browsing

#### Gutendex API (Recommended for Metadata)
**URL**: https://gutendex.com
**Type**: Free JSON REST API
**GitHub**: https://github.com/garethbjohnson/gutendex

**Features**:
- Free, no API key required
- Search by title, author, topic, language
- Filter by copyright status, format availability
- Pagination support
- JSON responses with download links

**Example Requests**:
```bash
# Get all books
curl https://gutendex.com/books/

# Search by title
curl https://gutendex.com/books/?search=alice

# Search by author
curl https://gutendex.com/books/?search=dickens

# Filter by language
curl https://gutendex.com/books/?languages=en

# Get specific book
curl https://gutendex.com/books/11/
```

**Response Format**:
```json
{
  "count": 70000,
  "next": "https://gutendex.com/books/?page=2",
  "previous": null,
  "results": [
    {
      "id": 11,
      "title": "Alice's Adventures in Wonderland",
      "authors": [
        {"name": "Carroll, Lewis", "birth_year": 1832, "death_year": 1898}
      ],
      "languages": ["en"],
      "subjects": ["Fantasy", "Children's literature"],
      "bookshelves": ["Children's Literature"],
      "formats": {
        "text/plain; charset=utf-8": "https://www.gutenberg.org/files/11/11-0.txt",
        "application/epub+zip": "https://www.gutenberg.org/ebooks/11.epub.noimages"
      }
    }
  ]
}
```

**Implementation Strategy**:
1. Use Gutendex API for browsing and searching
2. Extract download URL from `formats` field
3. Download book using direct HTTP GET
4. Save to `/books/` directory

#### OPDS Feed
**URL**: https://www.gutenberg.org/ebooks/search.opds/
**Format**: Atom XML (OPDS 1.2)

**Features**:
- Browse by category, author, language
- Search functionality
- Standard OPDS catalog format
- Pagination support

**Use Case**: If implementing full OPDS client support (Phase 07)

#### Robot Access Policy
Project Gutenberg allows automated access but requests:
- Limit to 1 request per second
- Use descriptive User-Agent header
- Avoid mass downloads (our use case is fine)
- Prefer mirrors if downloading many books

**Recommended User-Agent**:
```
User-Agent: OpenSourceEReader/1.0 (Raspberry Pi; educational project)
```

### Offline Catalog (Alternative)
For completely offline browsing:
- **RDF Catalog**: ~100MB compressed, full metadata
- **CSV Catalog**: ~14MB uncompressed, limited metadata
- Download once, ship with SD card image

**Pros**: Works without internet
**Cons**: Not searchable on-device, large size, goes stale

---

## 2. Internet Archive Books

### Overview
- **Catalog Size**: 10,000,000+ books and texts
- **Formats**: PDF, EPUB, TXT, DAISY, MOBI (derivatives)
- **License**: Mix of public domain and borrowable copyrighted works
- **Website**: https://archive.org

### Access Methods

#### Internet Archive API
**Documentation**: https://archive.org/developers/
**API Endpoint**: https://archive.org/services/docs/api/index.html

**Features**:
- Download on-the-fly derivative formats (EPUB, MOBI, DAISY)
- Filter by format (TXT, EPUB)
- Full-text search across entire catalog
- Rich metadata (OCR quality, page count, etc.)

**Example Request**:
```bash
# Search for public domain books
curl "https://archive.org/advancedsearch.php?q=mediatype:texts+AND+collection:gutenberg&fl[]=identifier,title,creator&rows=50&output=json"

# Get book metadata
curl "https://archive.org/metadata/{identifier}"

# Download file
curl "https://archive.org/download/{identifier}/{filename}"
```

**Complexity**:
- More complex API than Gutendex
- Requires understanding of collections and identifiers
- Borrowable books need authentication (skip for Phase 06)

**Use Case**: Phase 07 expansion for massive catalog access

#### Open Library API
**URL**: https://openlibrary.org/developers/api
**Type**: JSON REST API

**Features**:
- 3,000,000+ books
- Books API for metadata lookup
- Search API for discovery
- Author and subject browsing

**Example**:
```bash
# Get book by ISBN
curl "https://openlibrary.org/api/books?bibkeys=ISBN:0451526538&format=json"

# Search
curl "https://openlibrary.org/search.json?q=alice+wonderland"
```

**Integration**: Can be used alongside Archive.org for metadata enrichment

---

## 3. Standard Ebooks

### Overview
- **Catalog Size**: 600+ books (curated, high quality)
- **Formats**: EPUB, EPUB3 (with advanced features)
- **License**: Public domain + CC0 for enhancements
- **Quality**: Professional typography and formatting
- **Website**: https://standardebooks.org

### Access Methods

#### OPDS Feed (Recommended)
**URL**: https://standardebooks.org/opds/all
**Format**: OPDS 1.2 (Atom XML)

**Features**:
- Complete catalog in OPDS format
- High-quality EPUB files
- Professional cover art
- Semantic markup and proper hyphenation
- Accessible (ARIA landmarks, screen reader friendly)

**OPDS Endpoints**:
```
https://standardebooks.org/opds/all        # All books
https://standardebooks.org/feeds           # Feed browser
```

**Advantages**:
- Best-in-class formatting and typography
- Smaller, curated collection (easier to browse)
- Native OPDS support
- Beautiful cover images

**Limitations**:
- Smaller catalog than Gutenberg
- Only EPUB format (no TXT)
- Slower update cadence

**Use Case**: Premium book source for users who want quality over quantity

#### Direct Download
Each book has a direct download URL:
```
https://standardebooks.org/ebooks/{author}/{title}/downloads/{author}_{title}.epub
```

**Example**:
```
https://standardebooks.org/ebooks/lewis-carroll/alices-adventures-in-wonderland/downloads/lewis-carroll_alices-adventures-in-wonderland.epub
```

---

## 4. LibriVox (Audiobooks Only)

### Overview
- **Catalog Size**: 15,000+ audiobook recordings
- **Format**: MP3, OGG Vorbis
- **Content**: Public domain audiobooks (read by volunteers)
- **Website**: https://librivox.org

### Access Methods

#### LibriVox API
**URL**: https://librivox.org/api/feed/audiobooks
**Formats**: XML (default), JSON, JSONP

**Features**:
- Complete audiobook catalog
- Search by title, author, genre
- Download links to audio files

**Note for This Project**:
- LibriVox provides **audio**, not text
- Text versions are from Project Gutenberg
- **Not suitable for Phase 06** (text e-reader focus)
- Could be used in future for audio book support

---

## 5. Feedbooks (DEPRECATED)

### Status
- **Service Status**: Shut down in July 2024
- **Replacement**: Cantook.com
- **OPDS Feed**: No longer available at feedbooks.com

**Historical Note**:
Feedbooks was a major OPDS catalog provider with 3M+ ebooks distributed monthly. The catalog was a reference implementation for OPDS. However, it's no longer operational.

**Action**: ~~Do not use~~ - Check if Cantook offers similar public domain OPDS access

---

## OPDS Overview

### What is OPDS?
**Open Publication Distribution System** (OPDS) is a catalog format spec for ebooks. It uses Atom Syndication Format (XML) to enable ebook discovery and distribution.

### OPDS Catalogs Available
1. **Project Gutenberg**: https://www.gutenberg.org/ebooks/search.opds/
2. **Standard Ebooks**: https://standardebooks.org/opds/all
3. **Internet Archive**: Various collection feeds
4. **Calibre Servers**: Self-hosted library sharing

### OPDS Features
- Browse by category, author, subject
- Search across catalog
- Pagination
- Acquisition links (download)
- Cover images

### Complexity vs Benefit
**Pros**:
- Standard format, widely supported
- Rich browsing experience
- Works with existing OPDS readers

**Cons**:
- XML parsing overhead
- More complex than simple JSON API
- Requires OPDS client implementation

**Recommendation**: Start with simple JSON API (Gutendex), add OPDS in Phase 07 if needed

---

## Implementation Recommendations

### Phase 06: Initial Implementation

**Goal**: Basic book download capability

**Recommended Stack**:
1. **Backend**: Gutendex API for catalog browsing
2. **Download**: Direct Project Gutenberg URLs
3. **Formats**: TXT (primary), EPUB (future)
4. **UI**: Simple pre-configured catalog (top 100 popular books)

**Rationale**:
- Gutendex is free, simple, no API key
- Direct downloads are reliable
- TXT files are small and already supported
- Pre-configured list avoids on-device search complexity

**Implementation**:
```c
// Pseudo-code
typedef struct {
    int gutenberg_id;
    char title[128];
    char author[64];
    int size_kb;  // Approximate
} book_catalog_entry_t;

// Hardcoded catalog of popular books
book_catalog_entry_t popular_books[] = {
    {11, "Alice's Adventures in Wonderland", "Lewis Carroll", 150},
    {1342, "Pride and Prejudice", "Jane Austen", 700},
    {84, "Frankenstein", "Mary Shelley", 440},
    // ... top 100 books
};

// Download function
int download_book(int gutenberg_id, const char* dest_path) {
    char url[256];
    snprintf(url, sizeof(url),
             "https://www.gutenberg.org/ebooks/%d.txt.utf-8",
             gutenberg_id);

    // Use wget or libcurl to download
    return wget_download(url, dest_path);
}
```

**UI Flow**:
1. Settings > Library > Browse Online
2. Display hardcoded list of 100 popular books
3. Select book, confirm download
4. Show progress bar (bytes downloaded)
5. Save to /books/, add to library
6. Open book immediately

**Advantages**:
- No on-device search needed (Phase 07)
- Fast, predictable downloads
- Small code footprint
- Works offline after initial catalog load

### Phase 07: Advanced Features

**Enhancements**:
1. **Search**: Add Gutendex search API support
2. **OPDS**: Implement OPDS client for Standard Ebooks
3. **Metadata**: Show cover images, descriptions
4. **Collections**: Browse by genre, author
5. **Update Catalog**: Refresh popular books list over WiFi

---

## Alternative Approach: Manual URL Entry

**Concept**: Let users paste URLs from any source

**UI**:
1. Settings > Library > Download from URL
2. Text input system (from WiFi password entry)
3. Enter full URL to TXT or EPUB file
4. Download and add to library

**Pros**:
- Works with any public domain source
- Users can use web browser to find books
- No catalog management needed

**Cons**:
- Requires typing long URLs on e-reader
- No discovery mechanism
- Error-prone

**Verdict**: Could be added as secondary option, but pre-configured catalog is better UX

---

## Bandwidth and Storage Considerations

### Typical Book Sizes
- **TXT**: 100-800 KB (plain text)
- **EPUB**: 200 KB - 2 MB (with formatting)
- **PDF**: 1-10 MB (images, layout)

### Download Times (WiFi 802.11n)
- **500 KB book**: ~1-2 seconds
- **2 MB book**: ~5-10 seconds
- **Network latency**: 100-500ms (varies)

### Storage (8GB SD Card)
- **OS + App**: ~200 MB
- **Available for Books**: ~7.8 GB
- **Capacity**: 7,800 TXT books or 3,900 EPUB books

**Recommendation**: Show available storage in Library UI

---

## Legal and Ethical Considerations

### Copyright Status
- **Project Gutenberg**: US public domain only
- **Internet Archive**: Mix of PD and copyrighted (borrowable)
- **Standard Ebooks**: US public domain + CC0 enhancements

### Geographic Restrictions
Some books may be PD in the USA but copyrighted elsewhere. Project Gutenberg blocks access from certain countries for specific books.

**Mitigation**:
- Focus on pre-1928 works (PD worldwide)
- Respect Project Gutenberg's robot access policy
- Include attribution in downloaded books

### User Agreement
Consider adding a disclaimer:
```
"Books downloaded from Project Gutenberg are in the public
domain in the USA. You are responsible for checking copyright
status in your country."
```

---

## Testing Checklist

### API Testing
- [ ] Gutendex API returns valid JSON
- [ ] Search by title works
- [ ] Search by author works
- [ ] Pagination works
- [ ] Download URLs are valid

### Download Testing
- [ ] Small book (< 100 KB) downloads
- [ ] Large book (> 1 MB) downloads
- [ ] Network timeout handling
- [ ] Disk full error handling
- [ ] Interrupted download recovery

### Integration Testing
- [ ] Downloaded book appears in library
- [ ] Book opens correctly
- [ ] Metadata displays properly
- [ ] Storage space updates

---

## References

### Project Gutenberg
- [Gutendex API](https://gutendex.com/)
- [Gutendex GitHub](https://github.com/garethbjohnson/gutendex)
- [Project Gutenberg Robot Access Policy](https://www.gutenberg.org/policy/robot_access.html)

### Internet Archive
- [API Documentation](https://archive.org/developers/)
- [Open Library API](https://openlibrary.org/developers/api)
- [Advanced Search API](https://archive.org/developers/index-apis.html)

### Standard Ebooks
- [OPDS Feeds](https://standardebooks.org/feeds)
- [About Standard Ebooks](https://standardebooks.org)

### OPDS Resources
- [Awesome OPDS GitHub](https://github.com/opds-community/awesome-opds)
- [OPDS Specification](https://opds-spec.org/about/)

### LibriVox
- [API Documentation](https://librivox.org/api/info)

---

## Recommended Implementation Decision

### Phase 06 MVP: Pre-Configured Catalog

**Selected Approach**: **Hardcoded catalog of 100 popular books from Project Gutenberg**

**Rationale**:
1. **Simplicity**: No complex search UI required (Phase 07 feature)
2. **Performance**: Fast browsing, predictable downloads
3. **User Experience**: Most users want classic, popular books anyway
4. **Memory Efficient**: ~30 KB for 100 book entries vs potential API complexity
5. **Reliable**: No dependency on external API availability
6. **Testable**: Easily verify all downloads work

**Implementation Details**:

```c
// Hardcoded catalog (100 entries)
typedef struct {
    int gutenberg_id;           // Project Gutenberg book ID
    char title[64];             // Display title
    char author[48];            // Author name
    int size_kb;                // Approximate file size
    char genre[24];             // Genre for filtering (future)
} catalog_entry_t;

catalog_entry_t popular_books[100] = {
    {11, "Alice's Adventures in Wonderland", "Lewis Carroll", 150, "Fantasy"},
    {1342, "Pride and Prejudice", "Jane Austen", 700, "Romance"},
    {84, "Frankenstein", "Mary Shelley", 440, "Horror"},
    {1661, "Adventures of Sherlock Holmes", "Arthur Conan Doyle", 580, "Mystery"},
    {2701, "Moby Dick", "Herman Melville", 1200, "Adventure"},
    // ... 95 more entries
};
```

**Download Flow**:
1. User opens Settings > Online Library
2. Display scrollable list of 100 books (14 per page)
3. User selects book, confirms download
4. Progress bar shows download (uses wget or libcurl)
5. Book saved to /books/, added to library
6. User can immediately open the book

**Dependencies**:
- wget (simple, reliable) OR libcurl (more flexible)
- Recommendation: wget for Phase 06, migrate to libcurl in Phase 07

**Memory Footprint**:
- Catalog: 100 × 300 bytes = ~30 KB
- Download buffer: 64 KB (streaming)
- UI state: 5 KB
- Total: ~40 KB additional memory

**Curation Strategy**:
Select 100 books across categories:
- Classics: 30 books (Austen, Dickens, Shakespeare, etc.)
- Science Fiction: 10 books (Wells, Verne)
- Mystery: 10 books (Doyle, Christie)
- Adventure: 10 books (Twain, Stevenson)
- Philosophy: 10 books (Plato, Aristotle)
- Poetry: 10 books (Whitman, Dickinson)
- Children's: 10 books (Carroll, Baum)
- Horror: 10 books (Poe, Shelley)

Prioritize:
- Public domain worldwide (pre-1928 works)
- English language
- Text format availability
- Reasonable file size (< 1 MB preferred)
- Cultural significance and popularity

**Phase 07 Enhancements**:
- Add Gutendex API search for full catalog access
- Genre filtering on hardcoded catalog
- "Top 100" refresh via WiFi (update catalog quarterly)
- Standard Ebooks OPDS integration
- User can add custom URLs for other sources

---

## Next Steps

1. ✅ Research completed
2. ✅ Implementation approach decided: Pre-configured catalog (100 popular books)
3. [ ] Curate list of 100 popular Project Gutenberg books with IDs
4. [ ] Add required packages to Buildroot (wget preferred)
5. [ ] Implement download_manager.c with wget backend
6. [ ] Create library_browser.c UI with scrollable catalog
7. [ ] Test downloads with real WiFi connection
8. [ ] Document in USER_GUIDE.md

---

**Document Status**: Complete with Implementation Decision
**Last Updated**: 2026-01-17
**Decision**: Hardcoded 100-book catalog for Phase 06 MVP
**Next Review**: After Phase 06 implementation
