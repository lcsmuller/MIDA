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

/**
 * @typedef mida_byte
 * @brief Type used for raw byte operations
 *
 * Alias for char used for buffer operations to make it clearer that
 * we're dealing with raw bytes rather than characters.
 */
typedef char mida_byte;

/**
 * @def MIDA_EXT_BYTEMAP(_container, _bytemap, _size)
 * @brief Defines an extended bytemap for storing metadata
 *
 * Creates a bytemap buffer with local storage, of the appropriate size to hold
 * both the metadata structure and the user data.
 *
 * @param _container Name of the container structure
 * @param _bytemap Name of the bytemap array
 * @param _size Size of the data that will be stored with metadata
 */
#define MIDA_EXT_BYTEMAP(_container, _bytemap, _size)                         \
    mida_byte(_bytemap)[sizeof(_container) + (_size)]

/**
 * @def MIDA_BYTEMAP(_bytemap, _size)
 * @brief Defines a bytemap for storing metadata
 *
 * Creates a bytemap buffer with local storage, of the appropriate size to hold
 * both the metadata structure and the user data.
 *
 * @param _bytemap Name of the bytemap array
 * @param _size Size of the data that will be stored with metadata
 */
#define MIDA_BYTEMAP(_bytemap, _size)                                         \
    MIDA_EXT_BYTEMAP(struct mida_metadata, _bytemap, _size)

/**
 * @def MIDA_EXT_METADATA
 * @brief Defines the metadata structure for MIDA
 * @note When using this macro for extended metadata, ensure that the
 * macro is placed at the start of the struct definition.
 *
 * This macro defines the metadata fields that are used by MIDA to track
 * array size and length. The actual data will follow immediately after
 * the structure in memory.
 */
#define MIDA_EXT_METADATA                                                     \
    /** Total size of the data in bytes */                                    \
    size_t size;                                                              \
    /** Number of elements in the array */                                    \
    size_t length

/**
 * @struct mida_metadata
 * @brief Structure that holds metadata about a structure
 *
 * This structure is prefixed to all data managed by MIDA. It stores
 * information about the total size and number of elements in the array.
 * The actual data immediately follows this structure in memory.
 */
struct mida_metadata {
    MIDA_EXT_METADATA;
};

/**
 * @brief Internal function that allocates memory with metadata
 *
 * This is the core allocation function used by the mida_malloc and
 * mida_ext_malloc macros. It allocates memory for both the metadata container
 * and the actual data.
 *
 * @param container_size Size of the container structure
 * @param element_size Size of each element in the array
 * @param count Number of elements to allocate
 * @return Pointer to the allocated data (not the container)
 */
MIDA_API void *__mida_malloc(const size_t container_size,
                             const size_t element_size,
                             const size_t count);

/**
 * @def mida_ext_malloc(_container, _element_size, _count)
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
#define mida_ext_malloc(_container, _element_size, _count)                    \
    __mida_malloc(sizeof(_container), _element_size, _count)

/**
 * @def mida_malloc(_element_size, _count)
 * @brief Allocates memory for an array with standard metadata
 *
 * This macro allocates memory for an array of elements with the specified
 * element size and count, and adds standard metadata.
 *
 * @param _element_size Size of each element in bytes
 * @param _count Number of elements to allocate
 * @return Pointer to the allocated array (not the container)
 */
#define mida_malloc(_element_size, _count)                                    \
    mida_ext_malloc(struct mida_metadata, _element_size, _count)

/**
 * @brief Internal function that allocates and zeros memory with metadata
 *
 * This is the core allocation function used by the mida_calloc and
 * mida_ext_calloc macros. It allocates and zeros memory for both the metadata
 * container and the actual data.
 *
 * @param container_size Size of the container structure
 * @param element_size Size of each element in the array
 * @param count Number of elements to allocate
 * @return Pointer to the allocated data (not the container)
 */
MIDA_API void *__mida_calloc(const size_t container_size,
                             const size_t element_size,
                             const size_t count);

/**
 * @def mida_ext_calloc(_container, _element_size, _count)
 * @brief Allocates and zeros memory for an array with extended metadata
 *
 * This macro allocates and zeros memory for an array of elements with the
 * specified element size and count, and adds extended metadata using the
 * provided container type.
 *
 * @param _container Type of the container structure
 * @param _element_size Size of each element in bytes
 * @param _count Number of elements to allocate
 * @return Pointer to the allocated array (not the container)
 */
