#include "greatest.h"
#include "mida.h"

TEST
test_init_compound_literals(void)
{
    struct test {
        int *x;
        float *y;
        struct test *z;
    };

    struct test foo = {
        .x = mida_array(int, { 1, 2, 3 }),
        .y = mida_array(float, { 1.0f, 2.0f, 3.0f, 4.0f }),
        .z = mida_struct(struct test, { .x = mida_array(int, { 1, 2, 3 }),
                                        .y = mida_array(float, { 1.0f, 2.0f }),
                                        .z = NULL }),
    };

    ASSERT_EQ(3, mida_length(foo.x));
    ASSERT_EQ(4, mida_length(foo.y));
    ASSERT_EQ(1, mida_length(foo.z));
    ASSERT_EQ(3, mida_length(foo.z->x));
    ASSERT_EQ(2, mida_length(foo.z->y));
    ASSERT_EQ(NULL, foo.z->z);
    ASSERT_EQ(sizeof(int[3]), mida_sizeof(foo.x));
    ASSERT_EQ(sizeof(float[4]), mida_sizeof(foo.y));
    ASSERT_EQ(sizeof(struct test), mida_sizeof(foo.z));
    ASSERT_EQ(sizeof(int[3]), mida_sizeof(foo.z->x));
    ASSERT_EQ(sizeof(float[2]), mida_sizeof(foo.z->y));

    PASS();
}

