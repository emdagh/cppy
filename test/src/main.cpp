#include <zephyr/ztest.h>


ZTEST(testing, test_asserts) {
    zassert_true(1, "1 was false");
}

ZTEST(testing, test_foo) {
    zassert_false(0, "0 was true");
}

ZTEST_SUITE(testing, NULL, NULL, NULL, NULL, NULL);