#define mida_ext_calloc(_container, _element_size, _count)                    \
    __mida_calloc(sizeof(_container), _element_size, _count)

/**
 * @def mida_calloc(_element_size, _count)
 * @brief Allocates and zeros memory for an array with standard metadata
 *
 * This macro allocates and zeros memory for an array of elements with the
 * specified element size and count, and adds standard metadata.
 *
 * @param _element_size Size of each element in bytes
 * @param _count Number of elements to allocate
 * @return Pointer to the allocated array (not the container)
 */
#define mida_calloc(_element_size, _count)                                    \
    mida_ext_calloc(struct mida_metadata, _element_size, _count)

/**
 * @brief Internal function that reallocates memory with metadata
 *
 * This is the core reallocation function used by the mida_realloc and
 * mida_ext_realloc macros. It reallocates memory for both the metadata
 * container and the actual data.
 *
 * @param container_size Size of the container structure
 * @param base Pointer to the original data (not the container)
 * @param element_size Size of each element in the array
 * @param count New number of elements to allocate
 * @return Pointer to the reallocated data (not the container)
 */
MIDA_API void *__mida_realloc(const size_t container_size,
                              void *base,
                              const size_t element_size,
                              const size_t count);

/**
 * @def mida_ext_realloc(_container, _base, _element_size, _count)
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
#define mida_ext_realloc(_container, _base, _element_size, _count)            \
    __mida_realloc(sizeof(_container), _base, _element_size, _count)

/**
 * @def mida_realloc(_base, _element_size, _count)
 * @brief Reallocates memory for an array with standard metadata
 *
 * This macro reallocates memory for an array of elements with the specified
 * element size and count, preserving the standard metadata.
 *
 * @param _base Pointer to the original data (not the container)
 * @param _element_size Size of each element in bytes
 * @param _count New number of elements to allocate
 * @return Pointer to the reallocated array (not the container)
 */
#define mida_realloc(_base, _element_size, _count)                            \
    mida_ext_realloc(struct mida_metadata, _base, _element_size, _count)

/**
 * @brief Internal function that frees memory with metadata
 *
 * This is the core deallocation function used by the mida_free and
 * mida_ext_free macros. It frees memory for both the metadata container and
 * the actual data.
 *
 * @param base Pointer to the data (not the container)
 */
MIDA_API void __mida_free(const size_t container_size, void *base);

/**
 * @def mida_ext_free(_container, _base)
 * @brief Frees memory for an array with extended metadata
 *
 * This macro frees memory for an array with extended metadata.
 *
 * @param _container Type of the container structure
 * @param _base Pointer to the data (not the container)
 */
#define mida_ext_free(_container, _base) __mida_free(sizeof(_container), _base)

/**
 * @def mida_free(_base)
 * @brief Frees memory for an array with standard metadata
 *
 * This macro frees memory for an array with standard metadata.
 *
 * @param _base Pointer to the data (not the container)
 */
#define mida_free(_base) mida_ext_free(struct mida_metadata, _base)

/**
 * @brief Internal function that wraps existing data with metadata
 *
 * This is the core wrapping function used by the mida_wrap and mida_ext_wrap
 * macros. It wraps existing data with metadata using a bytemap.
 *
 * @param container_size Size of the container structure
 * @param data Pointer to the original data
 * @param size Size of the data in bytes
 * @param length Number of elements in the data
 * @param bytemap Pointer to the bytemap buffer
 * @return Pointer to the wrapped data (not the container)
 */
MIDA_API void *__mida_wrap(const size_t container_size,
                           void *data,
                           const size_t size,
                           const size_t length,
                           mida_byte *const bytemap);

/**
 * @def mida_ext_wrap(_container, _data, _bytemap)
 * @brief Wraps existing data with extended metadata
 *
 * This macro wraps existing data with extended metadata using a bytemap.
 *
 * @param _container Type of the container structure
 * @param _data Pointer to the original data
 * @param _bytemap Pointer to the bytemap buffer created with MIDA_EXT_BYTEMAP
 * @return Pointer to the wrapped data (not the container)
 */
#define mida_ext_wrap(_container, _data, _bytemap)                            \
    __mida_wrap(                                                              \
        sizeof(_container), _data, sizeof(_bytemap) - sizeof(_container),     \
        (sizeof(_bytemap) - sizeof(_container)) / sizeof *_data, _bytemap)