TEST
test_init_c89(void)
{
    struct test {
        int *x;
        float *y;
        struct test *z;
    };

    int zx[] = { 1, 2, 3 };
    MIDA_BYTEMAP(bytemap_zx, sizeof(zx)) = { 0 };
    float zy[] = { 1.0f, 2.0f };
    MIDA_BYTEMAP(bytemap_zy, sizeof(zy)) = { 0 };
    struct test z = {
        mida_wrap(zx, bytemap_zx),
        mida_wrap(zy, bytemap_zy),
        NULL,
    };
    MIDA_BYTEMAP(bytemap_z, sizeof(z)) = { 0 };

    int x[] = { 1, 2, 3 };
    MIDA_BYTEMAP(bytemap_x, sizeof(x)) = { 0 };
    float y[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    MIDA_BYTEMAP(bytemap_y, sizeof(y)) = { 0 };
    struct test foo = {
        mida_wrap(x, bytemap_x),
        mida_wrap(y, bytemap_y),
        mida_wrap(&z, bytemap_z),
    };

    ASSERT_EQ(3, mida_length(foo.x));
    ASSERT_EQ(4, mida_length(foo.y));
    ASSERT_EQ(1, mida_length(foo.z));
    ASSERT_EQ(3, mida_length(foo.z->x));
    ASSERT_EQ(2, mida_length(foo.z->y));
    ASSERT_EQ(NULL, foo.z->z);
    ASSERT_EQ(sizeof(int[3]), mida_sizeof(foo.x));
    ASSERT_EQ(sizeof(float[4]), mida_sizeof(foo.y));
    ASSERT_EQ(sizeof(struct test), mida_sizeof(foo.z));
    ASSERT_EQ(sizeof(int[3]), mida_sizeof(foo.z->x));
    ASSERT_EQ(sizeof(float[2]), mida_sizeof(foo.z->y));

    PASS();
}

TEST
test_different_types(void)
{
    char *str_array = mida_array(char, { 'a', 'b', 'c', 'd' });
    double *double_array = mida_array(double, { 1.1, 2.2, 3.3 });

    ASSERT_EQ(4, mida_length(str_array));
    ASSERT_EQ(3, mida_length(double_array));
    ASSERT_EQ(sizeof(char[4]), mida_sizeof(str_array));
    ASSERT_EQ(sizeof(double[3]), mida_sizeof(double_array));

    PASS();
}

TEST
test_large_array(void)
{
    int *large_array =
        mida_array(int, { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20 });

    ASSERT_EQ(20, mida_length(large_array));
    ASSERT_EQ(sizeof(int[20]), mida_sizeof(large_array));
    ASSERT_EQ(5, large_array[4]);
    ASSERT_EQ(20, large_array[19]);

    PASS();
}

TEST
test_malloc(void)
{
    int *array = mida_malloc(sizeof(int), 5);

    ASSERT_EQ(5, mida_length(array));
    ASSERT_EQ(sizeof(int) * 5, mida_sizeof(array));

    for (size_t i = 0; i < mida_length(array); i++) {
        array[i] = (int)i * 10;
    }

    ASSERT_EQ(0, array[0]);
    ASSERT_EQ(10, array[1]);
    ASSERT_EQ(40, array[4]);

    mida_free(array);
    PASS();
}

TEST
test_calloc(void)
{
    int *array = mida_calloc(sizeof(int), 5);

    ASSERT_EQ(5, mida_length(array));
    ASSERT_EQ(sizeof(int) * 5, mida_sizeof(array));

    for (size_t i = 0; i < mida_length(array); i++) {
        ASSERT_EQ(0, array[i]);
    }

    for (size_t i = 0; i < mida_length(array); i++) {
        array[i] = (int)i + 5;
    }

    ASSERT_EQ(5, array[0]);
    ASSERT_EQ(9, array[4]);

    mida_free(array);
    PASS();
}

TEST
test_realloc(void)
{
    float *array = mida_malloc(sizeof(float), 3);

    ASSERT_EQ(3, mida_length(array));
    ASSERT_EQ(sizeof(float) * 3, mida_sizeof(array));

    array[0] = 1.1f;
    array[1] = 2.2f;
    array[2] = 3.3f;

    array = mida_realloc(array, sizeof(float), 5);

    ASSERT_EQ(5, mida_length(array));
    ASSERT_EQ(sizeof(float) * 5, mida_sizeof(array));

    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");
    ASSERT_EQ_FMT(3.3f, array[2], "%.1f");

    array[3] = 4.4f;
    array[4] = 5.5f;

    array = mida_realloc(array, sizeof(float), 2);

    ASSERT_EQ(2, mida_length(array));
    ASSERT_EQ(sizeof(float) * 2, mida_sizeof(array));

    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");

    mida_free(array);
    PASS();
}

TEST
test_realloc_null(void)
{
    char *array = mida_realloc(NULL, sizeof(char), 4);

    ASSERT_EQ(4, mida_length(array));
    ASSERT_EQ(sizeof(char) * 4, mida_sizeof(array));

    array[0] = 'T';
    array[1] = 'E';
    array[2] = 'S';
    array[3] = 'T';
    ASSERT_EQ('T', array[0]);
    ASSERT_EQ('E', array[1]);
    ASSERT_EQ('S', array[2]);
    ASSERT_EQ('T', array[3]);

    mida_free(array);
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
        .nested_arrays = mida_array(
            char **,
            { mida_array(char *,
                         { mida_array(char, { 'f', 'o', 'o', '\0' }),
                           mida_array(char, { 'b', 'a', 'r', '\0' }) }),
              mida_array(char *,
                         { mida_array(char, { 'f', 'o', 'o', '\0' }) }) })
    };

    // Test the outermost array (has 2 elements)
    ASSERT_EQ(2, mida_length(container.nested_arrays));

    // Test the first inner array (has 2 elements: "foo" and "bar")
    ASSERT_EQ(2, mida_length(container.nested_arrays[0]));

    // Test the second inner array (has 1 element: "foo")
    ASSERT_EQ(1, mida_length(container.nested_arrays[1]));

    // Test string contents of the innermost arrays
    ASSERT_EQ(4, mida_length(container.nested_arrays[0][0]));
    ASSERT_EQ(4, mida_length(container.nested_arrays[0][1]));
    ASSERT_EQ(4, mida_length(container.nested_arrays[1][0]));

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
        .mixed_array =
            mida_array(void *,
                       {
                           (void *)regular_strings, // Regular array of strings
                           (void *)regular_2d_array, // Regular 2D array
                           (void *)(const char *[]){ "foo", "bar",
                                                     "baz" } // Unnamed array
                       })
    };

    // Test that the outermost array has metadata
    ASSERT_EQ(3, mida_length(container.mixed_array));
    ASSERT_EQ(sizeof(void *[3]), mida_sizeof(container.mixed_array));

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

    // We can't use mida_length() or mida_sizeof() on the inner elements
    // because they're not mida-managed arrays

    PASS();
}

