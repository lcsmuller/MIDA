#ifndef MIDA_H
#define MIDA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @typedef mida_byte
 * @brief Type used for raw byte operations
 *
 * Alias for char used for buffer operations to make it clearer that
 * we're dealing with raw bytes rather than characters.
 */
typedef char mida_byte;

/* Optional hidden signature header (enabled by default; opt-out) */
#ifdef MIDA_SIGNATURE_DISABLE
#undef MIDA_SIGNATURE_ENABLE
#define MIDA_SIGNATURE_ENABLE 0
#endif
#ifndef MIDA_SIGNATURE_ENABLE
#define MIDA_SIGNATURE_ENABLE 1
#endif /* MIDA_SIGNATURE_ENABLE */

/* Signature bytes and size (overridable). Keep it simple by default. */
#if MIDA_SIGNATURE_ENABLE
#ifndef MIDA_SIGNATURE_BYTES
#define MIDA_SIGNATURE_BYTES 'M', 'I', 'D', 'A'
#endif
#ifndef MIDA_SIGNATURE_SIZE
#define MIDA_SIGNATURE_SIZE 4
#endif
#else
#define MIDA_SIGNATURE_SIZE 0
#endif /* MIDA_SIGNATURE_ENABLE */

/* Align prefix to pointer size to preserve typical alignment guarantees */
#define __MIDA_ALIGN        ((size_t)sizeof(void *))
#define __MIDA_PAD_UP(n, a) (((n) + (a) - 1) & ~((a) - 1))
#define MIDA_PREFIX_SIZE    (__MIDA_PAD_UP(MIDA_SIGNATURE_SIZE, __MIDA_ALIGN))

/* Internal helpers to map between views when a prefix is present */
#define __mida_data_from_container(_container_ptr, _container_size)           \
    ((mida_byte *)(_container_ptr) + (_container_size))
#define __mida_container_from_data(_data_ptr, _container_size)                \
    (void *)((mida_byte *)(_data_ptr) - (_container_size))
#define __mida_buffer_from_data(_data_ptr, _container_size)                   \
    ((mida_byte *)__mida_container_from_data((_data_ptr), (_container_size))  \
     - (MIDA_PREFIX_SIZE))

#if MIDA_SIGNATURE_ENABLE
static inline void
__mida_write_signature(mida_byte *buffer)
{
    const mida_byte sig[MIDA_SIGNATURE_SIZE] = { MIDA_SIGNATURE_BYTES };
    /* MIDA_PREFIX_SIZE is padded; write only the signature bytes and zero the
     * rest */
    for (size_t i = 0; i < (size_t)MIDA_SIGNATURE_SIZE; ++i) {
        buffer[i] = sig[i];
    }
    for (size_t i = (size_t)MIDA_SIGNATURE_SIZE; i < (size_t)MIDA_PREFIX_SIZE;
         ++i)
    {
        buffer[i] = 0;
    }
}

static inline int
__mida_check_signature(const mida_byte *buffer)
{
    const mida_byte sig[MIDA_SIGNATURE_SIZE] = { MIDA_SIGNATURE_BYTES };
    for (size_t i = 0; i < (size_t)MIDA_SIGNATURE_SIZE; ++i) {
        if (buffer[i] != sig[i]) {
            return 0;
        }
    }
    return 1;
}
#else
static inline void
__mida_write_signature(mida_byte *buffer)
{
    (void)buffer; /* no-op when disabled */
}
static inline int
__mida_check_signature(const mida_byte *buffer)
{
    (void)buffer;
    return 1; /* always valid when disabled */
}
#endif /* MIDA_SIGNATURE_ENABLE */

#if __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define MIDA_WITH_C99
#endif /* __STDC_VERSION__ */

#ifdef MIDA_STATIC
#define MIDA_API static
#else
#define MIDA_API extern
#endif /* MIDA_STATIC */

/* moved typedef earlier to be available for inline helpers */

/**
 * @def MIDA_BYTEMAP(_container, _bytemap, _size)
 * @brief Defines an extended bytemap for storing metadata
 *
 * Creates a bytemap buffer with local storage, of the appropriate size to hold
 * both the metadata structure and the user data.
 *
 * @param _container Name of the container structure
 * @param _bytemap Name of the bytemap array
 * @param _sizeof Size of the data that will be stored with metadata
 */
#define MIDA_BYTEMAP(_container, _bytemap, _sizeof)                           \
    mida_byte(_bytemap)[MIDA_PREFIX_SIZE + sizeof(_container) + (_sizeof)]

MIDA_API void *__mida_malloc(const size_t container_size,
                             const size_t element_size,
                             const size_t count);

