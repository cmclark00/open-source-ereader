/*
 * epub_reader.c - EPUB Format Reader Implementation
 *
 * EPUB parser and text extractor using libzip and libxml2.
 * Extracts plain text from EPUB files for display on e-reader.
 *
 * Phase 04: EPUB and PDF Support
 */

#include "epub_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <zip.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

/*
 * Internal Helper Structures
 */

/* HTML entity mapping for common entities */
typedef struct {
    const char *entity;
    const char *replacement;
} html_entity_map_t;

/* Common HTML entities we need to handle */
static const html_entity_map_t html_entities[] = {
    {"&amp;", "&"},
    {"&lt;", "<"},
    {"&gt;", ">"},
    {"&quot;", "\""},
    {"&#39;", "'"},
    {"&apos;", "'"},
    {"&nbsp;", " "},
    {"&mdash;", "—"},
    {"&ndash;", "–"},
    {"&ldquo;", """},
    {"&rdquo;", """},
    {"&lsquo;", "'"},
    {"&rsquo;", "'"},
    {"&hellip;", "…"},
    {NULL, NULL}
};

/*
 * Internal Helper Functions
 */

/* Read a file from ZIP archive into memory */
static char* zip_read_file(zip_t *zip, const char *filename, size_t *out_size) {
    zip_stat_t stat;
    zip_file_t *file;
    char *buffer;
    zip_int64_t bytes_read;

    /* Get file statistics */
    if (zip_stat(zip, filename, 0, &stat) != 0) {
        fprintf(stderr, "epub: File not found in ZIP: %s\n", filename);
        return NULL;
    }

    /* Open file in ZIP */
    file = zip_fopen(zip, filename, 0);
    if (!file) {
        fprintf(stderr, "epub: Failed to open file in ZIP: %s\n", filename);
        return NULL;
    }

    /* Allocate buffer */
    buffer = malloc(stat.size + 1);
    if (!buffer) {
        fprintf(stderr, "epub: Out of memory allocating %lld bytes\n", stat.size);
        zip_fclose(file);
        return NULL;
    }

    /* Read file content */
    bytes_read = zip_fread(file, buffer, stat.size);
    zip_fclose(file);

    if (bytes_read != (zip_int64_t)stat.size) {
        fprintf(stderr, "epub: Read error: expected %lld bytes, got %lld\n",
                stat.size, bytes_read);
        free(buffer);
        return NULL;
    }

    /* Null-terminate */
    buffer[stat.size] = '\0';
    if (out_size) {
        *out_size = stat.size;
    }

    return buffer;
}

/* Strip HTML tags and extract text content */
static int strip_html_tags(const char *html, size_t html_length,
                          char **out_text, size_t *out_length) {
    char *text;
    size_t text_pos = 0;
    size_t i = 0;
    int in_tag = 0;
    int in_script = 0;
    int in_style = 0;

    /* Allocate output buffer (same size as input is maximum needed) */
    text = malloc(html_length + 1);
    if (!text) {
        return EPUB_ERROR_OUT_OF_MEMORY;
    }

    /* Parse HTML and extract text */
    while (i < html_length) {
        /* Check for <script> and <style> tags */
        if (!in_tag && i + 8 < html_length) {
            if (strncasecmp(&html[i], "<script", 7) == 0 &&
                (html[i+7] == '>' || html[i+7] == ' ')) {
                in_script = 1;
                in_tag = 1;
                i++;
                continue;
            }
            if (strncasecmp(&html[i], "<style", 6) == 0 &&
                (html[i+6] == '>' || html[i+6] == ' ')) {
                in_style = 1;
                in_tag = 1;
                i++;
                continue;
            }
        }

        /* Check for closing script/style tags */
        if ((in_script || in_style) && i + 9 < html_length) {
            if (in_script && strncasecmp(&html[i], "</script>", 9) == 0) {
                in_script = 0;
                i += 9;
                continue;
            }
            if (in_style && strncasecmp(&html[i], "</style>", 8) == 0) {
                in_style = 0;
                i += 8;
                continue;
            }
        }

        /* Skip content inside script/style tags */
        if (in_script || in_style) {
            i++;
            continue;
        }

        /* Handle tag start */
        if (html[i] == '<') {
            in_tag = 1;

            /* Add space before block-level tags for word separation */
            if (i + 2 < html_length) {
                if (html[i+1] == 'p' || html[i+1] == 'P' ||
                    html[i+1] == 'h' || html[i+1] == 'H' ||
                    (html[i+1] == 'b' && html[i+2] == 'r') ||
                    (html[i+1] == 'd' && html[i+2] == 'i' && html[i+3] == 'v')) {
                    if (text_pos > 0 && text[text_pos - 1] != '\n' && text[text_pos - 1] != ' ') {
                        text[text_pos++] = '\n';
                    }
                }
            }
            i++;
            continue;
        }

        /* Handle tag end */
        if (html[i] == '>') {
            in_tag = 0;
            i++;
            continue;
        }

        /* Skip tag content */
        if (in_tag) {
            i++;
            continue;
        }

        /* Copy text content */
        text[text_pos++] = html[i];
        i++;
    }

    /* Null-terminate */
    text[text_pos] = '\0';

    *out_text = text;
    *out_length = text_pos;
    return EPUB_SUCCESS;
}