/**
 * @def mida_wrap(_data, _bytemap)
 * @brief Wraps existing data with standard metadata
 *
 * This macro wraps existing data with standard metadata using a bytemap.
 *
 * @param _data Pointer to the original data
 * @param _bytemap Pointer to the bytemap buffer created with MIDA_BYTEMAP
 * @return Pointer to the wrapped data (not the container)
 */
#define mida_wrap(_data, _bytemap)                                            \
    mida_ext_wrap(struct mida_metadata, _data, _bytemap)

#ifdef MIDA_SUPPORTS_C99

/**
 * @def mida_ext_bytemap(_container, _size)
 * @brief Creates a bytemap buffer with local storage, with the given size
 *
 * C99 only. Creates a compound literal for a bytemap of the specified size.
 *
 * @param _container The type of the container structure
 * @param _size Size of the data that will be stored with metadata
 * @return A compound literal initialized to zero
 */
#define mida_ext_bytemap(_container, _size)                                   \
    (mida_byte[sizeof(_container) + _size])                                   \
    {                                                                         \
        0                                                                     \
    }

/**
 * @def mida_bytemap(_size)
 * @brief Creates a bytemap buffer with local storage, with the given size
 *
 * C99 only. Creates a compound literal for a bytemap of the specified size.
 *
 * @param _size Size of the data that will be stored with metadata
 * @return A compound literal initialized to zero
 */
#define mida_bytemap(_size) mida_ext_bytemap(struct mida_metadata, _size)

/**
 * @def mida_ext_array(_container, _type, ...)
 * @brief Creates an array with extended MIDA metadata
 *
 * C99 only. Creates an array of the specified type with values provided
 * as a compound literal, and wraps it with extended MIDA metadata.
 *
 * @param _container The type of the container structure
 * @param _type The type of array elements
 * @param ... Compound literal array initialization values
 * @return Pointer to the array with extended MIDA metadata
 */
#define mida_ext_array(_container, _type, ...)                                \
    (_type *)(__mida_wrap(                                                    \
        sizeof(_container), (_type[])__VA_ARGS__,                             \
        sizeof((_type[])__VA_ARGS__),                                         \
        sizeof((_type[])__VA_ARGS__) / sizeof(_type),                         \
        mida_ext_bytemap(_container, sizeof((_type[])__VA_ARGS__))))

/**
 * @def mida_array(_type, ...)
 * @brief Creates an array with MIDA metadata
 *
 * C99 only. Creates an array of the specified type with values provided
 * as a compound literal, and wraps it with MIDA metadata.
 *
 * @param _type The type of array elements
 * @param ... Compound literal array initialization values
 * @return Pointer to the array with MIDA metadata
 */
#define mida_array(_type, ...)                                                \
    mida_ext_array(struct mida_metadata, _type, __VA_ARGS__)

/**
 * @def mida_ext_struct(_container, _type, ...)
 * @brief Creates a structure with extended MIDA metadata
 *
 * C99 only. Creates a structure of the specified type with values provided
 * as a compound literal, and wraps it with extended MIDA metadata.
 *
 * @param _container The type of the container structure
 * @param _type The structure type
 * @param ... Compound literal structure initialization values
 * @return Pointer to the structure with extended MIDA metadata
 */
#define mida_ext_struct(_container, _type, ...)                               \
    mida_ext_array(_container, _type, { __VA_ARGS__ })

/**
 * @def mida_struct(_type, ...)
 * @brief Creates a structure with MIDA metadata
 *
 * C99 only. Creates a structure of the specified type with values provided
 * as a compound literal, and wraps it with MIDA metadata.
 *
 * @param _type The structure type
 * @param ... Compound literal structure initialization values
 * @return Pointer to the structure with MIDA metadata
 */
#define mida_struct(_type, ...) mida_array(_type, { __VA_ARGS__ })

#endif /* MIDA_SUPPORTS_C99 */

/**
 * @def mida_ext_container(_container, _base)
 * @brief Retrieves the extended MIDA container structure for a given data
 * pointer
 *
 * Given a pointer to data managed by MIDA, returns a pointer to the
 * extended container structure that holds its metadata.
 *
 * @param _container The type of the container structure
 * @param _base Pointer to data managed by MIDA
 * @return Pointer to the extended container structure containing the metadata
 */
