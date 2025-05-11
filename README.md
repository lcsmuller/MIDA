# MIDA

MIDA (**M**etadata **I**njection for **D**ata **A**ugmentation) is a lightweight C library that adds metadata tracking to C native structures.

## About

MIDA is a single-header library that attaches size and length metadata to C native structures. It provides wrappers for standard memory functions while transparently storing metadata alongside your data, for easy access and management. This allows you to work with arrays and other data structures without the need for manual bookkeeping, making your code cleaner and less error-prone.

## Key Features

- **Size and length tracking** for arrays without manual bookkeeping
- **Familiar API** through wrappers for `malloc`, `calloc`, `realloc`, and `free`
- **Compound literal support** for creating arrays with metadata attached
- **Nested structure support** for complex data structures
- **Custom metadata extensions** for adding your own metadata fields
- **No dependencies** beyond standard C libraries
- **Header-only** implementation for easy integration
- **Zero overhead** for accessing array elements

## Examples

### Creating Arrays with Compound Literals

```c
// Create arrays with compound literals (using C99 designated initializers)
int *numbers = mida_array(int, { 1, 2, 3, 4, 5 });
char *letters = mida_array(char, { 'a', 'b', 'c', 'd' });

// Access the metadata
size_t len = mida_length(numbers);  // 5
size_t size = mida_sizeof(numbers); // 20 bytes (5 * sizeof(int))

// Use them like normal arrays
for (size_t i = 0; i < mida_length(numbers); i++) {
    printf("%d ", numbers[i]);  // Outputs: 1 2 3 4 5
}
```

### Creating Nested Structures

```c
// Define a structure with nested arrays
struct my_data {
    int *numbers;
    float *values;
};

// Create a structure with metadata
struct my_data *data = mida_struct(
    struct my_data, {
        .numbers = mida_array(int, { 1, 2, 3 }),
        .values = mida_array(float, { 1.0f, 2.0f, 3.0f })
    }
);

// Access data and metadata
printf("Number of elements: %zu\n", mida_length(data->numbers)); // 3
printf("First value: %f\n", data->values[0]); // 1.0
```

### Deep Nesting

```c
// Create deeply nested arrays (like a 2D string array)
char ***nested = mida_array(
    char **, {
        mida_array(char *, { 
            mida_array(char, { 'f', 'o', 'o', '\0' }),
            mida_array(char, { 'b', 'a', 'r', '\0' })
        }),
        mida_array(char *, {
            mida_array(char, { 'h', 'i', '\0' })
        })
    }
);

// Access elements from deep nesting
printf("%s %s\n", nested[0][0], nested[0][1]); // "foo bar"
printf("%s\n", nested[1][0]); // "hi"

// Access metadata at any level
printf("Outer length: %zu\n", mida_length(nested)); // 2
printf("Inner length: %zu\n", mida_length(nested[0])); // 2
printf("String length: %zu\n", mida_length(nested[0][0])); // 4 (including \0)
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

### Using Extended Metadata

You can define custom metadata structures with additional fields:

```c
// Define a custom metadata structure with extra fields
struct my_metadata {
    int flags;          // Custom field added before MIDA_EXT_METADATA
    char tag[16];       // Custom field added before MIDA_EXT_METADATA
    MIDA_EXT_METADATA;  // Must be the last field in the struct
};

// Use extended malloc to allocate with custom metadata
int *array = mida_ext_malloc(struct my_metadata, sizeof(int), 10);

// Access the custom metadata through the container
struct my_metadata *meta = mida_ext_container(struct my_metadata, array);
meta->flags = 0x1;
strcpy(meta->tag, "important");

// Standard metadata still works
printf("Array length: %zu\n", mida_ext_length(struct my_metadata, array));

// Clean up with extended free
mida_ext_free(struct my_metadata, array);
```

## C89 Compatibility Mode (without Compound Literals)

MIDA provides an alternative API for C89 compatibility:

```c
// Create a bytemap buffer to hold data and metadata
int data[] = {1, 2, 3, 4, 5};
MIDA_BYTEMAP(bytemap, sizeof(data));

// Wrap existing data with metadata
int *wrapped = mida_wrap(data, bytemap);

// Access metadata as usual
printf("Length: %zu\n", mida_length(wrapped)); // 5
```

For extended metadata in C89:

```c
// Define custom metadata structure
struct my_metadata {
    MIDA_EXT_METADATA;
    int flags;
};

// Create an extended bytemap
int data[] = {1, 2, 3, 4, 5};
MIDA_EXT_BYTEMAP(struct my_metadata, bytemap, sizeof(data));

// Wrap with extended metadata
int *wrapped = mida_ext_wrap(struct my_metadata, data, bytemap);

