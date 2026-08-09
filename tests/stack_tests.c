#define STACK_IMPL
#include "../src/lib/stack.h"
#include "common.h"

TEST(stack, new_small) {
    // Arrange
    // Act
    stack* s = stack_new(8);
    // Assert
    EXPECT_TRUE(s != NULL);
    EXPECT_SIZE_EQ(s->capacity, 8);
    EXPECT_SIZE_EQ(s->size, 0);
    stack_free(s);
}

TEST(stack, new_zero) {
    // Arrange
    // Act
    stack* s = stack_new(0);
    // Assert
    EXPECT_TRUE(s == NULL);
}

TEST(stack, new_overflow) {
    // Arrange
    // Act
    stack* s = stack_new(SIZE_MAX);
    // Assert
    EXPECT_TRUE(s == NULL);
}

TEST(stack, free_null) {
    // Arrange
    // Act
    stack_free(NULL);
    // Assert
}

TEST(stack, push_works) {
    // Arrange
    stack* s = stack_new(3);
    // Act
    // Assert
    EXPECT_TRUE(stack_push(s, 10));
    EXPECT_TRUE(stack_push(s, 20));
    EXPECT_TRUE(stack_push(s, 30));
    EXPECT_SIZE_EQ(s->size, 3);
    EXPECT_FALSE(stack_push(s, 40));
    stack_free(s);
}

TEST(stack, push_order) {
    // Arrange
    stack* s = stack_new(4);
    // Act
    // Assert
    EXPECT_TRUE(stack_push(s, 100));
    EXPECT_TRUE(stack_push(s, 200));
    EXPECT_SIZE_EQ(stack_pop(s), 200);
    EXPECT_SIZE_EQ(stack_pop(s), 100);
    stack_free(s);
}

TEST(stack, pop_works) {
    // Arrange
    stack* s = stack_new(4);
    stack_push(s, 10);
    stack_push(s, 20);
    EXPECT_SIZE_EQ(s->size, 2);
    // Act
    size_t popped = stack_pop(s);
    // Assert
    EXPECT_SIZE_EQ(popped, 20);
    EXPECT_SIZE_EQ(s->size, 1);
    EXPECT_SIZE_EQ(stack_top(s), 10);
    stack_free(s);
}

TEST(stack, pop_on_empty) {
    // Arrange
    stack* s = stack_new(1);
    EXPECT_SIZE_EQ(s->size, 0);
    // Act
    size_t popped = stack_pop(s);
    // Assert
    EXPECT_SIZE_EQ(popped, STACK_NONE);
    EXPECT_SIZE_EQ(s->size, 0);
    stack_free(s);
}

TEST(stack, top_works) {
    // Arrange
    stack* s = stack_new(4);
    stack_push(s, 10);
    stack_push(s, 20);
    // Act
    // Assert
    EXPECT_SIZE_EQ(stack_top(s), 20);
    EXPECT_SIZE_EQ(s->size, 2);
    stack_free(s);
}

TEST(stack, top_on_empty) {
    // Arrange
    stack* s = stack_new(1);
    // Act
    // Assert
    EXPECT_SIZE_EQ(stack_top(s), STACK_NONE);
    EXPECT_SIZE_EQ(s->size, 0);
    stack_free(s);
}

TEST(stack, clear_reset) {
    // Arrange
    stack* s = stack_new(4);
    stack_push(s, 10);
    EXPECT_SIZE_EQ(s->size, 1);
    // Act
    stack_clear(s);
    // Assert
    EXPECT_SIZE_EQ(s->size, 0);
    EXPECT_TRUE(stack_push(s, 20));
    EXPECT_SIZE_EQ(s->size, 1);
    EXPECT_SIZE_EQ(stack_top(s), 20);
    stack_free(s);
}

TEST(stack, empty_and_full) {
    // Arrange
    stack* s = stack_new(2);
    // Act
    // Assert
    EXPECT_TRUE(stack_empty(s));
    EXPECT_FALSE(stack_full(s));
    stack_push(s, 10);
    EXPECT_FALSE(stack_empty(s));
    EXPECT_FALSE(stack_full(s));
    stack_push(s, 20);
    EXPECT_FALSE(stack_empty(s));
    EXPECT_TRUE(stack_full(s));
    stack_free(s);
}

TEST(stack, large) {
    // Arrange
    size_t const n = 1000;
    stack* s = stack_new(n);
    // Act
    for (size_t i = 0; i < n; ++i)
        EXPECT_TRUE(stack_push(s, i));
    // Assert
    EXPECT_SIZE_EQ(s->size, n);
    for (size_t i = 0; i < n; ++i)
        EXPECT_SIZE_EQ(stack_pop(s), n - 1 - i);
    EXPECT_SIZE_EQ(s->size, 0);
    stack_free(s);
}

int main(void) {
    RUN_TEST(stack, new_small);
    RUN_TEST(stack, new_zero);
    RUN_TEST(stack, new_overflow);
    RUN_TEST(stack, push_works);
    RUN_TEST(stack, push_order);
    RUN_TEST(stack, pop_works);
    RUN_TEST(stack, pop_on_empty);
    RUN_TEST(stack, top_works);
    RUN_TEST(stack, top_on_empty);
    RUN_TEST(stack, clear_reset);
    RUN_TEST(stack, empty_and_full);
    RUN_TEST(stack, large);
    RUN_TEST(stack, free_null);
    return test_summary();
}