#define mida_ext_container(_container, _base)                                 \
    ((_container *)((mida_byte *)(_base) - sizeof(_container)))

/**
 * @def mida_container(_base)
 * @brief Retrieves the MIDA container structure for a given data pointer
 *
 * Given a pointer to data managed by MIDA, returns a pointer to the
 * container structure that holds its metadata.
 *
 * @param _base Pointer to data managed by MIDA
 * @return Pointer to the mida structure containing the metadata
 */
#define mida_container(_base) mida_ext_container(struct mida_metadata, _base)

/**
 * @def mida_ext_sizeof(_container, _base)
 * @brief Gets the total size of data managed by MIDA
 *
 * @param _container The type of the container structure
 * @param _base Pointer to data managed by MIDA
 * @return The total size of the data in bytes
 */
#define mida_ext_sizeof(_container, _base)                                    \
    mida_ext_container(_container, _base)->size

/**
 * @def mida_sizeof(_base)
 * @brief Gets the total size of data managed by MIDA
 *
 * @param _base Pointer to data managed by MIDA
 * @return The total size of the data in bytes
 */
#define mida_sizeof(_base) mida_ext_sizeof(struct mida_metadata, _base)

/**
 * @def mida_ext_length(_container, _base)
 * @brief Gets the number of elements in an array managed by MIDA
 *
 * @param _container The type of the container structure
 * @param _base Pointer to data managed by MIDA
 * @return The number of elements in the array
 */
#define mida_ext_length(_container, _base)                                    \
    mida_ext_container(_container, _base)->length

/**
 * @def mida_length(_base)
 * @brief Gets the number of elements in an array managed by MIDA
 *
 * @param _base Pointer to data managed by MIDA
 * @return The number of elements in the array
 */
#define mida_length(_base) mida_ext_length(struct mida_metadata, _base)

#ifndef MIDA_HEADER

#include <string.h>
#include <stdlib.h>

#define __mida_data_from_container(_container_ptr, _container_size)           \
    ((mida_byte *)_container_ptr + _container_size)
#define __mida_container_from_data(_data_ptr, _container_size)                \
    ((mida_byte *)_data_ptr - _container_size)

MIDA_API void *
__mida_malloc(const size_t container_size,
              const size_t element_size,
              const size_t count)
{
    const size_t data_size = element_size * count,
                 total_size = container_size + data_size;
    mida_byte *container = malloc(total_size);
    struct mida_metadata *mida = (struct mida_metadata *)container;
    if (!container) return NULL;
    mida->size = data_size;
    mida->length = count;
    return __mida_data_from_container(container, container_size);
}

MIDA_API void *
__mida_calloc(const size_t container_size,
              const size_t element_size,
              const size_t count)
{
    const size_t data_size = element_size * count,
                 total_size = container_size + data_size;
    mida_byte *container = calloc(1, total_size);
    struct mida_metadata *mida = (struct mida_metadata *)container;
    if (!container) return NULL;
    mida->size = data_size;
    mida->length = count;
    return __mida_data_from_container(container, container_size);
}

MIDA_API void *
__mida_realloc(const size_t container_size,
               void *base,
               const size_t element_size,
               const size_t count)
{
    if (base) {
        const size_t data_size = element_size * count,
                     total_size = container_size + data_size;
        mida_byte *original_container =
            __mida_container_from_data(base, container_size);
        mida_byte *container = realloc(original_container, total_size);
        struct mida_metadata *mida = (struct mida_metadata *)container;
        if (!container) return NULL;
        mida->size = data_size;
        mida->length = count;
        return __mida_data_from_container(container, container_size);
    }
    return __mida_malloc(container_size, element_size, count);
}

MIDA_API void
__mida_free(const size_t container_size, void *base)
{
    mida_byte *container = __mida_container_from_data(base, container_size);
    free(container);
}

MIDA_API void *
__mida_wrap(const size_t container_size,
            void *data,
            const size_t size,
            const size_t length,
            mida_byte *const container)
{
    struct mida_metadata *mida = (struct mida_metadata *)container;
    mida->size = size;
    mida->length = length;
    return memcpy(__mida_data_from_container(container, container_size), data,
                  size);
}

#undef _mida_data_from_container
#undef _mida_container_from_data

#endif /* MIDA_HEADER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MIDA_H */
