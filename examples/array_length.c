#include <stdio.h>
#include "../mida.h"

// Define a custom metadata structure for arrays
typedef struct array_metadata {
    size_t length;
} ArrayMD;

int
main()
{
    // Allocate an array with length metadata
    int *numbers = mida_malloc(ArrayMD, sizeof(int), 5);

    // Set metadata
    ArrayMD *meta = MIDA(ArrayMD, numbers);
    meta->length = 5;

    // Initialize the array
    for (size_t i = 0; i < meta->length; i++) {
        numbers[i] = i * 10;
    }

    // Print the array with its length
    printf("Array length: %zu\n", meta->length);
    printf("Array contents: ");
    for (size_t i = 0; i < meta->length; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    // Free the array
    mida_free(ArrayMD, numbers);

    return 0;
}
