#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../mida.h"

// Define a custom metadata structure with various fields
typedef struct record_metadata {
    size_t record_count;
    unsigned int version;
    char author[32];
    time_t timestamp;
    int flags;
} RecordMD;

// Define our custom data structure
typedef struct person {
    char name[64];
    int age;
    float salary;
} Person;

// Helper to print record flags
void
print_flags(int flags)
{
    printf("Flags: ");
    if (flags & 0x1) printf("ACTIVE ");
    if (flags & 0x2) printf("VERIFIED ");
    if (flags & 0x4) printf("ADMIN ");
    if (flags & 0x8) printf("PREMIUM ");
    printf("\n");
}

int
main()
{
    // Create an array of Person structures with metadata
    Person *employees = mida_malloc(RecordMD, sizeof(Person), 3);

    // Set metadata
    RecordMD *meta = MIDA(RecordMD, employees);
    meta->record_count = 3;
    meta->version = 1;
    strcpy(meta->author, "John Doe");
    meta->timestamp = time(NULL);
    meta->flags = 0x3; // ACTIVE | VERIFIED

    // Fill in the employee records
    strcpy(employees[0].name, "Alice Johnson");
    employees[0].age = 32;
    employees[0].salary = 75000.0f;

    strcpy(employees[1].name, "Bob Smith");
    employees[1].age = 45;
    employees[1].salary = 92000.0f;

    strcpy(employees[2].name, "Carol Davis");
    employees[2].age = 28;
    employees[2].salary = 68000.0f;

    // Print metadata about the record set
    printf("Employee Records (Version %u)\n", meta->version);
    printf("Record Count: %zu\n", meta->record_count);
    printf("Author: %s\n", meta->author);

    char time_str[64];
    struct tm *timeinfo = localtime(&meta->timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("Timestamp: %s\n", time_str);

    print_flags(meta->flags);

    // Print the employee records
    printf("\nEmployee Records:\n");
    printf("%-20s %-5s %-10s\n", "Name", "Age", "Salary");
    printf("---------------------------------------\n");

    for (size_t i = 0; i < meta->record_count; i++) {
        printf("%-20s %-5d $%-10.2f\n", employees[i].name, employees[i].age,
               employees[i].salary);
    }

    // Free the records
    mida_free(RecordMD, employees);

    return 0;
}
