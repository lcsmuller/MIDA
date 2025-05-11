#ifndef MIDA_H
#define MIDA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

#if __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define MIDA_SUPPORTS_C99
#endif /* __STDC_VERSION__ */

#ifdef MIDA_STATIC
#define MIDA_API static
#else
#define MIDA_API extern
#endif /* MIDA_STATIC */

typedef char mida_byte;

#define MIDA_BYTEMAP(_bytemap, _size)                                         \
    mida_byte(_bytemap)[sizeof(struct mida) + (_size)]

// alloc (sizeof(struct mida) - 1) + sizeof(_type[])
// the -1 is to account for mida->data[0] which is not part of the size
struct mida {
    size_t size;
    size_t length;
    mida_byte data[1];
};

MIDA_API void *mida_malloc(size_t element_size, size_t count);

MIDA_API void *mida_calloc(size_t element_size, size_t count);

MIDA_API void *mida_realloc(void *ptr, size_t element_size, size_t count);

MIDA_API void mida_free(void *ptr);

MIDA_API void *_mida_wrap(void *data,
                          const size_t size,
                          const size_t length,
                          mida_byte *const bytemap);

#define mida_wrap(_data, _bytemap)                                            \
    _mida_wrap(_data, sizeof(_bytemap) - sizeof(struct mida),                 \
               (sizeof(_bytemap) - sizeof(struct mida)) / sizeof *_data,      \
               _bytemap)

#ifdef MIDA_SUPPORTS_C99

#define mida_bytemap(_size)                                                   \
    (mida_byte[sizeof(struct mida) + _size])                                  \
    {                                                                         \
        0                                                                     \
    }
#define mida_array(_type, ...)                                                \
    (_type *)(_mida_wrap((_type[])__VA_ARGS__, sizeof((_type[])__VA_ARGS__),  \
                         sizeof((_type[])__VA_ARGS__) / sizeof(_type),        \
                         mida_bytemap(sizeof((_type[])__VA_ARGS__))))
#define mida_struct(_type, ...) mida_array(_type, { __VA_ARGS__ })

#endif /* MIDA_SUPPORTS_C99 */

#define mida_container(_base)                                                 \
    ((const struct mida *)((mida_byte *)(_base) - offsetof(struct mida, data)))
#define mida_sizeof(_base) mida_container(_base)->size
#define mida_length(_base) mida_container(_base)->length

#ifndef MIDA_HEADER

#include <string.h>
#include <stdlib.h>

MIDA_API void *
mida_malloc(size_t element_size, size_t count)
{
    const size_t data_size = element_size * count,
                 total_size = sizeof(struct mida) - 1 + data_size;
    struct mida *mida = malloc(total_size);
    if (!mida) return NULL;
    mida->size = data_size;
    mida->length = count;
    return &mida->data;
}

MIDA_API void *
mida_calloc(size_t element_size, size_t count)
{
    const size_t data_size = element_size * count,
                 total_size = sizeof(struct mida) - 1 + data_size;
    struct mida *mida = calloc(1, total_size);
    if (!mida) return NULL;
    mida->size = data_size;
    mida->length = count;
    return &mida->data;
}

MIDA_API void *
mida_realloc(void *ptr, size_t element_size, size_t count)
{
    if (ptr) {
        struct mida *mida = (struct mida *)mida_container(ptr);
        const size_t data_size = element_size * count,
                     total_size = sizeof(struct mida) - 1 + data_size;
        void *tmp = realloc(mida, total_size);
        if (!tmp) return NULL;
        mida = tmp;
        mida->size = data_size;
        mida->length = count;
        return &mida->data;
    }
    return mida_malloc(element_size, count);
}

MIDA_API void
mida_free(void *ptr)
{
    free((void *)mida_container(ptr));
}

MIDA_API void *
_mida_wrap(void *data,
           const size_t size,
           const size_t length,
           mida_byte *const bytemap)
{
    struct mida *mida = (struct mida *)bytemap;
    mida->size = size;
    mida->length = length;
    return memcpy(&mida->data, data, size);
}

#endif /* MIDA_HEADER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MIDA_H */
