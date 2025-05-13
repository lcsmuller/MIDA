#include "greatest.h"
#include "mida.h"

typedef struct test_metadata {
    size_t size;
    size_t length;
} MD;

void *
_test_array(void *data, size_t size, size_t length)
{
    MIDA(MD, data)->size = size;
    MIDA(MD, data)->length = length;
    return data;
}

void *
_test_struct(void *data, size_t size, size_t length)
{
    MIDA(MD, data)->size = size;
    MIDA(MD, data)->length = length;
    return data;
}

char *
_test_string(char *data, size_t size)
{
    MIDA(MD, data)->size = size;
    MIDA(MD, data)->length = size - 1; // Exclude null terminator
    return data;
}

void *
_test_wrap(void *data, size_t size, size_t length)
{
    MIDA(MD, data)->size = size;
    MIDA(MD, data)->length = length;
    return data;
}

void *
test_malloc(size_t size, size_t length)
{
    void *data = mida_malloc(MD, size, length);
    MIDA(MD, data)->size = size * length;
    MIDA(MD, data)->length = length;
    return data;
}

void *
test_calloc(size_t size, size_t length)
{
    void *data = mida_calloc(MD, size, length);
    MIDA(MD, data)->size = size * length;
    MIDA(MD, data)->length = length;
    return data;
}

void *
test_realloc(void *base, size_t size, size_t length)
{
    void *data = mida_realloc(MD, base, size, length);
    MIDA(MD, data)->size = size * length;
    MIDA(MD, data)->length = length;
    return data;
}

#define test_array(_type, ...)                                                \
    (_type *)_test_array(mida_array(MD, _type, __VA_ARGS__),                  \
                         sizeof((_type[])__VA_ARGS__),                        \
                         sizeof((_type[])__VA_ARGS__) / sizeof(_type))

#define test_struct(_type, ...)                                               \
    (_type *)_test_struct(mida_struct(MD, _type, __VA_ARGS__), sizeof(_type), \
                          1)

#define test_string(_string)                                                  \
    _test_string(mida_string(MD, _string), sizeof(_string))

#define test_wrap(_data, _bytemap)                                            \
    _test_wrap(mida_wrap(MD, _data, _bytemap), sizeof(_bytemap) - sizeof(MD), \
               (sizeof(_bytemap) - sizeof(MD)) / sizeof *_data)

TEST
test_init_compound_literals(void)
{
    struct test {
        int *x;
        float *y;
        struct test *z;
    };

    struct test foo = {
        .x = test_array(int, { 1, 2, 3 }),
        .y = test_array(float, { 1.0f, 2.0f, 3.0f, 4.0f }),
        .z = test_struct(struct test, { .x = test_array(int, { 1, 2, 3 }),
                                        .y = test_array(float, { 1.0f, 2.0f }),
                                        .z = NULL }),
    };

    ASSERT_EQ(3, MIDA(MD, foo.x)->length);
    ASSERT_EQ(4, MIDA(MD, foo.y)->length);
    ASSERT_EQ(1, MIDA(MD, foo.z)->length);
    ASSERT_EQ(3, MIDA(MD, foo.z->x)->length);
    ASSERT_EQ(2, MIDA(MD, foo.z->y)->length);
    ASSERT_EQ(NULL, foo.z->z);
    ASSERT_EQ(sizeof(int[3]), MIDA(MD, foo.x)->size);
    ASSERT_EQ(sizeof(float[4]), MIDA(MD, foo.y)->size);
    ASSERT_EQ(sizeof(struct test), MIDA(MD, foo.z)->size);
    ASSERT_EQ(sizeof(int[3]), MIDA(MD, foo.z->x)->size);
    ASSERT_EQ(sizeof(float[2]), MIDA(MD, foo.z->y)->size);

    PASS();
}

