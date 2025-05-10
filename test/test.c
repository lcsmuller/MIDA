#include "greatest.h"
#include "mida.h"

TEST
test_init(void)
{
    struct {
        int *x;
        float *y;
        float *z;
    } foo = {
        .x = mida_compound_literal(int, 1, 2, 3),
        .y = mida_compound_literal(float, 1.0f, 2.0f, 3.0f, 4.0f),
    };

    ASSERT_EQ(3, mida_length(foo.x));
    ASSERT_EQ(4, mida_length(foo.y));
    ASSERT_EQ(sizeof(int[3]), mida_sizeof(foo.x));
    ASSERT_EQ(sizeof(float[4]), mida_sizeof(foo.y));

    PASS();
}

TEST
test_different_types(void)
{
    char *str_array = mida_compound_literal(char, 'a', 'b', 'c', 'd');
    double *double_array = mida_compound_literal(double, 1.1, 2.2, 3.3);

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
        mida_compound_literal(int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20);

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

SUITE(mida_suite)
{
    RUN_TEST(test_init);
    RUN_TEST(test_different_types);
    RUN_TEST(test_large_array);
}

SUITE(mida_stdlib)
{
    RUN_TEST(test_malloc);
    RUN_TEST(test_calloc);
    RUN_TEST(test_realloc);
    RUN_TEST(test_realloc_null);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(mida_suite);
    RUN_SUITE(mida_stdlib);
    GREATEST_MAIN_END();
}