/*
 * Utility Functions Implementation
 */

const char* epub_error_string(epub_error_t error) {
    switch (error) {
        case EPUB_SUCCESS:
            return "Success";
        case EPUB_ERROR_NOT_FOUND:
            return "EPUB file not found";
        case EPUB_ERROR_INVALID_FORMAT:
            return "Invalid EPUB format";
        case EPUB_ERROR_CORRUPT_ZIP:
            return "Corrupt ZIP archive";
        case EPUB_ERROR_MISSING_CONTAINER:
            return "Missing META-INF/container.xml";
        case EPUB_ERROR_MISSING_OPF:
            return "Missing content.opf file";
        case EPUB_ERROR_PARSE_ERROR:
            return "XML parse error";
        case EPUB_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case EPUB_ERROR_TOO_LARGE:
            return "EPUB file too large";
        case EPUB_ERROR_READ_FAILED:
            return "Failed to read EPUB file";
        case EPUB_ERROR_NO_CONTENT:
            return "No readable content found";
        default:
            return "Unknown error";
    }
}

int epub_is_epub_file(const char *filename) {
    if (!filename) {
        return 0;
    }

    size_t len = strlen(filename);
    if (len < 6) {  /* Minimum: "a.epub" */
        return 0;
    }

    /* Check for .epub extension (case-insensitive) */
    const char *ext = filename + len - 5;
    return (strcasecmp(ext, ".epub") == 0);
}

/*
 * Core EPUB Functions Implementation
 */

int epub_validate(const char *filepath) {
    zip_t *zip;
    zip_error_t error;
    int result;

    if (!filepath) {
        return EPUB_ERROR_NOT_FOUND;
    }

    /* Try to open as ZIP */
    zip = zip_open(filepath, ZIP_RDONLY, &result);
    if (!zip) {
        zip_error_init_with_code(&error, result);
        fprintf(stderr, "epub_validate: Failed to open %s: %s\n",
                filepath, zip_error_strerror(&error));
        zip_error_fini(&error);
        return EPUB_ERROR_CORRUPT_ZIP;
    }

    /* Check for mimetype file */
    zip_stat_t stat;
    if (zip_stat(zip, "mimetype", 0, &stat) != 0) {
        fprintf(stderr, "epub_validate: Missing mimetype file\n");
        zip_close(zip);
        return EPUB_ERROR_INVALID_FORMAT;
    }

    /* Check for META-INF/container.xml */
    if (zip_stat(zip, "META-INF/container.xml", 0, &stat) != 0) {
        fprintf(stderr, "epub_validate: Missing META-INF/container.xml\n");
        zip_close(zip);
        return EPUB_ERROR_MISSING_CONTAINER;
    }

    zip_close(zip);
    return EPUB_SUCCESS;
}