/**
 * @def mida_malloc(_container, _element_size, _count)
 * @brief Allocates memory for an array with extended metadata
 *
 * This macro allocates memory for an array of elements with the specified
 * element size and count, and adds extended metadata using the provided
 * container type.
 *
 * @param _container Type of the container structure
 * @param _element_size Size of each element in bytes
 * @param _count Number of elements to allocate
 * @return Pointer to the allocated array (not the container)
 */
#define mida_malloc(_container, _element_size, _count)                        \
    __mida_malloc(sizeof(_container), _element_size, _count)

MIDA_API void *__mida_calloc(const size_t container_size,
                             const size_t element_size,
                             const size_t count);

/**
 * @def mida_calloc(_container, _element_size, _count)
 * @brief Allocates and zeros memory for an array with extended metadata
 *
 * This function allocates and zeros memory for an array of elements with the
 * specified element size and count, and adds extended metadata using the
 * provided container type.
 *
 * @param _container Type of the container structure
 * @param _element_size Size of each element in bytes
 * @param _count Number of elements to allocate
 * @return Pointer to the allocated array (not the container)
 */
#define mida_calloc(_container, _element_size, _count)                        \
    __mida_calloc(sizeof(_container), _element_size, _count)

MIDA_API void *__mida_realloc(const size_t container_size,
                              void *base,
                              const size_t element_size,
                              const size_t count);

/**
 * @def mida_realloc(_container, _base, _element_size, _count)
 * @brief Reallocates memory for an array with extended metadata
 *
 * This macro reallocates memory for an array of elements with the specified
 * element size and count, preserving the extended metadata.
 *
 * @param _container Type of the container structure
 * @param _base Pointer to the original data (not the container)
 * @param _element_size Size of each element in bytes
 * @param _count New number of elements to allocate
 * @return Pointer to the reallocated array (not the container)
 */
#define mida_realloc(_container, _base, _element_size, _count)                \
    __mida_realloc(sizeof(_container), _base, _element_size, _count)

/**
 * @def mida_free(_container, _base)
 * @brief Frees memory for an array with extended metadata
 *
 * This macro frees memory for an array with extended metadata.
 *
 * @param _container Type of the container structure
 * @param _base Pointer to the data (not the container)
 */
#define mida_free(_container, _base)                                          \
    free(__mida_buffer_from_data((_base), sizeof(_container)))

MIDA_API void *__mida_nwrap(const size_t container_size,
                            void *data,
                            const size_t size,
                            mida_byte *const container);

/**
 * @def mida_nwrap(_container, _data, _bytemap, _bytemap_size)
 * @brief Wraps existing data with extended metadata
 *
 * This macro wraps existing data with extended metadata using a bytemap,
 *  includes a bytemap_size parameter for fine-tuning the size of the
 *  bytemap buffer.
 *
 * @param _container Type of the container structure
 * @param _data Pointer to the original data
 * @param _bytemap Pointer to the bytemap buffer created with MIDA_BYTEMAP
 * @param _bytemap_size Size of the bytemap buffer
 * @return Pointer to the wrapped data (not the container)
 */
#define mida_nwrap(_container, _data, _bytemap, _bytemap_size)                \
    __mida_nwrap(sizeof(_container), _data,                                   \
                 (_bytemap_size) - MIDA_PREFIX_SIZE - sizeof(_container),     \
                 (mida_byte *)(_bytemap) + MIDA_PREFIX_SIZE)

/**
 * @def mida_wrap(_container, _data, _bytemap)
 * @brief Wraps existing data with extended metadata
 *
 * This macro wraps existing data with extended metadata using a bytemap.
 *
 * @param _container Type of the container structure
 * @param _data Pointer to the original data
 * @param _bytemap Pointer to the bytemap buffer created with MIDA_BYTEMAP
 * @return Pointer to the wrapped data (not the container)
 */
#define mida_wrap(_container, _data, _bytemap)                                \
    mida_nwrap(_container, _data, _bytemap, sizeof(_bytemap))

#ifdef MIDA_WITH_C99

/**
 * @def mida_bytemap(_container, _size)
 * @brief Creates a bytemap buffer with local storage, with the given size
 *
 * C99 only. Creates a compound literal for a bytemap of the specified size.
 *
 * @param _container The type of the container structure
 * @param _sizeof Size of the data that will be stored with metadata
 * @return A compound literal initialized to zero
 */
#define mida_bytemap(_container, _sizeof)                                     \
    (mida_byte[MIDA_PREFIX_SIZE + sizeof(_container) + _sizeof]){ 0 }

/**
 * @def mida_array(_container, _type, ...)
 * @brief Creates an array with extended MIDA metadata
 *
 * C99 only. Creates an unnamed array of the specified type with values
 * provided as a compound literal, and wraps it with extended MIDA metadata.
 *
 * @param _container The type of the container structure
 * @param _type The type of array elements
 * @param ... Compound literal array initialization values
 * @return Pointer to the array with extended MIDA metadata
 */
