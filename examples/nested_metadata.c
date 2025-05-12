#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mida.h"

// Different metadata for different levels
typedef struct string_metadata {
    size_t length;
} StrMD;

typedef struct array_metadata {
    size_t count;
    char description[64];
} ArrayMD;

typedef struct object_metadata {
    char type[32];
    int id;
} ObjMD;

// Define document structure
typedef struct document {
    char *title;
    int *page_lengths;
    struct document *related_doc;
} Document;

int
main()
{
    // Create a document with different metadata for each component
    Document *doc = mida_malloc(ObjMD, sizeof(Document), 1);

    // Set metadata for document
    ObjMD *doc_meta = MIDA(ObjMD, doc);
    strcpy(doc_meta->type, "Report");
    doc_meta->id = 101;

    // Create title with string metadata
    doc->title = mida_malloc(StrMD, sizeof(char), 22);
    strcpy(doc->title, "Annual Financial Report");
    StrMD *title_meta = MIDA(StrMD, doc->title);
    title_meta->length = strlen(doc->title);

    // Create page lengths array with array metadata
    doc->page_lengths = mida_malloc(ArrayMD, sizeof(int), 5);
    doc->page_lengths[0] = 2; // Summary
    doc->page_lengths[1] = 10; // Introduction
    doc->page_lengths[2] = 25; // Financial Data
    doc->page_lengths[3] = 15; // Analysis
    doc->page_lengths[4] = 5; // Conclusion

    ArrayMD *pages_meta = MIDA(ArrayMD, doc->page_lengths);
    pages_meta->count = 5;
    strcpy(pages_meta->description, "Pages per section");

    // Create related document with object metadata
    doc->related_doc = mida_malloc(ObjMD, sizeof(Document), 1);

    ObjMD *related_meta = MIDA(ObjMD, doc->related_doc);
    strcpy(related_meta->type, "Appendix");
    related_meta->id = 102;

    // Create title for related document
    doc->related_doc->title = mida_malloc(StrMD, sizeof(char), 25);
    strcpy(doc->related_doc->title, "Financial Report Appendix");
    StrMD *rel_title_meta = MIDA(StrMD, doc->related_doc->title);
    rel_title_meta->length = strlen(doc->related_doc->title);

    // No pages or related docs for the appendix
    doc->related_doc->page_lengths = NULL;
    doc->related_doc->related_doc = NULL;

    // Print document information using metadata
    printf("Document Type: %s (ID: %d)\n", doc_meta->type, doc_meta->id);
    printf("Title: %s (Length: %zu characters)\n", doc->title,
           title_meta->length);

    printf("\nSections (%s):\n", pages_meta->description);
    const char *sections[] = { "Summary", "Introduction", "Financial Data",
                               "Analysis", "Conclusion" };
    for (size_t i = 0; i < pages_meta->count; i++) {
        printf("  %s: %d pages\n", sections[i], doc->page_lengths[i]);
    }

    // Calculate total pages
    int total = 0;
    for (size_t i = 0; i < pages_meta->count; i++) {
        total += doc->page_lengths[i];
    }
    printf("Total pages: %d\n", total);

    // Print related document information
    printf("\nRelated Document:\n");
    printf("Type: %s (ID: %d)\n", related_meta->type, related_meta->id);
    printf("Title: %s (Length: %zu characters)\n", doc->related_doc->title,
           rel_title_meta->length);

    // Free all allocated memory
    mida_free(StrMD, doc->related_doc->title);
    mida_free(ObjMD, doc->related_doc);
    mida_free(ArrayMD, doc->page_lengths);
    mida_free(StrMD, doc->title);
    mida_free(ObjMD, doc);

    return 0;
}
