// Unit tests for assignment4 helpers (CLI parser and size generator).
// Uses Unity test framework to validate parsing, defaults, and geometric sequences.

#include "assignment4/cli.h"
#include "assignment4/sizes.h"

extern "C" {
#include "vendor/unity/unity.h"
}

#include <vector>
#include <string>

using namespace assignment4;

// Test: CLI parser returns correct default values when no args provided
static void test_cli_defaults(void)
{
    Options opt;
    std::string err;
    const char* argv0 = "assignment4";
    char* argv[] = { (char*)argv0, 0 };
    TEST_ASSERT_TRUE(parse_cli(1, argv, opt, err));
    TEST_ASSERT_INT_EQUAL(10, opt.warmup);
    TEST_ASSERT_INT_EQUAL(100, opt.iters);
    TEST_ASSERT_INT_EQUAL(4, opt.min_bytes);
    TEST_ASSERT_INT_EQUAL(10485760, opt.max_bytes);
    TEST_ASSERT_INT_EQUAL(2, opt.factor);
}

// Test: CLI parser correctly sets custom values from arguments
static void test_cli_custom(void)
{
    Options opt;
    std::string err;
    const char* argv0 = "assignment4";
    const char* a1 = "--warmup"; const char* v1 = "5";
    const char* a2 = "--iters";  const char* v2 = "50";
    const char* a3 = "--min-bytes"; const char* v3 = "4";
    const char* a4 = "--max-bytes"; const char* v4 = "64";
    const char* a5 = "--factor"; const char* v5 = "2";
    char* argv[] = { (char*)argv0,
                     (char*)a1, (char*)v1,
                     (char*)a2, (char*)v2,
                     (char*)a3, (char*)v3,
                     (char*)a4, (char*)v4,
                     (char*)a5, (char*)v5,
                     0 };
    TEST_ASSERT_TRUE(parse_cli(11, argv, opt, err));
    TEST_ASSERT_INT_EQUAL(5, opt.warmup);
    TEST_ASSERT_INT_EQUAL(50, opt.iters);
    TEST_ASSERT_INT_EQUAL(4, opt.min_bytes);
    TEST_ASSERT_INT_EQUAL(64, opt.max_bytes);
    TEST_ASSERT_INT_EQUAL(2, opt.factor);
}

// Test: size generator produces correct geometric sequence [4, 8, 16, 32, 64]
static void test_sizes_geom(void)
{
    std::vector<int> s;
    TEST_ASSERT_TRUE(make_sizes(4, 64, 2, s));
    TEST_ASSERT_INT_EQUAL(5, (int)s.size());
    TEST_ASSERT_INT_EQUAL(4, s[0]);
    TEST_ASSERT_INT_EQUAL(8, s[1]);
    TEST_ASSERT_INT_EQUAL(16, s[2]);
    TEST_ASSERT_INT_EQUAL(32, s[3]);
    TEST_ASSERT_INT_EQUAL(64, s[4]);
}

int main(void)
{
    UnityBegin("assignment4 helpers");

    RUN_TEST(test_cli_defaults);
    RUN_TEST(test_cli_custom);
    RUN_TEST(test_sizes_geom);

    return UnityEnd();
}