// Access extended metadata
struct my_metadata *meta = mida_ext_container(struct my_metadata, wrapped);
meta->flags = 1;
```

## Memory Management

MIDA uses a clever approach to store metadata alongside the array data. Each mida-managed array is prefixed with a small header containing:

- `size`: The total size of the array in bytes
- `length`: The number of elements in the array

The actual array data follows this header, allowing normal array access syntax while maintaining metadata access through helper macros. Here's a simplified view of the memory layout:

```
Standard Memory Layout:
+---------------+------------------+
| MIDA METADATA | ACTUAL ARRAY DATA|
| size, length  | [0][1][2]...[n]  |
+---------------+------------------+
                ^
                |
         Pointer returned to user

Extended Memory Layout:
+---------------------+------------------+
| EXTENDED METADATA   | ACTUAL ARRAY DATA|
| custom fields,      | [0][1][2]...[n]  |
| size, length        |                  |
+---------------------+------------------+
                      ^
                      |
               Pointer returned to user
```

## Mixing with Regular C Arrays

You can also use MIDA to wrap only the outermost container while using regular C arrays inside:

```c
// Regular C arrays
int matrix[][2] = { {1, 2}, {3, 4} };
const char *names[] = {"Alice", "Bob"};

// Wrap in a MIDA container
void **container = mida_array(
    void *, {
        (void*)matrix,
        (void*)names
    }
);

// Only the container has metadata
printf("Container size: %zu\n", mida_length(container)); // 2

// Access inner arrays normally (casting back to appropriate type)
int (*m)[2] = (int(*)[2])container[0];
printf("Matrix value: %d\n", m[1][0]); // 3

const char **n = (const char**)container[1];
printf("Name: %s\n", n[1]); // "Bob"
```

## API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `void *mida_malloc(size_t element_size, size_t count)` | Allocates memory with metadata for `count` elements of size `element_size` |
| `void *mida_calloc(size_t element_size, size_t count)` | Allocates zeroed memory with metadata for `count` elements of size `element_size` |
| `void *mida_realloc(void *ptr, size_t element_size, size_t count)` | Resizes memory with metadata for `count` elements of size `element_size` |
| `void mida_free(void *ptr)` | Frees memory allocated with MIDA functions |

### Extended Functions (Custom Metadata)

| Function | Description |
|----------|-------------|
| `void *mida_ext_malloc(container_type, size_t element_size, size_t count)` | Allocates memory with extended metadata |
| `void *mida_ext_calloc(container_type, size_t element_size, size_t count)` | Allocates zeroed memory with extended metadata |
| `void *mida_ext_realloc(container_type, void *ptr, size_t element_size, size_t count)` | Resizes memory with extended metadata |
| `void mida_ext_free(container_type, void *ptr)` | Frees memory with extended metadata |

### C99 Macros (Compound Literals)

| Macro | Description |
|-------|-------------|
| `mida_array(type, {...})` | Creates an array with metadata using compound literals |
| `mida_struct(type, {...})` | Creates a structure with metadata using compound literals |
| `mida_bytemap(size)` | Creates a bytemap buffer with the specified size (for internal use) |
| `mida_ext_array(container_type, type, {...})` | Creates an array with extended metadata |
| `mida_ext_struct(container_type, type, {...})` | Creates a structure with extended metadata |
| `mida_ext_bytemap(container_type, size)` | Creates a bytemap for extended metadata |

### C89 Compatibility Macros

| Macro | Description |
|-------|-------------|
| `MIDA_BYTEMAP(bytemap, size)` | Defines a bytemap buffer to hold metadata and data |
| `MIDA_EXT_BYTEMAP(container_type, bytemap, size)` | Defines a bytemap for extended metadata |
| `mida_wrap(data, bytemap)` | Wraps existing data with metadata using a bytemap buffer |
| `mida_ext_wrap(container_type, data, bytemap)` | Wraps data with extended metadata |

### Metadata Access Macros

| Macro | Description |
|-------|-------------|
| `mida_sizeof(ptr)` | Gets the total size in bytes of data managed by MIDA |
| `mida_length(ptr)` | Gets the number of elements in an array managed by MIDA |
| `mida_container(ptr)` | Gets the container structure for a given data pointer |
| `mida_ext_sizeof(container_type, ptr)` | Gets size for extended metadata |
| `mida_ext_length(container_type, ptr)` | Gets length for extended metadata |
| `mida_ext_container(container_type, ptr)` | Gets container for extended metadata |

### Extended Metadata Definition

| Macro | Description |
|-------|-------------|
| `MIDA_EXT_METADATA` | Define metadata fields in custom container structs |

## Build

MIDA is a single-header-only library, so it includes additional macros for more complex use cases. `#define MIDA_STATIC` hides all mida API symbols by making them static. Also, if you want to include `mida.h` from multiple C files, to avoid duplication of symbols you may define `MIDA_HEADER` macro.

```c
/* In every .c file that uses MIDA include only declarations: */
#define MIDA_HEADER
#include "mida.h"

/* Additionally, create one mida.c file for MIDA implementation: */
#include "mida.h"
```

## License

[MIT License](LICENSE) - see LICENSE file for details
