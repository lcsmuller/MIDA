#include "greatest.h"
#include "mida.h"

static struct mida mida;

static void
setup(void *data)
{
    (void)data;
    mida_init(&mida);
}

static void
teardown(void *data)
{
    (void)data;
    mida_cleanup(&mida);
}

TEST
test_init(void)
{
    ASSERT_EQ(0, mida.length);
    PASS();
}

SUITE(mida_suite)
{
    SET_SETUP(setup, NULL);
    SET_TEARDOWN(teardown, NULL);

    RUN_TEST(test_init);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(mida_suite);
    GREATEST_MAIN_END();
}