TEST
test_ext_malloc(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        int flags;
        char tag[16];
        MIDA_EXT_METADATA;
    };

    // Allocate memory with extended metadata
    int *array = mida_ext_malloc(struct custom_metadata, sizeof(int), 5);

    // Access standard metadata
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(sizeof(int) * 5, mida_ext_sizeof(struct custom_metadata, array));

    // Access custom metadata fields
    struct custom_metadata *meta =
        mida_ext_container(struct custom_metadata, array);
    meta->flags = 42;
    strcpy(meta->tag, "test-tag");

    // Initialize array data
    for (size_t i = 0; i < mida_ext_length(struct custom_metadata, array); i++)
    {
        array[i] = (int)i * 10;
    }

    // Verify data and metadata
    ASSERT_EQ(0, array[0]);
    ASSERT_EQ(10, array[1]);
    ASSERT_EQ(40, array[4]);
    ASSERT_EQ(42, meta->flags);
    ASSERT_STR_EQ("test-tag", meta->tag);

    mida_ext_free(struct custom_metadata, array);
    PASS();
}

TEST
test_ext_calloc(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        int flags;
        float version;
        MIDA_EXT_METADATA;
    };

    // Allocate zeroed memory with extended metadata
    int *array = mida_ext_calloc(struct custom_metadata, sizeof(int), 5);

    // Access custom metadata fields
    struct custom_metadata *meta =
        mida_ext_container(struct custom_metadata, array);
    meta->flags = 0x1234;
    meta->version = 1.5f;

    // Verify data is zeroed
    for (size_t i = 0; i < mida_ext_length(struct custom_metadata, array); i++)
    {
        ASSERT_EQ(0, array[i]);
    }

    // Verify metadata
    ASSERT_EQ(0x1234, meta->flags);
    ASSERT_EQ_FMT(1.5f, meta->version, "%.1f");
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(sizeof(int) * 5, mida_ext_sizeof(struct custom_metadata, array));

    mida_ext_free(struct custom_metadata, array);
    PASS();
}

TEST
test_ext_realloc(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        char name[32];
        MIDA_EXT_METADATA;
    };

    // Allocate memory with extended metadata
    float *array = mida_ext_malloc(struct custom_metadata, sizeof(float), 3);

    // Set custom metadata
    struct custom_metadata *meta =
        mida_ext_container(struct custom_metadata, array);
    strcpy(meta->name, "extended-test");

    // Initialize array data
    array[0] = 1.1f;
    array[1] = 2.2f;
    array[2] = 3.3f;

    // Resize array
    array = mida_ext_realloc(struct custom_metadata, array, sizeof(float), 5);

    // Verify data and metadata persisted
    meta = mida_ext_container(struct custom_metadata, array);
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(sizeof(float) * 5,
              mida_ext_sizeof(struct custom_metadata, array));
    ASSERT_STR_EQ("extended-test", meta->name);
    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");
    ASSERT_EQ_FMT(3.3f, array[2], "%.1f");

    // Add data to new elements
    array[3] = 4.4f;
    array[4] = 5.5f;

    // Resize down
    array = mida_ext_realloc(struct custom_metadata, array, sizeof(float), 2);

    // Verify data and metadata persisted
    meta = mida_ext_container(struct custom_metadata, array);
    ASSERT_EQ(2, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(sizeof(float) * 2,
              mida_ext_sizeof(struct custom_metadata, array));
    ASSERT_STR_EQ("extended-test", meta->name);
    ASSERT_EQ_FMT(1.1f, array[0], "%.1f");
    ASSERT_EQ_FMT(2.2f, array[1], "%.1f");

    mida_ext_free(struct custom_metadata, array);
    PASS();
}

TEST
test_ext_bytemap_wrap(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        unsigned long id;
        MIDA_EXT_METADATA;
    };

    // Create original data
    int data[] = { 10, 20, 30, 40, 50 };

    // Create a bytemap for the extended metadata
    MIDA_EXT_BYTEMAP(struct custom_metadata, bytemap, sizeof(data));

    // Wrap the data with extended metadata
    int *wrapped = mida_ext_wrap(struct custom_metadata, data, bytemap);

    // Set and verify custom metadata
    struct custom_metadata *meta =
        mida_ext_container(struct custom_metadata, wrapped);
    meta->id = 12345UL;

    // Verify standard metadata and data
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, wrapped));
    ASSERT_EQ(sizeof(data), mida_ext_sizeof(struct custom_metadata, wrapped));
    ASSERT_EQ(12345UL, meta->id);
    ASSERT_EQ(10, wrapped[0]);
    ASSERT_EQ(30, wrapped[2]);
    ASSERT_EQ(50, wrapped[4]);

    // No need to free wrapped data - it uses stack memory from bytemap
    PASS();
}