TEST
test_init_bytemap(void)
{
    struct test {
        int *x;
        float *y;
        struct test *z;
    };

    int zx[] = { 1, 2, 3 };
    float zy[] = { 1.0f, 2.0f };
    struct test z = {
        test_wrap(zx, mida_bytemap(MD, sizeof(zx))),
        test_wrap(zy, mida_bytemap(MD, sizeof(zy))),
        NULL,
    };

    int x[] = { 1, 2, 3 };
    float y[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    struct test foo = {
        test_wrap(x, mida_bytemap(MD, sizeof(x))),
        test_wrap(x, mida_bytemap(MD, sizeof(y))),
        test_wrap(&z, mida_bytemap(MD, sizeof(z))),
    };

    ASSERT_EQ(3, MIDA(MD, foo.x)->length);
    ASSERT_EQ(4, MIDA(MD, foo.y)->length);
    ASSERT_EQ(1, MIDA(MD, foo.z)->length);
    ASSERT_EQ(3, MIDA(MD, foo.z->x)->length);
    ASSERT_EQ(2, MIDA(MD, foo.z->y)->length);
    ASSERT_EQ(NULL, foo.z->z);
    ASSERT_EQ(sizeof(int[3]), MIDA(MD, foo.x)->size);
    ASSERT_EQ(sizeof(float[4]), MIDA(MD, foo.y)->size);
    ASSERT_EQ(sizeof(struct test), MIDA(MD, foo.z)->size);
    ASSERT_EQ(sizeof(int[3]), MIDA(MD, foo.z->x)->size);
    ASSERT_EQ(sizeof(float[2]), MIDA(MD, foo.z->y)->size);

    PASS();
}

TEST
test_different_types(void)
{
    char *str_array = test_string("abcd");
    double *double_array = test_array(double, { 1.1, 2.2, 3.3 });

    ASSERT_EQ(4, MIDA(MD, str_array)->length);
    ASSERT_EQ(3, MIDA(MD, double_array)->length);
    ASSERT_EQ(sizeof("abcd"), MIDA(MD, str_array)->size);
    ASSERT_EQ(sizeof(double[3]), MIDA(MD, double_array)->size);

    PASS();
}

TEST
test_large_array(void)
{
    int *large_array =
        test_array(int, { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20 });

    ASSERT_EQ(20, MIDA(MD, large_array)->length);
    ASSERT_EQ(sizeof(int[20]), MIDA(MD, large_array)->size);
    ASSERT_EQ(5, large_array[4]);
    ASSERT_EQ(20, large_array[19]);

    PASS();
}

TEST
test_mida_malloc(void)
{
    int *array = test_malloc(sizeof(int), 5);

    ASSERT_EQ(5, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(int) * 5, MIDA(MD, array)->size);

    for (size_t i = 0; i < MIDA(MD, array)->length; i++) {
        array[i] = (int)i * 10;
    }

    ASSERT_EQ(0, array[0]);
    ASSERT_EQ(10, array[1]);
    ASSERT_EQ(40, array[4]);

    mida_free(MD, array);
    PASS();
}

TEST
test_mida_calloc(void)
{
    int *array = test_calloc(sizeof(int), 5);

    ASSERT_EQ(5, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(int) * 5, MIDA(MD, array)->size);

    for (size_t i = 0; i < MIDA(MD, array)->length; i++) {
        ASSERT_EQ(0, array[i]);
    }

    for (size_t i = 0; i < MIDA(MD, array)->length; i++) {
        array[i] = (int)i + 5;
    }

    ASSERT_EQ(5, array[0]);
    ASSERT_EQ(9, array[4]);

    mida_free(MD, array);
    PASS();
}

TEST
test_mida_realloc(void)
{
    typedef struct metadata {
        size_t size;
        size_t length;
    } MD;

    float *array = test_malloc(sizeof(float), 3);

    ASSERT_EQ(3, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(float) * 3, MIDA(MD, array)->size);

    array[0] = 1.1f;
    array[1] = 2.2f;
    array[2] = 3.3f;

    array = test_realloc(array, sizeof(float), 5);

    // Update metadata after realloc
    MIDA(MD, array)->size = sizeof(float) * 5;
    MIDA(MD, array)->length = 5;

    ASSERT_EQ(5, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(float) * 5, MIDA(MD, array)->size);

    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");
    ASSERT_EQ_FMT(3.3f, array[2], "%.1f");

    array[3] = 4.4f;
    array[4] = 5.5f;

    array = test_realloc(array, sizeof(float), 2);

    // Update metadata after realloc
    MIDA(MD, array)->size = sizeof(float) * 2;
    MIDA(MD, array)->length = 2;

    ASSERT_EQ(2, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(float) * 2, MIDA(MD, array)->size);

    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");

    mida_free(MD, array);
    PASS();
}

TEST
test_mida_realloc_null(void)
{
    char *array = test_realloc(NULL, sizeof(char), 5);

    ASSERT_EQ(5, MIDA(MD, array)->length);
    ASSERT_EQ(sizeof(char) * 5, MIDA(MD, array)->size);
    array[0] = 'T';
    array[1] = 'E';
    array[2] = 'S';
    array[3] = 'T';
    array[4] = '\0';

    ASSERT_STR_EQ("TEST", array);

    mida_free(MD, array);
    PASS();
}

TEST
test_deep_nested_arrays(void)
{
    // Create a structure with nested arrays - strings within arrays within
    // arrays
    struct {
        char ***nested_arrays;
    } container = {
        .nested_arrays = test_array(
            char **,
            { test_array(char *, { test_array(char, { 'f', 'o', 'o', '\0' }),
                                   test_string("bar") }),
              test_array(char *, { test_string("foo") }) })
    };

    // Test the outermost array (has 2 elements)
    ASSERT_EQ(2, MIDA(MD, container.nested_arrays)->length);

    // Test the first inner array (has 2 elements: "foo" and "bar")
    ASSERT_EQ(2, MIDA(MD, container.nested_arrays[0])->length);

    // Test the second inner array (has 1 element: "foo")
    ASSERT_EQ(1, MIDA(MD, container.nested_arrays[1])->length);

    // Test string contents of the innermost arrays
    ASSERT_EQ(4, MIDA(MD, container.nested_arrays[0][0])->length);
    // When using strings, the length excludes the null terminator
    ASSERT_EQ(3, MIDA(MD, container.nested_arrays[0][1])->length);
    ASSERT_EQ(3, MIDA(MD, container.nested_arrays[1][0])->length);

    ASSERT_STR_EQ("foo", container.nested_arrays[0][0]);
    ASSERT_STR_EQ("bar", container.nested_arrays[0][1]);
    ASSERT_STR_EQ("foo", container.nested_arrays[1][0]);

    PASS();
}

TEST
test_shallow_mida_deep_nesting(void)
{
    // Define a regular C array of strings (char arrays)
    const char *regular_strings[] = { "hello", "world" };

    // Define a regular 2D int array
    int regular_2d_array[][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };

    // Create an array where only the outer container is mida-tracked
    // but the inner elements are regular C arrays
    struct {
        void **mixed_array;
    } container = {
        .mixed_array = test_array(
            void *,
            {
                regular_strings, // Regular array of strings
                regular_2d_array, // Regular 2D array
                (const char *[]){ "foo", "bar", "baz" } // Unnamed array
            })
    };

    // Test that the outermost array has metadata
    ASSERT_EQ(3, MIDA(MD, container.mixed_array)->length);
    ASSERT_EQ(sizeof(void *[3]), MIDA(MD, container.mixed_array)->size);

    // Access the inner arrays as regular C arrays (no mida metadata)
    const char **strings = (const char **)container.mixed_array[0];
    ASSERT_STR_EQ("hello", strings[0]);
    ASSERT_STR_EQ("world", strings[1]);

    int(*matrix)[3] = (int(*)[3])container.mixed_array[1];
    ASSERT_EQ(1, matrix[0][0]);
    ASSERT_EQ(5, matrix[1][1]);
    ASSERT_EQ(9, matrix[2][2]);

    const char **more_strings = (const char **)container.mixed_array[2];
    ASSERT_STR_EQ("foo", more_strings[0]);
    ASSERT_STR_EQ("bar", more_strings[1]);
    ASSERT_STR_EQ("baz", more_strings[2]);

    // We can't use MIDA to access metadata on the inner elements
    // because they're not mida-managed arrays

    PASS();
}

TEST
test_custom_metadata(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        int flags;
        char tag[16];
    };

    // Allocate memory with extended metadata
    int *array = mida_malloc(struct custom_metadata, sizeof(int), 5);

    // Set metadata manually
    struct custom_metadata *meta = MIDA(struct custom_metadata, array);
    meta->flags = 42;
    strcpy(meta->tag, "test-tag");

    // Verify data and metadata
    ASSERT_EQ(42, meta->flags);
    ASSERT_STR_EQ("test-tag", meta->tag);

    mida_free(struct custom_metadata, array);
    PASS();
}

