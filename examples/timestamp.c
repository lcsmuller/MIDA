#include <stdio.h>
#include <time.h>
#include "../mida.h"

// Define a custom metadata structure with timestamp
typedef struct timestamp_metadata {
    time_t created;
    time_t modified;
} TimestampMD;

// Helper function to print timestamps
void
print_time(const char *label, time_t timestamp)
{
    char time_str[64];
    struct tm *timeinfo = localtime(&timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("%s: %s\n", label, time_str);
}

int
main()
{
    // Create a structure with timestamp metadata
    struct data {
        int value;
        char name[32];
    };

    // Allocate and initialize the structure
    struct data *record = mida_malloc(TimestampMD, sizeof(struct data), 1);
    record->value = 42;
    strcpy(record->name, "Example Data");

    // Set metadata
    TimestampMD *meta = MIDA(TimestampMD, record);
    meta->created = time(NULL);
    meta->modified = meta->created;

    // Print initial information
    printf("Record: %s (value: %d)\n", record->name, record->value);
    print_time("Created", meta->created);
    print_time("Modified", meta->modified);

    // Modify the record and update the timestamp
    printf("\nModifying the record...\n");
    record->value = 100;
    meta->modified = time(NULL);

    // Print updated information
    printf("Record: %s (value: %d)\n", record->name, record->value);
    print_time("Created", meta->created);
    print_time("Modified", meta->modified);

    // Free the record
    mida_free(TimestampMD, record);

    return 0;
}