#ifdef MIDA_SUPPORTS_C99
TEST
test_ext_compound_literals(void)
{
    // Define a custom metadata structure with additional fields
    struct custom_metadata {
        long timestamp;
        MIDA_EXT_METADATA;
    };

    // Create an array with extended metadata using compound literals
    int *array =
        mida_ext_array(struct custom_metadata, int, { 5, 6, 7, 8, 9 });

    // Set and access custom metadata
    struct custom_metadata *meta =
        mida_ext_container(struct custom_metadata, array);
    meta->timestamp = 1625000000L;

    // Verify standard metadata and data
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(sizeof(int) * 5, mida_ext_sizeof(struct custom_metadata, array));
    ASSERT_EQ(1625000000L, meta->timestamp);
    ASSERT_EQ(5, array[0]);
    ASSERT_EQ(7, array[2]);
    ASSERT_EQ(9, array[4]);

    // Create a structure with extended metadata
    struct point {
        int x;
        int y;
    };

    struct point *pt = mida_ext_struct(struct custom_metadata, struct point,
                                       { .x = 100, .y = 200 });

    // Access metadata and struct fields
    meta = mida_ext_container(struct custom_metadata, pt);
    meta->timestamp = 1625000001L;

    ASSERT_EQ(1, mida_ext_length(struct custom_metadata, pt));
    ASSERT_EQ(sizeof(struct point),
              mida_ext_sizeof(struct custom_metadata, pt));
    ASSERT_EQ(1625000001L, meta->timestamp);
    ASSERT_EQ(100, pt->x);
    ASSERT_EQ(200, pt->y);

    // No need to free - these use stack memory in compound literals
    PASS();
}
#endif

TEST
test_metadata_conversion(void)
{
    // Define a custom metadata structure
    struct custom_metadata {
        int category;
        MIDA_EXT_METADATA;
    };

    // Allocate memory with extended metadata
    int *array = mida_ext_malloc(struct custom_metadata, sizeof(int), 5);

    // Set initial values
    for (int i = 0; i < 5; i++) {
        array[i] = i * 5;
    }

    // Set custom metadata
    struct custom_metadata *ext_meta =
        mida_ext_container(struct custom_metadata, array);
    ext_meta->category = 42;

    // Access the metadata through both interfaces
    ASSERT_EQ(5, mida_ext_length(struct custom_metadata, array));
    ASSERT_EQ(42, ext_meta->category);

    // Convert to standard metadata
    const struct mida_metadata *std_meta =
        (const struct mida_metadata *)((char *)ext_meta
                                       + offsetof(struct custom_metadata,
                                                  size));

    // Access standard metadata fields
    ASSERT_EQ(5, std_meta->length);
    ASSERT_EQ(sizeof(int) * 5, std_meta->size);

    mida_ext_free(struct custom_metadata, array);
    PASS();
}

SUITE(suite_compound_literals)
{
    RUN_TEST(test_init_compound_literals);
    RUN_TEST(test_init_c89);
    RUN_TEST(test_different_types);
    RUN_TEST(test_large_array);
    RUN_TEST(test_deep_nested_arrays);
    RUN_TEST(test_shallow_mida_deep_nesting);
}

SUITE(suite_stdlib)
{
    RUN_TEST(test_malloc);
    RUN_TEST(test_calloc);
    RUN_TEST(test_realloc);
    RUN_TEST(test_realloc_null);
}

SUITE(suite_extended_metadata)
{
    RUN_TEST(test_ext_malloc);
    RUN_TEST(test_ext_calloc);
    RUN_TEST(test_ext_realloc);
    RUN_TEST(test_ext_bytemap_wrap);
#ifdef MIDA_SUPPORTS_C99
    RUN_TEST(test_ext_compound_literals);
#endif
    RUN_TEST(test_metadata_conversion);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(suite_compound_literals);
    RUN_SUITE(suite_stdlib);
    RUN_SUITE(suite_extended_metadata);
    GREATEST_MAIN_END();
}
