#pragma once

#include <stdio.h>

static unsigned int failed_tests = 0;
static unsigned int total_tests = 0;

#define TEST(object, func) \
    static void object##_##func(void); \
    static void run_##object##_##func(void) { \
        printf("    [%s: %s]\n", #object, #func); \
        object##_##func(); \
    } \
    static void object##_##func(void)

#define RUN_TEST(object, func) \
    do { \
        total_tests++; \
        run_##object##_##func(); \
    } while (0)

#define EXPECT_EQ(a, b) \
    do { \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        if (_a != _b) { \
            printf( \
                "       FAIL: %s == %s (%lld != %lld) at %s:%d\n", \
                #a, #b, (long long)_a, (long long)_b, __FILE__, __LINE__ \
            ); \
            failed_tests++; \
        } \
    } while (0)

#define EXPECT_SIZE_EQ(a, b) \
    EXPECT_EQ((size_t) (a), (size_t) (b))

#define EXPECT_TRUE(x) \
    do { \
        if (!(x)) { \
            printf("        FAIL: %s is false at %s:%d\n", #x, __FILE__, __LINE__); \
            failed_tests++; \
        } \
    } while (0)

#define EXPECT_FALSE(x) \
    do { \
        if (x) { \
            printf("        FAIL: %s is true at %s:%d\n", #x, __FILE__, __LINE__); \
            failed_tests++; \
        } \
    } while (0)

static inline int test_summary(void) {
    printf("%d tests run, %d failures\n", total_tests, failed_tests);
    return failed_tests != 0;
}

