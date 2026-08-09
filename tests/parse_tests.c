#define PARSE_IMPL
#include "../src/lib/parse.h"
#include "common.h"

TEST(parse_size, valid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("5", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_SIZE_EQ(out, 5);
}

TEST(parse_size, valid_large) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("123456789", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_SIZE_EQ(out, 123456789);
}

TEST(parse_size, valid_leading_whitespace) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("  5", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_SIZE_EQ(out, 5);
}

TEST(parse_size, zero_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("0", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_size, negative_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("-3", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_size, empty_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_size, non_numeric_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("abc", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_size, trailing_garbage_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("5abc", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_size, overflow_invalid) {
    // Arrange
    size_t out = 0;
    // Act
    bool ok = parse_size("999999999999999999999999", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_int, valid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("5", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(out, 5);
}

TEST(parse_int, valid_negative) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("-3", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(out, -3);
}

TEST(parse_int, valid_zero) {
    // Arrange
    int out = -1;
    // Act
    bool ok = parse_int("0", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(out, 0);
}

TEST(parse_int, valid_max) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("2147483647", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(out, 2147483647);
}

TEST(parse_int, overflow_invalid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("2147483648", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_int, underflow_invalid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("-2147483649", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_int, empty_invalid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_int, non_numeric_invalid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("abc", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_int, trailing_garbage_invalid) {
    // Arrange
    int out = 0;
    // Act
    bool ok = parse_int("5abc", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_double, valid_fraction) {
    // Arrange
    double out = 0.0;
    // Act
    bool ok = parse_double("0.5", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_TRUE(out > 0.499999 && out < 0.500001);
}

TEST(parse_double, valid_negative) {
    // Arrange
    double out = 0.0;
    // Act
    bool ok = parse_double("-2.5", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_TRUE(out > -2.500001 && out < -2.499999);
}

TEST(parse_double, valid_exponent) {
    // Arrange
    double out = 0.0;
    // Act
    bool ok = parse_double("1e3", &out);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_TRUE(out > 999.999 && out < 1000.001);
}

TEST(parse_double, empty_invalid) {
    // Arrange
    double out = 0.0;
    // Act
    bool ok = parse_double("", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_double, non_numeric_invalid) {
    // Arrange
    double out = 0.0;
    // Act
    bool ok = parse_double("abc", &out);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_pair, valid_simple) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("1 2", &a, &b);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
}

TEST(parse_pair, valid_surrounding_whitespace) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("  3   4  ", &a, &b);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 4);
}

TEST(parse_pair, valid_negative) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("-1 -2", &a, &b);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(a, -1);
    EXPECT_EQ(b, -2);
}

TEST(parse_pair, single_value_invalid) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("1", &a, &b);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_pair, empty_invalid) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("", &a, &b);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_pair, non_numeric_invalid) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("abc def", &a, &b);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_pair, second_non_numeric_invalid) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("1 def", &a, &b);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(parse_pair, trailing_garbage_invalid) {
    // Arrange
    int a = 0, b = 0;
    // Act
    bool ok = parse_pair("1 2 3", &a, &b);
    // Assert
    EXPECT_FALSE(ok);
}

int main(void) {
    RUN_TEST(parse_size, valid);
    RUN_TEST(parse_size, valid_large);
    RUN_TEST(parse_size, valid_leading_whitespace);
    RUN_TEST(parse_size, zero_invalid);
    RUN_TEST(parse_size, negative_invalid);
    RUN_TEST(parse_size, empty_invalid);
    RUN_TEST(parse_size, non_numeric_invalid);
    RUN_TEST(parse_size, trailing_garbage_invalid);
    RUN_TEST(parse_size, overflow_invalid);
    RUN_TEST(parse_int, valid);
    RUN_TEST(parse_int, valid_negative);
    RUN_TEST(parse_int, valid_zero);
    RUN_TEST(parse_int, valid_max);
    RUN_TEST(parse_int, overflow_invalid);
    RUN_TEST(parse_int, underflow_invalid);
    RUN_TEST(parse_int, empty_invalid);
    RUN_TEST(parse_int, non_numeric_invalid);
    RUN_TEST(parse_int, trailing_garbage_invalid);
    RUN_TEST(parse_double, valid_fraction);
    RUN_TEST(parse_double, valid_negative);
    RUN_TEST(parse_double, valid_exponent);
    RUN_TEST(parse_double, empty_invalid);
    RUN_TEST(parse_double, non_numeric_invalid);
    RUN_TEST(parse_pair, valid_simple);
    RUN_TEST(parse_pair, valid_surrounding_whitespace);
    RUN_TEST(parse_pair, valid_negative);
    RUN_TEST(parse_pair, single_value_invalid);
    RUN_TEST(parse_pair, empty_invalid);
    RUN_TEST(parse_pair, non_numeric_invalid);
    RUN_TEST(parse_pair, second_non_numeric_invalid);
    RUN_TEST(parse_pair, trailing_garbage_invalid);
    return test_summary();
}
