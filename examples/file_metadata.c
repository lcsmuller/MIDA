#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../mida.h"

// Define file-like metadata
typedef struct file_metadata {
    char filename[64];
    char mime_type[32];
    size_t size;
    time_t created;
    unsigned int permissions;
} FileMD;

int
main()
{
    // Create a buffer with file metadata
    char content[] = "This is some example file content.\n"
                     "It has multiple lines of text.\n"
                     "It could represent a document or any other file.";

    // Wrap the data with metadata
    char *file_data =
        mida_wrap(FileMD, content, mida_bytemap(FileMD, sizeof(content)));

    // Set the metadata
    FileMD *meta = MIDA(FileMD, file_data);
    strcpy(meta->filename, "example.txt");
    strcpy(meta->mime_type, "text/plain");
    meta->size = strlen(content);
    meta->created = time(NULL);
    meta->permissions = 0644; // rw-r--r--

    // Print the file information
    printf("File: %s\n", meta->filename);
    printf("MIME Type: %s\n", meta->mime_type);
    printf("Size: %zu bytes\n", meta->size);

    char time_str[64];
    struct tm *timeinfo = localtime(&meta->created);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("Created: %s\n", time_str);

    printf("Permissions: %o\n", meta->permissions);

    printf("\nContent:\n%s\n", file_data);

    // No need to free, as we used a bytemap with automatic storage

    return 0;
}
