#include <stdlib.h>
#include <string.h>

#define STRING_VIEW_IMPL
#include "../src/lib/string_view.h"
#include "common.h"

TEST(string_view, from_cstr) {
    // Arrange
    // Act
    string_view sv = sv_from_cstr("hello");
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, left_trim) {
    // Arrange
    string_view sv = {8, "   hello"};
    // Act
    sv = sv_left_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, left_trim_no_whitespace) {
    // Arrange
    string_view sv = {5, "hello"};
    // Act
    sv = sv_left_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, left_trim_all_whitespace) {
    // Arrange
    string_view sv = {3, "   "};
    // Act
    sv = sv_left_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 0);
}

TEST(string_view, right_trim) {
    // Arrange
    string_view sv = {8, "hello   "};
    // Act
    sv = sv_right_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, right_trim_no_whitespace) {
    // Arrange
    string_view sv = {5, "hello"};
    // Act
    sv = sv_right_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, right_trim_all_whitespace) {
    // Arrange
    string_view sv = {3, "   "};
    // Act
    sv = sv_right_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 0);
}

TEST(string_view, trim) {
    // Arrange
    string_view sv = {9, "  hello  "};
    // Act
    sv = sv_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 5);
    EXPECT_EQ(sv.data[0], 'h');
    EXPECT_EQ(sv.data[1], 'e');
    EXPECT_EQ(sv.data[2], 'l');
    EXPECT_EQ(sv.data[3], 'l');
    EXPECT_EQ(sv.data[4], 'o');
}

TEST(string_view, trim_empty) {
    // Arrange
    string_view sv = {0, ""};
    // Act
    sv = sv_trim(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 0);
}

TEST(string_view, get_word) {
    // Arrange
    string_view sv = {11, "hello world"};
    // Act
    string_view word = sv_get_word(sv);
    // Assert
    EXPECT_EQ(word.size, (size_t) 5);
    EXPECT_EQ(word.data[0], 'h');
    EXPECT_EQ(word.data[1], 'e');
    EXPECT_EQ(word.data[2], 'l');
    EXPECT_EQ(word.data[3], 'l');
    EXPECT_EQ(word.data[4], 'o');
}

TEST(string_view, get_word_single) {
    // Arrange
    string_view sv = {5, "hello"};
    // Act
    string_view word = sv_get_word(sv);
    // Assert
    EXPECT_EQ(word.size, (size_t) 5);
    EXPECT_EQ(word.data[0], 'h');
    EXPECT_EQ(word.data[1], 'e');
    EXPECT_EQ(word.data[2], 'l');
    EXPECT_EQ(word.data[3], 'l');
    EXPECT_EQ(word.data[4], 'o');
}

TEST(string_view, get_word_empty) {
    // Arrange
    string_view sv = {0, ""};
    // Act
    string_view word = sv_get_word(sv);
    // Assert
    EXPECT_EQ(word.size, (size_t) 0);
}

TEST(string_view, get_word_whitespace) {
    // Arrange
    string_view sv = {3, "   "};
    // Act
    string_view word = sv_get_word(sv);
    // Assert
    EXPECT_EQ(word.size, (size_t) 0);
}

TEST(string_view, skip_word) {
    // Arrange
    string_view sv = {11, "hello world"};
    // Act
    sv = sv_skip_word(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 6);
    EXPECT_EQ(sv.data[0], ' ');
    EXPECT_EQ(sv.data[1], 'w');
    EXPECT_EQ(sv.data[2], 'o');
    EXPECT_EQ(sv.data[3], 'r');
    EXPECT_EQ(sv.data[4], 'l');
    EXPECT_EQ(sv.data[5], 'd');
}

TEST(string_view, skip_word_single) {
    // Arrange
    string_view sv = {5, "hello"};
    // Act
    sv = sv_skip_word(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 0);
}

TEST(string_view, skip_word_empty) {
    // Arrange
    string_view sv = {0, ""};
    // Act
    sv = sv_skip_word(sv);
    // Assert
    EXPECT_EQ(sv.size, (size_t) 0);
}

TEST(string_view, copy_cstr) {
    // Arrange
    char buffer[16];
    string_view sv = {5, "hello"};
    // Act
    bool result = sv_copy_cstr(buffer, sv, sizeof(buffer));
    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(strncmp(buffer, "hello", 5), 0);
}

TEST(string_view, copy_cstr_exact_size) {
    // Arrange
    char buffer[6];
    string_view sv = {5, "hello"};
    // Act
    bool result = sv_copy_cstr(buffer, sv, sizeof(buffer));
    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(strncmp(buffer, "hello", 5), 0);
}

TEST(string_view, copy_cstr_too_small) {
    // Arrange
    char buffer[5];
    string_view sv = {5, "hello"};
    // Act
    bool result = sv_copy_cstr(buffer, sv, sizeof(buffer));
    // Assert
    EXPECT_FALSE(result);
}

TEST(string_view, copy_cstr_empty) {
    // Arrange
    char buffer[1];
    string_view sv = {0, ""};
    // Act
    bool result = sv_copy_cstr(buffer, sv, sizeof(buffer));
    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(buffer[0], '\0');
}

int main(void) {
    RUN_TEST(string_view_from_cstr);
    RUN_TEST(string_view_left_trim);
    RUN_TEST(string_view_left_trim_no_whitespace);
    RUN_TEST(string_view_left_trim_all_whitespace);
    RUN_TEST(string_view_right_trim);
    RUN_TEST(string_view_right_trim_no_whitespace);
    RUN_TEST(string_view_right_trim_all_whitespace);
    RUN_TEST(string_view_trim);
    RUN_TEST(string_view_trim_empty);
    RUN_TEST(string_view_get_word);
    RUN_TEST(string_view_get_word_single);
    RUN_TEST(string_view_get_word_empty);
    RUN_TEST(string_view_get_word_whitespace);
    RUN_TEST(string_view_skip_word);
    RUN_TEST(string_view_skip_word_single);
    RUN_TEST(string_view_skip_word_empty);
    RUN_TEST(string_view_copy_cstr);
    RUN_TEST(string_view_copy_cstr_exact_size);
    RUN_TEST(string_view_copy_cstr_too_small);
    RUN_TEST(string_view_copy_cstr_empty);
    return test_summary();
}

