# MIDA

WIP

## About

WIP

## Key Features

- WIP
- WIP

## Examples

WIP

## Memory Management

WIP

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