int epub_parse_container(void *zip_handle, char *opf_path, size_t opf_path_size) {
    zip_t *zip = (zip_t *)zip_handle;
    char *container_xml;
    size_t xml_size;
    xmlDocPtr doc;
    xmlXPathContextPtr xpath_ctx;
    xmlXPathObjectPtr xpath_obj;
    int result = EPUB_ERROR_PARSE_ERROR;

    /* Read container.xml */
    container_xml = zip_read_file(zip, "META-INF/container.xml", &xml_size);
    if (!container_xml) {
        return EPUB_ERROR_MISSING_CONTAINER;
    }

    /* Parse XML */
    doc = xmlReadMemory(container_xml, xml_size, "container.xml", NULL, 0);
    free(container_xml);

    if (!doc) {
        fprintf(stderr, "epub_parse_container: Failed to parse container.xml\n");
        return EPUB_ERROR_PARSE_ERROR;
    }

    /* Create XPath context */
    xpath_ctx = xmlXPathNewContext(doc);
    if (!xpath_ctx) {
        xmlFreeDoc(doc);
        return EPUB_ERROR_PARSE_ERROR;
    }

    /* Register namespace */
    xmlXPathRegisterNs(xpath_ctx, BAD_CAST "ns",
                      BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:container");

    /* Find rootfile element with full-path attribute */
    xpath_obj = xmlXPathEvalExpression(
        BAD_CAST "//ns:rootfile[@full-path]/@full-path", xpath_ctx);

    if (xpath_obj && xpath_obj->nodesetval && xpath_obj->nodesetval->nodeNr > 0) {
        xmlNodePtr node = xpath_obj->nodesetval->nodeTab[0];
        const char *path = (const char *)xmlNodeGetContent(node);
        if (path) {
            strncpy(opf_path, path, opf_path_size - 1);
            opf_path[opf_path_size - 1] = '\0';
            result = EPUB_SUCCESS;
        }
    }

    /* Cleanup */
    if (xpath_obj) {
        xmlXPathFreeObject(xpath_obj);
    }
    xmlXPathFreeContext(xpath_ctx);
    xmlFreeDoc(doc);

    if (result != EPUB_SUCCESS) {
        fprintf(stderr, "epub_parse_container: Could not find OPF path\n");
        return EPUB_ERROR_MISSING_OPF;
    }

    return EPUB_SUCCESS;
}

int epub_parse_opf(void *zip_handle, const char *opf_path, epub_book_t *book) {
    zip_t *zip = (zip_t *)zip_handle;
    char *opf_xml;
    size_t xml_size;
    xmlDocPtr doc;
    xmlXPathContextPtr xpath_ctx;
    xmlXPathObjectPtr xpath_obj;
    int result = EPUB_ERROR_PARSE_ERROR;

    /* Read OPF file */
    opf_xml = zip_read_file(zip, opf_path, &xml_size);
    if (!opf_xml) {
        return EPUB_ERROR_MISSING_OPF;
    }

    /* Parse XML */
    doc = xmlReadMemory(opf_xml, xml_size, opf_path, NULL, 0);
    free(opf_xml);

    if (!doc) {
        fprintf(stderr, "epub_parse_opf: Failed to parse %s\n", opf_path);
        return EPUB_ERROR_PARSE_ERROR;
    }

    /* Create XPath context */
    xpath_ctx = xmlXPathNewContext(doc);
    if (!xpath_ctx) {
        xmlFreeDoc(doc);
        return EPUB_ERROR_PARSE_ERROR;
    }

    /* Register OPF namespace */
    xmlXPathRegisterNs(xpath_ctx, BAD_CAST "opf",
                      BAD_CAST "http://www.idpf.org/2007/opf");
    xmlXPathRegisterNs(xpath_ctx, BAD_CAST "dc",
                      BAD_CAST "http://purl.org/dc/elements/1.1/");

    /* Extract metadata - title */
    xpath_obj = xmlXPathEvalExpression(BAD_CAST "//dc:title", xpath_ctx);
    if (xpath_obj && xpath_obj->nodesetval && xpath_obj->nodesetval->nodeNr > 0) {
        xmlNodePtr node = xpath_obj->nodesetval->nodeTab[0];
        xmlChar *content = xmlNodeGetContent(node);
        if (content) {
            strncpy(book->metadata.title, (const char *)content, 255);
            book->metadata.title[255] = '\0';
            xmlFree(content);
        }
    }
    if (xpath_obj) xmlXPathFreeObject(xpath_obj);

    /* Extract metadata - author */
    xpath_obj = xmlXPathEvalExpression(BAD_CAST "//dc:creator", xpath_ctx);
    if (xpath_obj && xpath_obj->nodesetval && xpath_obj->nodesetval->nodeNr > 0) {
        xmlNodePtr node = xpath_obj->nodesetval->nodeTab[0];
        xmlChar *content = xmlNodeGetContent(node);
        if (content) {
            strncpy(book->metadata.author, (const char *)content, 255);
            book->metadata.author[255] = '\0';
            xmlFree(content);
        }
    }
    if (xpath_obj) xmlXPathFreeObject(xpath_obj);

    /* Extract metadata - language */
    xpath_obj = xmlXPathEvalExpression(BAD_CAST "//dc:language", xpath_ctx);
    if (xpath_obj && xpath_obj->nodesetval && xpath_obj->nodesetval->nodeNr > 0) {
        xmlNodePtr node = xpath_obj->nodesetval->nodeTab[0];
        xmlChar *content = xmlNodeGetContent(node);
        if (content) {
            strncpy(book->metadata.language, (const char *)content, 15);
            book->metadata.language[15] = '\0';
            xmlFree(content);
        }
    }
    if (xpath_obj) xmlXPathFreeObject(xpath_obj);

    /* Build manifest map (id -> href) - simplified, just store in chapters for now */
    /* Extract spine (reading order) */
    xpath_obj = xmlXPathEvalExpression(BAD_CAST "//opf:spine/opf:itemref/@idref", xpath_ctx);

    if (!xpath_obj || !xpath_obj->nodesetval || xpath_obj->nodesetval->nodeNr == 0) {
        fprintf(stderr, "epub_parse_opf: No spine items found\n");
        xmlXPathFreeContext(xpath_ctx);
        xmlFreeDoc(doc);
        return EPUB_ERROR_NO_CONTENT;
    }

    /* Allocate chapter array */
    int spine_count = xpath_obj->nodesetval->nodeNr;
    book->chapters = calloc(spine_count, sizeof(epub_chapter_t));
    if (!book->chapters) {
        xmlXPathFreeObject(xpath_obj);
        xmlXPathFreeContext(xpath_ctx);
        xmlFreeDoc(doc);
        return EPUB_ERROR_OUT_OF_MEMORY;
    }

    /* Get OPF directory path for resolving relative paths */
    char opf_dir[EPUB_MAX_PATH_LENGTH] = "";
    const char *last_slash = strrchr(opf_path, '/');
    if (last_slash) {
        size_t dir_len = last_slash - opf_path + 1;
        strncpy(opf_dir, opf_path, dir_len);
        opf_dir[dir_len] = '\0';
    }

    /* Process each spine item */
    for (int i = 0; i < spine_count && i < EPUB_MAX_CHAPTERS; i++) {
        xmlNodePtr node = xpath_obj->nodesetval->nodeTab[i];
        xmlChar *idref = xmlNodeGetContent(node);

        if (idref) {
            /* Store ID */
            strncpy(book->chapters[i].id, (const char *)idref, 127);
            book->chapters[i].id[127] = '\0';

            /* Find corresponding manifest item */
            char xpath_query[512];
            snprintf(xpath_query, sizeof(xpath_query),
                    "//opf:manifest/opf:item[@id='%s']/@href", (const char *)idref);

            xmlXPathObjectPtr href_obj = xmlXPathEvalExpression(
                BAD_CAST xpath_query, xpath_ctx);

            if (href_obj && href_obj->nodesetval && href_obj->nodesetval->nodeNr > 0) {
                xmlNodePtr href_node = href_obj->nodesetval->nodeTab[0];
                xmlChar *href = xmlNodeGetContent(href_node);
                if (href) {
                    /* Construct full path relative to OPF directory */
                    snprintf(book->chapters[i].href, EPUB_MAX_PATH_LENGTH,
                            "%s%s", opf_dir, (const char *)href);
                    xmlFree(href);
                }
            }

            if (href_obj) xmlXPathFreeObject(href_obj);
            xmlFree(idref);
            book->chapter_count++;
        }
    }

    /* Cleanup */
    xmlXPathFreeObject(xpath_obj);
    xmlXPathFreeContext(xpath_ctx);
    xmlFreeDoc(doc);

    if (book->chapter_count == 0) {
        free(book->chapters);
        book->chapters = NULL;
        return EPUB_ERROR_NO_CONTENT;
    }

    return EPUB_SUCCESS;
}

epub_book_t* epub_open(const char *filepath) {
    epub_book_t *book;
    zip_t *zip;
    int result;
    char opf_path[EPUB_MAX_PATH_LENGTH];

    /* Validate file */
    result = epub_validate(filepath);
    if (result != EPUB_SUCCESS) {
        fprintf(stderr, "epub_open: Validation failed: %s\n",
                epub_error_string(result));
        return NULL;
    }

    /* Open ZIP */
    zip = zip_open(filepath, ZIP_RDONLY, &result);
    if (!zip) {
        fprintf(stderr, "epub_open: Failed to open ZIP\n");
        return NULL;
    }

    /* Allocate book structure */
    book = calloc(1, sizeof(epub_book_t));
    if (!book) {
        zip_close(zip);
        return NULL;
    }

    /* Store filepath and ZIP handle */
    strncpy(book->filepath, filepath, EPUB_MAX_PATH_LENGTH - 1);
    book->filepath[EPUB_MAX_PATH_LENGTH - 1] = '\0';
    book->zip_handle = zip;

    /* Parse container.xml to find OPF path */
    result = epub_parse_container(zip, opf_path, sizeof(opf_path));
    if (result != EPUB_SUCCESS) {
        fprintf(stderr, "epub_open: Failed to parse container: %s\n",
                epub_error_string(result));
        epub_close(book);
        return NULL;
    }

    /* Parse OPF file */
    result = epub_parse_opf(zip, opf_path, book);
    if (result != EPUB_SUCCESS) {
        fprintf(stderr, "epub_open: Failed to parse OPF: %s\n",
                epub_error_string(result));
        epub_close(book);
        return NULL;
    }

    return book;
}

void epub_close(epub_book_t *book) {
    if (!book) {
        return;
    }

    /* Free chapters */
    if (book->chapters) {
        for (int i = 0; i < book->chapter_count; i++) {
            free(book->chapters[i].text);
        }
        free(book->chapters);
    }

    /* Free full text */
    free(book->full_text);

    /* Close ZIP */
    if (book->zip_handle) {
        zip_close((zip_t *)book->zip_handle);
    }

    free(book);
}

int epub_decode_entities(const char *text, size_t text_length,
                        char **out_text, size_t *out_length) {
    char *decoded;
    size_t decoded_pos = 0;
    size_t i = 0;

    /* Allocate output buffer (same size is maximum) */
    decoded = malloc(text_length + 1);
    if (!decoded) {
        return EPUB_ERROR_OUT_OF_MEMORY;
    }

    while (i < text_length) {
        /* Check for entity */
        if (text[i] == '&') {
            int entity_found = 0;

            /* Check named entities */
            for (int j = 0; html_entities[j].entity != NULL; j++) {
                size_t entity_len = strlen(html_entities[j].entity);
                if (i + entity_len <= text_length &&
                    strncmp(&text[i], html_entities[j].entity, entity_len) == 0) {
                    /* Replace entity */
                    const char *replacement = html_entities[j].replacement;
                    while (*replacement) {
                        decoded[decoded_pos++] = *replacement++;
                    }
                    i += entity_len;
                    entity_found = 1;
                    break;
                }
            }

            /* Check numeric entities &#123; or &#xAB; */
            if (!entity_found && i + 3 < text_length && text[i+1] == '#') {
                int value = 0;
                size_t entity_end = i + 2;
                int is_hex = (text[i+2] == 'x' || text[i+2] == 'X');

                if (is_hex) {
                    entity_end++;
                }

                /* Parse number */
                while (entity_end < text_length && text[entity_end] != ';') {
                    char c = text[entity_end];
                    if (is_hex) {
                        if (c >= '0' && c <= '9') {
                            value = value * 16 + (c - '0');
                        } else if (c >= 'a' && c <= 'f') {
                            value = value * 16 + (c - 'a' + 10);
                        } else if (c >= 'A' && c <= 'F') {
                            value = value * 16 + (c - 'A' + 10);
                        } else {
                            break;
                        }
                    } else {
                        if (c >= '0' && c <= '9') {
                            value = value * 10 + (c - '0');
                        } else {
                            break;
                        }
                    }
                    entity_end++;
                }

                if (entity_end < text_length && text[entity_end] == ';' && value > 0 && value < 128) {
                    /* Replace with ASCII character */
                    decoded[decoded_pos++] = (char)value;
                    i = entity_end + 1;
                    entity_found = 1;
                }
            }

            /* If no entity found, copy & as-is */
            if (!entity_found) {
                decoded[decoded_pos++] = text[i++];
            }
        } else {
            /* Regular character */
            decoded[decoded_pos++] = text[i++];
        }
    }

    decoded[decoded_pos] = '\0';
    *out_text = decoded;
    *out_length = decoded_pos;
    return EPUB_SUCCESS;
}

int epub_extract_html_text(const char *html, size_t html_length,
                           char **out_text, size_t *out_length) {
    char *stripped_text;
    size_t stripped_length;
    int result;

    /* Strip HTML tags */
    result = strip_html_tags(html, html_length, &stripped_text, &stripped_length);
    if (result != EPUB_SUCCESS) {
        return result;
    }

    /* Decode HTML entities */
    result = epub_decode_entities(stripped_text, stripped_length, out_text, out_length);
    free(stripped_text);

    return result;
}

int epub_extract_text(epub_book_t *book) {
    size_t total_text_size = 0;
    char *full_text;
    size_t full_text_pos = 0;
    zip_t *zip;

    if (!book || !book->zip_handle) {
        return EPUB_ERROR_INVALID_FORMAT;
    }

    zip = (zip_t *)book->zip_handle;

    /* First pass: extract text from each chapter and calculate total size */
    for (int i = 0; i < book->chapter_count; i++) {
        epub_chapter_t *chapter = &book->chapters[i];

        /* Read chapter HTML */
        size_t html_size;
        char *html = zip_read_file(zip, chapter->href, &html_size);
        if (!html) {
            fprintf(stderr, "epub_extract_text: Failed to read chapter %d (%s)\n",
                    i, chapter->href);
            continue;
        }

        /* Extract text from HTML */
        char *chapter_text;
        size_t chapter_text_length;
        int result = epub_extract_html_text(html, html_size,
                                           &chapter_text, &chapter_text_length);
        free(html);

        if (result != EPUB_SUCCESS) {
            fprintf(stderr, "epub_extract_text: Failed to extract text from chapter %d\n", i);
            continue;
        }

        /* Store chapter text */
        chapter->text = chapter_text;
        chapter->text_length = chapter_text_length;
        total_text_size += chapter_text_length + 2;  /* +2 for "\n\n" separator */
    }

    if (total_text_size == 0) {
        return EPUB_ERROR_NO_CONTENT;
    }

    if (total_text_size > EPUB_MAX_TEXT_SIZE) {
        return EPUB_ERROR_TOO_LARGE;
    }

    /* Allocate full text buffer */
    full_text = malloc(total_text_size + 1);
    if (!full_text) {
        return EPUB_ERROR_OUT_OF_MEMORY;
    }

    /* Concatenate all chapter texts */
    for (int i = 0; i < book->chapter_count; i++) {
        epub_chapter_t *chapter = &book->chapters[i];
        if (chapter->text && chapter->text_length > 0) {
            memcpy(&full_text[full_text_pos], chapter->text, chapter->text_length);
            full_text_pos += chapter->text_length;

            /* Add chapter separator */
            if (i < book->chapter_count - 1) {
                full_text[full_text_pos++] = '\n';
                full_text[full_text_pos++] = '\n';
            }
        }
    }

    full_text[full_text_pos] = '\0';

    book->full_text = full_text;
    book->full_text_length = full_text_pos;

    return EPUB_SUCCESS;
}

epub_chapter_t* epub_get_chapter(epub_book_t *book, int index) {
    if (!book || index < 0 || index >= book->chapter_count) {
        return NULL;
    }
    return &book->chapters[index];
}

int epub_get_text(epub_book_t *book, const char **out_text, size_t *out_length) {
    if (!book) {
        return EPUB_ERROR_INVALID_FORMAT;
    }

    if (!book->full_text) {
        return EPUB_ERROR_NO_CONTENT;
    }

    *out_text = book->full_text;
    *out_length = book->full_text_length;
    return EPUB_SUCCESS;
}
