# MIDA

MIDA (**M**etadata **I**njection for **D**ata **A**ugmentation) is a lightweight C library that injects and manages metadata alongside C native structures.

## Table of Contents

- [About](#about)
- [Key Features](#key-features)
- [Examples](#examples)
  - [Basic Metadata Injection](#basic-metadata-injection)
    - [Dynamic Storage](#dynamic-storage)
    - [Local Storage](#local-storage)
  - [Injecting Custom Metadata](#injecting-custom-metadata)
  - [Working with Strings](#working-with-strings)
  - [Using C99 Compound Literals](#using-c99-compound-literals-with-custom-metadata)
  - [Working with Nested Data Structures](#working-with-nested-data-structures)
- [Memory Management](#memory-management)
- [API Reference](#api-reference)
- [Build](#build)
- [Examples and Tests](#examples-and-tests)
- [License](#license)

## About

MIDA is a single-header library designed for seamless metadata injection into C data structures. This allows you to attach arbitrary metadata to your data, from simple tracking information to complex custom metadata structures. The library provides a clean API that makes metadata management transparent, letting you focus on your application logic while the metadata is handled automatically.

The power of MIDA comes from its ability to inject and manage custom metadata for any data structure, giving you complete flexibility in defining what information you want to attach to your data.

## Key Features

- **Flexible metadata injection** for any C data structure
- **Custom metadata structure support** for defining your own fields
- **Zero-copy transparent access** to both data and metadata
- **Familiar API** through wrappers for standard memory functions
- **Compound literal support** in C99 for creating data with metadata attached
- **No dependencies** beyond standard C libraries
- **Header-only** implementation for easy integration
- **Zero overhead** for accessing the original data

## Examples

### Basic Metadata Injection

#### Dynamic Storage

```c
// Define a custom metadata structure
typedef struct array_metadata {
    size_t length;
} ArrayMD;

// Allocate an array with metadata
int *numbers = mida_malloc(ArrayMD, sizeof(int), 5);
// Access and set the metadata via the MIDA macro
MIDA(ArrayMD, numbers)->length = 5;
// Initialize data
for (size_t i = 0; i < MIDA(ArrayMD, numbers)->length; i++) {
    numbers[i] = i + 1;
}

printf("Array length: %zu\n", MIDA(ArrayMD, numbers)->length);  // 5
// Clean up
mida_free(ArrayMD, numbers);
```

#### Local Storage

```c
// Define a custom metadata structure
typedef struct array_metadata {
    size_t length;
} ArrayMD;

// Allocate an array with metadata
int data[5] = {0};
MIDA_BYTEMAP(ArrayMD, bytemap, sizeof(data));
int *numbers = mida_wrap(ArrayMD, data, bytemap);

// Access and set the metadata via the MIDA macro
MIDA(ArrayMD, numbers)->length = sizeof(data) / sizeof(data[0]);
// Initialize data
for (size_t i = 0; i < MIDA(ArrayMD, numbers)->length; i++) {
    numbers[i] = i + 1;
}

printf("Array length: %zu\n", MIDA(ArrayMD, numbers)->length);  // 5
```

Or if you want to use C99 compound literals, you can skip the `MIDA_BYTEMAP` macro, and
use `mida_bytemap` directly in the `mida_wrap` function:

```c
... // Previous code

int data[5] = {0};
int *numbers = mida_wrap(ArrayMD, data, mida_bytemap(ArrayMD, sizeof(data)));

... // Continue with the rest of the code
```

### Injecting Custom Metadata

```c
// Define custom metadata with your own fields
struct custom_metadata {
    int flags;
    char *owner;
    double timestamp;
};

// Allocate data with custom metadata
float *data = mida_malloc(struct custom_metadata, sizeof(float), 10);

// Access the custom metadata
MIDA(struct custom_metadata, data)->flags = 0x1;
MIDA(struct custom_metadata, data)->owner = "example_user";
MIDA(struct custom_metadata, data)->timestamp = 1625000000.0;
// Use the data normally
for (size_t i = 0; i < 10; i++) {
    data[i] = (float)i * 1.5f;
}

// Clean up
mida_free(struct custom_metadata, data);
```

### Working with Strings

MIDA provides convenient functions for handling strings with metadata:

```c
// Define a metadata structure for strings
typedef struct string_metadata {
    size_t length;
} StrMD;

// Create a string with metadata
char *message = mida_string(StrMD, "Hello, World!");
// Set the length
MIDA(StrMD, message)->length = strlen(message);
// Access and use like a normal string
printf("String length: %zu\n", MIDA(StrMD, message)->length);  // 13
printf("Message: %s\n", message);
printf("First character: %c\n", message[0]);  // 'H'

// Create strings with custom metadata
typedef struct extended_string_metadata {
    size_t length;
    char *language;
    time_t created;
} ExtendedStrMD;

char *greeting = mida_string(ExtendedStrMD, "Good morning!");
// Access the custom metadata
MIDA(ExtendedStrMD, greeting)->length = strlen(greeting);
MIDA(ExtendedStrMD, greeting)->language = "English";
MIDA(ExtendedStrMD, greeting)->created = time(NULL);
// String functions still work normally
printf("Greeting length (standard): %zu\n", strlen(greeting));
printf("Greeting length (MIDA): %zu\n", MIDA(ExtendedStrMD, greeting)->length);
// Fully compatible with standard C string functions
if (strcmp(greeting, "Good morning!") == 0) {
    printf("Strings match!\n");
}
```

### Using C99 Compound Literals with Custom Metadata

```c
// Define custom metadata structure
typedef struct file_metadata {
    char *filename;
    unsigned long permissions;
    time_t modified;
} FileMD;

// Create a struct with custom metadata using compound literals
struct point {
    double x, y, z;
};

struct point *location = mida_struct(
    FileMD,
    struct point, {
        .x = 10.5,
        .y = 20.3,
        .z = 5.7
    }
);
// Set custom metadata
MIDA(FileMD, location)->filename = "point_data.bin";
MIDA(FileMD, location)->permissions = 0644;
MIDA(FileMD, location)->modified = time(NULL);
// Use the data as normal
printf("Location: (%f, %f, %f)\n", location->x, location->y, location->z);
```

### Working with Nested Data Structures

```c
// Create a complex structure with metadata at multiple levels
struct person {
    char *name;
    int *scores;
    struct person *manager;
};

// Different metadata for different parts
typedef struct name_meta {
    size_t length;
    const char *language;
} NameMD;

typedef struct scores_meta {
    size_t count;
    const char *subject;
    double average;
} ScoresMD;

typedef struct manager_meta {
    int department_id;
} ManagerMD;

// Create the structure with different metadata at each level
struct person employee = {
    .name = mida_string(NameMD, "John Doe"),
    .scores = mida_array(ScoresMD, int, {85, 92, 78, 90}),
    .manager = mida_struct(ManagerMD, struct person, {
        .name = mida_string(NameMD, "Boss"),
        .scores = NULL,
        .manager = NULL
    })
};

// Access and set specific metadata for each component
NameMD *name_info = MIDA(NameMD, employee.name);
name_info->length = strlen(employee.name);
name_info->language = "English";

ScoresMD *scores_info = MIDA(ScoresMD, employee.scores);
scores_info->count = 4;
scores_info->subject = "Computer Science";
scores_info->average = 86.25;

ManagerMD *mgr_info = MIDA(ManagerMD, employee.manager);
mgr_info->department_id = 42;

// Use the data naturally
printf("Employee: %s, Average: %.2f, Manager: %s, Dept: %d\n",
       employee.name, scores_info.average, employee.manager->name, mgr_info->department_id);
```

## Memory Management

MIDA uses a clever approach to store metadata alongside your data. The metadata is stored immediately before the data pointer that's returned to you, allowing for:

1. Direct access to your data with zero overhead
2. Easy retrieval of metadata when needed
3. Transparent memory management

Here's a simplified view of the memory layout:

```
Memory Layout:
+------------------+------------------+
| CUSTOM METADATA  | ACTUAL DATA      |
| (your structure) | [your data here] |
+------------------+------------------+
                   ^
                   |
            Pointer returned to user
```

## API Reference

### Core Metadata Function

| Function | Description |
|----------|-------------|
| `MIDA(container_type, ptr)` | Gets the container structure for a given data pointer |

### Memory Management Functions

| Function | Description |
|----------|-------------|
| `mida_malloc(container_type, element_size, count)` | Allocates memory with metadata |
| `mida_calloc(container_type, element_size, count)` | Allocates zeroed memory with metadata |
| `mida_realloc(container_type, base, element_size, count)` | Resizes memory with metadata |
| `mida_free(container_type, base)` | Frees memory with metadata |
| `MIDA_BYTEMAP(container_type, bytemap, size)` | Defines a bytemap buffer for local storage metadata |
| `mida_nwrap(container_type, data, bytemap, bytemap_size)` | Wraps data with metadata with bytemap size |
| `mida_wrap(container_type, data, bytemap)` | Wraps data with metadata |

### C99 Macros (Compound Literals)

| Macro | Description |
|-------|-------------|
| `mida_array(container_type, type, {...})` | Creates an unnamed array with metadata |
| `mida_struct(container_type, type, {...})` | Creates an unnamed structure with metadata |
| `mida_string(container_type, string)` | Creates a string-literal with metadata |
| `mida_bytemap(container_type, size)` | Creates a unnamed bytemap for metadata |

## Build

MIDA is a single-header-only library with flexible inclusion options:

```c
/* In every .c file that uses MIDA include only declarations: */
#define MIDA_HEADER
#include "mida.h"

/* Additionally, create one mida.c file for MIDA implementation: */
#include "mida.h"
```

To make all MIDA functions static (to avoid symbol conflicts), use:

```c
#define MIDA_STATIC
#include "mida.h"
```

## Examples and Tests

For more examples and tests, please refer to the [examples](examples) and [tests](tests) directories in the repository.

## License

[MIT License](LICENSE) - see LICENSE file for details
