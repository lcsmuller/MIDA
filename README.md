# MIDA

MIDA (**M**etadata **I**njection for **D**ata **A**ugmentation) is a lightweight C library that adds metadata tracking to C native structures.

## About

MIDA is a single-header library that attaches size and length metadata to C native structures. It provides wrappers for standard memory functions while transparently storing metadata alongside your data, for easy access and management. This allows you to work with arrays and other data structures without the need for manual bookkeeping, making your code cleaner and less error-prone.

## Key Features

- **Size and length tracking** for arrays without manual bookkeeping
- **Familiar API** through wrappers for `malloc`, `calloc`, `realloc`, and `free`
- **Compound literal support** for creating arrays with metadata attached
- **No dependencies** beyond standard C libraries
- **Header-only** implementation for easy integration
- **Zero overhead** for accessing array elements

## Examples

### Creating Arrays with Compound Literals

```c
// Create arrays with compound literals
int *numbers = mida_compound_literal(int, 1, 2, 3, 4, 5);
char *letters = mida_compound_literal(char, 'a', 'b', 'c', 'd');

// Access the metadata
size_t len = mida_length(numbers);  // 5
size_t size = mida_sizeof(numbers); // 20 bytes (5 * sizeof(int))

// Use them like normal arrays
for (size_t i = 0; i < mida_length(numbers); i++) {
    printf("%d ", numbers[i]);  // Outputs: 1 2 3 4 5
}
```

### Using Memory Management Functions

```c
// Allocate a new array
float *data = mida_malloc(sizeof(float), 10);

// Initialize the array
for (size_t i = 0; i < mida_length(data); i++) {
    data[i] = (float)i * 1.5f;
}

// Resize the array
data = mida_realloc(data, sizeof(float), 20);

// Free the array when done
mida_free(data);
```

## Memory Management

MIDA uses a clever approach to store metadata alongside the array data. Each mida-managed array is prefixed with a small header containing:

- `size`: The total size of the array in bytes
- `length`: The number of elements in the array

The actual array data follows this header, allowing normal array access syntax while maintaining metadata access through helper macros. Here's a simplified view of the memory layout:

```
Memory Layout:
+---------------+------------------+
| MIDA METADATA | ACTUAL ARRAY DATA|
| size, length  | [0][1][2]...[n]  |
+---------------+------------------+
                ^
                |
         Pointer returned to user
```

## Build

mida is single-header-only library, so it includes additional macros for more complex uses cases. `#define MIDA_STATIC` hides all mida API symbols by making them static. Also, if you want to include `mida.h` from multiple C files, to avoid duplication of symbols you may define `MIDA_HEADER` macro.

```c
/* In every .c file that uses mida include only declarations: */
#define MIDA_HEADER
#include "mida.h"

/* Additionally, create one mida.c file for mida implementation: */
#include "mida.h"
```

## License

[MIT License](LICENSE) - see LICENSE file for details