#define mida_array(_container, _type, ...)                                   \
    (_type *)(mida_wrap(                                                     \
        _container, (_type[])__VA_ARGS__,                                    \
        mida_bytemap(_container, sizeof((_type[])__VA_ARGS__))))

/**
 * @def mida_struct(_container, _type, ...)
 * @brief Creates an unnamed structure with extended MIDA metadata
 *
 * C99 only. Creates a structure of the specified type with values provided
 * as a compound literal, and wraps it with extended MIDA metadata.
 *
 * @param _container The type of the container structure
 * @param _type The structure type
 * @param ... Compound literal structure initialization values
 * @return Pointer to the structure with extended MIDA metadata
 */
#define mida_struct(_container, _type, ...)                                   \
    mida_array(_container, _type, { __VA_ARGS__ })

/**
 * @def mida_string(_container, _string)
 * @brief Creates a string literal with extended MIDA metadata
 *
 * C99 only. Creates a string with the provided string literal and wraps it
 * with extended MIDA metadata.
 *
 * @param _container The type of the container structure
 * @param _string The string literal to be wrapped
 * @return Pointer to the string with extended MIDA metadata
 *
 * @note The returned string can be used with standard string functions
 * while still maintaining access to its metadata.
 */
#define mida_string(_container, _string)                                      \
    (char *)(mida_wrap(_container, _string,                                   \
                       mida_bytemap(_container, sizeof(_string))))

#endif /* MIDA_WITH_C99 */

#define MIDA(_container, _base)                                               \
    ((_container *)((mida_byte *)(_base) - sizeof(_container)))

/* Public helper: validate that a pointer was produced by MIDA (best-effort).
 * Returns 1 when valid, 0 when invalid. When signatures are disabled, this
 * always returns 1 to avoid breaking existing code paths. */
#define mida_is_valid(_container, _base)                                      \
    (__mida_check_signature((const mida_byte *)__mida_buffer_from_data(       \
        (_base), sizeof(_container))))

#ifndef MIDA_HEADER

#include <stdlib.h>

MIDA_API void *
__mida_malloc(const size_t container_size,
              const size_t element_size,
              const size_t count)
{
    const size_t data_size = element_size * count;
    const size_t total_size = MIDA_PREFIX_SIZE + container_size + data_size;
    mida_byte *buffer = malloc(total_size);
    if (!buffer) {
        return NULL;
    }
    if (MIDA_PREFIX_SIZE) {
        __mida_write_signature(buffer);
    }
    mida_byte *container = buffer + MIDA_PREFIX_SIZE;
    return __mida_data_from_container(container, container_size);
}

MIDA_API void *
__mida_calloc(const size_t container_size,
              const size_t element_size,
              const size_t count)
{
    const size_t data_size = element_size * count;
    const size_t total_size = MIDA_PREFIX_SIZE + container_size + data_size;
    mida_byte *buffer = calloc(1, total_size);
    if (!buffer) {
        return NULL;
    }
    if (MIDA_PREFIX_SIZE) {
        __mida_write_signature(buffer);
    }
    mida_byte *container = buffer + MIDA_PREFIX_SIZE;
    return __mida_data_from_container(container, container_size);
}

MIDA_API void *
__mida_realloc(const size_t container_size,
               void *base,
               const size_t element_size,
               const size_t count)
{
    if (base) {
        const size_t data_size = element_size * count;
        const size_t total_size =
            MIDA_PREFIX_SIZE + container_size + data_size;
        mida_byte *original_container =
            (mida_byte *)__mida_container_from_data(base, container_size);
        mida_byte *original_buffer = original_container - MIDA_PREFIX_SIZE;
        mida_byte *buffer = realloc(original_buffer, total_size);
        if (!buffer) {
            return NULL;
        }
        /* Signature preserved by realloc; rewrite just in case of
         * calloc->realloc pattern */
        if (MIDA_PREFIX_SIZE) {
            __mida_write_signature(buffer);
        }
        mida_byte *container = buffer + MIDA_PREFIX_SIZE;
        return __mida_data_from_container(container, container_size);
    }
    return __mida_malloc(container_size, element_size, count);
}

MIDA_API void *
__mida_nwrap(const size_t container_size,
             void *data,
             const size_t size,
             mida_byte *const container)
{
    if (MIDA_PREFIX_SIZE) {
        /* container points to the beginning of the container region; header is
         * just before */
        __mida_write_signature(container - MIDA_PREFIX_SIZE);
    }
    return memcpy(__mida_data_from_container(container, container_size), data,
                  size);
}

#endif /* MIDA_HEADER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MIDA_H */