TEST
test_custom_calloc(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        int flags;
        float version;
    };

    // Allocate zeroed memory with custom metadata
    int *array = mida_calloc(struct custom_metadata, sizeof(int), 5);

    // Set metadata manually
    struct custom_metadata *meta = MIDA(struct custom_metadata, array);
    meta->flags = 0x1234;
    meta->version = 1.5f;

    // Verify metadata
    ASSERT_EQ(0x1234, meta->flags);
    ASSERT_EQ_FMT(1.5f, meta->version, "%.1f");

    mida_free(struct custom_metadata, array);
    PASS();
}

SUITE(suite_compound_literals)
{
    RUN_TEST(test_init_compound_literals);
    RUN_TEST(test_init_bytemap);
    RUN_TEST(test_different_types);
    RUN_TEST(test_large_array);
    RUN_TEST(test_deep_nested_arrays);
    RUN_TEST(test_shallow_mida_deep_nesting);
}

SUITE(suite_stdlib)
{
    RUN_TEST(test_mida_malloc);
    RUN_TEST(test_mida_calloc);
    RUN_TEST(test_mida_realloc);
    RUN_TEST(test_mida_realloc_null);
}

SUITE(suite_custom_metadata)
{
    RUN_TEST(test_custom_metadata);
    RUN_TEST(test_custom_calloc);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(suite_compound_literals);
    RUN_SUITE(suite_stdlib);
    RUN_SUITE(suite_custom_metadata);
    GREATEST_MAIN_END();
}
