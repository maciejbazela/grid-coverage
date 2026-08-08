#define QUEUE_IMPL
#include "../src/lib/queue.h"
#include "common.h"

TEST(queue, new_small) {
    // Arrange
    // Act
    queue* q = queue_new(8);
    // Assert
    EXPECT_TRUE(q != NULL);
    EXPECT_SIZE_EQ(q->capacity, 8);
    EXPECT_SIZE_EQ(q->size, 0);
    EXPECT_SIZE_EQ(q->head, 0);
    EXPECT_SIZE_EQ(q->tail, 0);
    queue_free(q);
}

TEST(queue, new_zero) {
    // Arrange
    // Act
    queue* q = queue_new(0);
    // Assert
    EXPECT_TRUE(q == NULL);
}

TEST(queue, free_null) {
    // Arrange
    // Act
    queue_free(NULL);
    // Assert
}

TEST(queue, push_works) {
    // Arrange
    queue* q = queue_new(3);
    // Act
    // Assert
    EXPECT_TRUE(queue_push(q, 10));
    EXPECT_TRUE(queue_push(q, 20));
    EXPECT_TRUE(queue_push(q, 30));
    EXPECT_SIZE_EQ(q->size, 3);
    EXPECT_FALSE(queue_push(q, 40));
    queue_free(q);
}

TEST(queue, push_order) {
    // Arrange
    queue* q = queue_new(4);
    // Act
    // Assert
    EXPECT_TRUE(queue_push(q, 100));
    EXPECT_TRUE(queue_push(q, 200));
    EXPECT_SIZE_EQ(queue_pop(q), 100);
    EXPECT_SIZE_EQ(queue_pop(q), 200);
    queue_free(q);
}

TEST(queue, pop_works) {
    // Arrange
    queue* q = queue_new(4);
    queue_push(q, 10);
    queue_push(q, 20);
    EXPECT_SIZE_EQ(q->size, 2);
    // Act
    size_t popped = queue_pop(q);
    // Assert
    EXPECT_SIZE_EQ(popped, 10);
    EXPECT_SIZE_EQ(q->size, 1);
    EXPECT_SIZE_EQ(queue_front(q), 20);
    queue_free(q);
}

TEST(queue, pop_on_empty) {
    // Arrange
    queue* q = queue_new(1);
    EXPECT_SIZE_EQ(q->size, 0);
    // Act
    size_t popped = queue_pop(q);
    // Assert
    EXPECT_SIZE_EQ(popped, QUEUE_NONE);
    EXPECT_SIZE_EQ(q->size, 0);
    queue_free(q);
}

TEST(queue, front_works) {
    // Arrange
    queue* q = queue_new(4);
    queue_push(q, 10);
    queue_push(q, 20);
    // Act
    // Assert
    EXPECT_SIZE_EQ(queue_front(q), 10);
    EXPECT_SIZE_EQ(q->size, 2);
    queue_free(q);
}

TEST(queue, front_on_empty) {
    // Arrange
    queue* q = queue_new(1);
    // Act
    // Assert
    EXPECT_SIZE_EQ(queue_front(q), QUEUE_NONE);
    EXPECT_SIZE_EQ(q->size, 0);
    queue_free(q);
}

TEST(queue, clear_reset) {
    // Arrange
    queue* q = queue_new(4);
    queue_push(q, 10);
    queue_push(q, 20);
    queue_pop(q);
    EXPECT_SIZE_EQ(q->size, 1);
    // Act
    queue_clear(q);
    // Assert
    EXPECT_SIZE_EQ(q->size, 0);
    EXPECT_SIZE_EQ(q->head, 0);
    EXPECT_SIZE_EQ(q->tail, 0);
    EXPECT_TRUE(queue_push(q, 30));
    EXPECT_SIZE_EQ(q->size, 1);
    EXPECT_SIZE_EQ(queue_front(q), 30);
    queue_free(q);
}

TEST(queue, empty_and_full) {
    // Arrange
    queue* q = queue_new(2);
    // Act
    // Assert
    EXPECT_TRUE(queue_empty(q));
    EXPECT_FALSE(queue_full(q));
    queue_push(q, 10);
    EXPECT_FALSE(queue_empty(q));
    EXPECT_FALSE(queue_full(q));
    queue_push(q, 20);
    EXPECT_FALSE(queue_empty(q));
    EXPECT_TRUE(queue_full(q));
    queue_free(q);
}

TEST(queue, wrap_around) {
    // Arrange
    queue* q = queue_new(3);
    queue_push(q, 10);
    queue_push(q, 20);
    queue_push(q, 30);
    EXPECT_SIZE_EQ(queue_pop(q), 10);
    EXPECT_SIZE_EQ(queue_pop(q), 20);
    // Act
    EXPECT_TRUE(queue_push(q, 40));
    EXPECT_TRUE(queue_push(q, 50));
    EXPECT_FALSE(queue_push(q, 60));
    // Assert
    EXPECT_SIZE_EQ(queue_pop(q), 30);
    EXPECT_SIZE_EQ(queue_pop(q), 40);
    EXPECT_SIZE_EQ(queue_pop(q), 50);
    EXPECT_TRUE(queue_empty(q));
    queue_free(q);
}

TEST(queue, large) {
    // Arrange
    size_t const n = 1000;
    queue* q = queue_new(n);
    // Act
    for (size_t i = 0; i < n; i++)
        EXPECT_TRUE(queue_push(q, i));
    // Assert
    EXPECT_SIZE_EQ(q->size, n);
    for (size_t i = 0; i < n; i++)
        EXPECT_SIZE_EQ(queue_pop(q), i);
    EXPECT_SIZE_EQ(q->size, 0);
    queue_free(q);
}

int main(void) {
    RUN_TEST(queue, new_small);
    RUN_TEST(queue, new_zero);
    RUN_TEST(queue, free_null);
    RUN_TEST(queue, push_works);
    RUN_TEST(queue, push_order);
    RUN_TEST(queue, pop_works);
    RUN_TEST(queue, pop_on_empty);
    RUN_TEST(queue, front_works);
    RUN_TEST(queue, front_on_empty);
    RUN_TEST(queue, clear_reset);
    RUN_TEST(queue, empty_and_full);
    RUN_TEST(queue, wrap_around);
    RUN_TEST(queue, large);
    return test_summary();
}
