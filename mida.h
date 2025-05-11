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
 * @def MIDA_BYTEMAP(_bytemap, _size)
 * @brief Defines a bytemap for storing metadata
 *
 * Creates a bytemap buffer of the appropriate size to hold both the
 * metadata structure and the user data.
 *
 * @param _bytemap Name of the bytemap array
 * @param _size Size of the data that will be stored with metadata
 */
#define MIDA_BYTEMAP(_bytemap, _size)                                         \
    mida_byte(_bytemap)[sizeof(struct mida) + (_size)]

/**
 * @struct mida
 * @brief Structure that holds metadata about an array
 *
 * This structure is prefixed to all data managed by MIDA. It stores
 * information about the total size and number of elements in the array.
 * The data field is a flexible array member that marks the start of
 * the actual array data.
 *
 * @note The size calculation for allocation is (sizeof(struct mida) - 1) +
 * sizeof(_type[]), where the -1 accounts for mida->data[0] which is part
 * of the struct but not counted in the size.
 */
struct mida {
    /** Total size of the data in bytes */
    size_t size;
    /** Number of elements in the array */
    size_t length;
    /** Start of the actual array data (flexible array member) */
    mida_byte data[1];
};

/**
 * @brief Allocates memory with MIDA metadata
 *
 * Works like standard malloc but stores metadata about the allocated
 * array, allowing size and length information to be retrieved later.
 *
 * @param element_size Size of each element in bytes
 * @param count Number of elements to allocate
 * @return Pointer to the allocated memory, or NULL if allocation fails
 */
MIDA_API void *mida_malloc(size_t element_size, size_t count);

/**
 * @brief Allocates zeroed memory with MIDA metadata
 *
 * Works like standard calloc but stores metadata about the allocated
 * array, allowing size and length information to be retrieved later.
 *
 * @param element_size Size of each element in bytes
 * @param count Number of elements to allocate
 * @return Pointer to the allocated memory, or NULL if allocation fails
 */
MIDA_API void *mida_calloc(size_t element_size, size_t count);

/**
 * @brief Resizes memory with MIDA metadata
 *
 * Works like standard realloc but preserves metadata about the array,
 * updating it to reflect the new size and length.
 *
 * @param ptr Pointer to memory previously allocated with MIDA functions, or
 * NULL
 * @param element_size Size of each element in bytes
 * @param count Number of elements in the resized array
 * @return Pointer to the resized memory, or NULL if reallocation fails
 */
MIDA_API void *mida_realloc(void *ptr, size_t element_size, size_t count);

/**
 * @brief Frees memory allocated with MIDA functions
 *
 * Works like standard free but accounts for the metadata structure.
 *
 * @param ptr Pointer to memory previously allocated with MIDA functions
 */
MIDA_API void mida_free(void *ptr);

/**
 * @brief Internal function to wrap existing data with MIDA metadata
 *
 * This function copies data into a bytemap buffer and initializes the
 * metadata structure at the beginning of the buffer.
 *
 * @param data Pointer to the data to wrap
 * @param size Total size of the data in bytes
 * @param length Number of elements in the data
 * @param bytemap Buffer to store the metadata and data copy
 * @return Pointer to the data portion of the bytemap
 */
MIDA_API void *_mida_wrap(void *data,
                          const size_t size,
                          const size_t length,
                          mida_byte *const bytemap);

/**
 * @def mida_wrap(_data, _bytemap)
 * @brief Wraps existing data with MIDA metadata
 *
 * Creates a MIDA container for existing data by copying it to a bytemap
 * buffer and initializing the metadata.
 *
 * @param _data Data to wrap with metadata
 * @param _bytemap Bytemap buffer to store the metadata and data
 * @return Pointer to the data portion of the bytemap
 */
#define mida_wrap(_data, _bytemap)                                            \
    _mida_wrap(_data, sizeof(_bytemap) - sizeof(struct mida),                 \
               (sizeof(_bytemap) - sizeof(struct mida)) / sizeof *_data,      \
               _bytemap)

#ifdef MIDA_SUPPORTS_C99

/**
 * @def mida_bytemap(_size)
 * @brief Creates a bytemap buffer with the given size
 *
 * C99 only. Creates a compound literal for a bytemap of the specified size.
 *
 * @param _size Size of the data that will be stored with metadata
 * @return A compound literal initialized to zero
 */
#define mida_bytemap(_size)                                                   \
    (mida_byte[sizeof(struct mida) + _size])                                  \
    {                                                                         \
        0                                                                     \
    }

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
    (_type *)(_mida_wrap((_type[])__VA_ARGS__, sizeof((_type[])__VA_ARGS__),  \
                         sizeof((_type[])__VA_ARGS__) / sizeof(_type),        \
                         mida_bytemap(sizeof((_type[])__VA_ARGS__))))

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
 * @def mida_container(_base)
 * @brief Retrieves the MIDA container structure for a given data pointer
 *
 * Given a pointer to data managed by MIDA, returns a pointer to the
 * container structure that holds its metadata.
 *
 * @param _base Pointer to data managed by MIDA
 * @return Pointer to the mida structure containing the metadata
 */
#define mida_container(_base)                                                 \
    ((const struct mida *)((mida_byte *)(_base) - offsetof(struct mida, data)))

/**
 * @def mida_sizeof(_base)
 * @brief Gets the total size of data managed by MIDA
 *
 * @param _base Pointer to data managed by MIDA
 * @return The total size of the data in bytes
 */
#define mida_sizeof(_base) mida_container(_base)->size

/**
 * @def mida_length(_base)
 * @brief Gets the number of elements in an array managed by MIDA
 *
 * @param _base Pointer to data managed by MIDA
 * @return The number of elements in the array
 */
#define mida_length(_base) mida_container(_base)->length

#ifndef MIDA_HEADER

#include <string.h>
#include <stdlib.h>

/**
 * @brief Allocates memory with MIDA metadata
 *
 * Works like standard malloc but stores metadata about the allocated
 * array, allowing size and length information to be retrieved later.
 *
 * @param element_size Size of each element in bytes
 * @param count Number of elements to allocate
 * @return Pointer to the allocated memory, or NULL if allocation fails
 */
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

/**
 * @brief Allocates zeroed memory with MIDA metadata
 *
 * Works like standard calloc but stores metadata about the allocated
 * array, allowing size and length information to be retrieved later.
 *
 * @param element_size Size of each element in bytes
 * @param count Number of elements to allocate
 * @return Pointer to the allocated memory, or NULL if allocation fails
 */
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

/**
 * @brief Resizes memory with MIDA metadata
 *
 * Works like standard realloc but preserves metadata about the array,
 * updating it to reflect the new size and length.
 *
 * @param ptr Pointer to memory previously allocated with MIDA functions, or
 * NULL
 * @param element_size Size of each element in bytes
 * @param count Number of elements in the resized array
 * @return Pointer to the resized memory, or NULL if reallocation fails
 */
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

/**
 * @brief Frees memory allocated with MIDA functions
 *
 * Works like standard free but accounts for the metadata structure.
 *
 * @param ptr Pointer to memory previously allocated with MIDA functions
 */
MIDA_API void
mida_free(void *ptr)
{
    free((void *)mida_container(ptr));
}

/**
 * @brief Internal function to wrap existing data with MIDA metadata
 *
 * This function copies data into a bytemap buffer and initializes the
 * metadata structure at the beginning of the buffer.
 *
 * @param data Pointer to the data to wrap
 * @param size Total size of the data in bytes
 * @param length Number of elements in the data
 * @param bytemap Buffer to store the metadata and data copy
 * @return Pointer to the data portion of the bytemap
 */
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
