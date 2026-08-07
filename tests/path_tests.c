#define PATH_IMPL
#include "../src/lib/path.h"
#include "common.h"

TEST(cell, from_row_column) {
    // Arrange
    // Act
    cell c = cell_from_row_col(2, 3, 10);
    // Assert
    EXPECT_SIZE_EQ(c.idx, 23);
}

TEST(cell, from_row_column_zero) {
    // Arrange
    // Act
    cell c = cell_from_row_col(0, 0, 5);
    // Assert
    EXPECT_SIZE_EQ(c.idx, 0);
}

TEST(cell, row_of) {
    // Arrange
    cell c1 = cell_from_row_col(2, 3, 10);
    cell c2 = cell_from_row_col(0, 0, 5);
    // Act
    // Assert
    EXPECT_SIZE_EQ(cell_row(c1, 10), 2);
    EXPECT_SIZE_EQ(cell_row(c2, 5), 0);
}

TEST(cell, col_of) {
    // Arrange
    cell c1 = cell_from_row_col(2, 3, 10);
    cell c2 = cell_from_row_col(2, 0, 10);
    // Act
    // Assert
    EXPECT_SIZE_EQ(cell_col(c1, 10), 3);
    EXPECT_SIZE_EQ(cell_col(c2, 10), 0);
}

TEST(cell, row_of_col_of_all_cells) {
    // Arrange
    // Act
    // Assert
    for (size_t r = 0; r < 5; r++)
        for (size_t c = 0; c < 7; c++) {
            cell cl = cell_from_row_col(r, c, 7);
            EXPECT_EQ(cell_row(cl, 7), r);
            EXPECT_EQ(cell_col(cl, 7), c);
        }
}

TEST(cell, equality) {
    // Arrange
    cell a = cell_from_row_col(1, 2, 10);
    cell b = cell_from_row_col(1, 2, 10);
    cell c = cell_from_row_col(2, 1, 10);
    // Act
    // Assert
    EXPECT_TRUE(cell_eq(a, b));
    EXPECT_FALSE(cell_eq(a, c));
}

TEST(cell, none) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(CELL_NONE.idx, SIZE_MAX);
}

TEST(path, new_small) {
    // Arrange
    // Act
    path* p = path_new(8);
    // Assert
    EXPECT_TRUE(p != NULL);
    EXPECT_SIZE_EQ(p->capacity, 8);
    EXPECT_SIZE_EQ(p->size, 0);
    path_free(p);
}

TEST(path, new_zero) {
    // Arrange
    // Act
    path* p = path_new(0);
    // Assert
    EXPECT_TRUE(p == NULL);
}

TEST(path, new_size_t_overflow) {
    // Arrange
    // Act
    path* p = path_new(SIZE_MAX);
    // Assert
    EXPECT_TRUE(p == NULL);
}

TEST(path, append) {
    // Arrange
    path* p = path_new(3);
    cell c0 = cell_from_row_col(0, 0, 3);
    cell c1 = cell_from_row_col(0, 1, 3);
    cell c2 = cell_from_row_col(0, 2, 3);
    cell c3 = cell_from_row_col(1, 0, 3);
    // Act
    // Assert
    EXPECT_TRUE(path_push(p, c0));
    EXPECT_TRUE(path_push(p, c1));
    EXPECT_TRUE(path_push(p, c2));
    EXPECT_SIZE_EQ(p->size, 3);
    EXPECT_FALSE(path_push(p, c3));
    path_free(p);
}

TEST(path, push_order) {
    // Arrange
    path* p = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(1, 1, 4);
    // Act
    // Assert
    EXPECT_TRUE(path_push(p, c0));
    EXPECT_TRUE(path_push(p, c1));
    EXPECT_TRUE(cell_eq(path_at(p, 0), c0));
    EXPECT_TRUE(cell_eq(path_at(p, 1), c1));
    path_free(p);
}

TEST(path, last) {
    // Arrange
    path* p = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(2, 3, 4);
    // Act
    // Assert
    EXPECT_TRUE(path_push(p, c0));
    EXPECT_TRUE(path_push(p, c1));
    EXPECT_TRUE(cell_eq(path_top(p), c1));
    EXPECT_SIZE_EQ(p->size, 2);
    path_free(p);
}

TEST(path, remove_last) {
    // Arrange
    path* p = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(2, 3, 4);
    EXPECT_TRUE(path_push(p, c0));
    EXPECT_TRUE(path_push(p, c1));
    EXPECT_SIZE_EQ(p->size, 2);
    // Act
    cell popped = path_pop(p);
    // Assert
    EXPECT_TRUE(cell_eq(popped, c1));
    EXPECT_SIZE_EQ(p->size, 1);
    EXPECT_TRUE(cell_eq(path_top(p), c0));
    path_free(p);
}

TEST(path, clear_reset) {
    // Arrange
    path* p = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(1, 1, 4);
    EXPECT_TRUE(path_push(p, c0));
    EXPECT_SIZE_EQ(p->size, 1);
    // Act
    path_clear(p);
    // Assert
    EXPECT_SIZE_EQ(p->size, 0);
    EXPECT_TRUE(path_push(p, c1));
    EXPECT_SIZE_EQ(p->size, 1);
    EXPECT_TRUE(cell_eq(path_top(p), c1));
    path_free(p);
}

TEST(path, copy_into) {
    // Arrange
    path* src = path_new(4);
    path* dst = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(1, 1, 4);
    EXPECT_TRUE(path_push(src, c0));
    EXPECT_TRUE(path_push(src, c1));
    // Act
    bool copied = path_copy(dst, src);
    // Assert
    EXPECT_TRUE(copied);
    EXPECT_SIZE_EQ(dst->size, 2);
    EXPECT_TRUE(cell_eq(path_at(dst, 0), c0));
    EXPECT_TRUE(cell_eq(path_at(dst, 1), c1));
    path_free(src);
    path_free(dst);
}

TEST(path, copy_small_dst) {
    // Arrange
    path* src = path_new(4);
    path* dst = path_new(1);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(1, 1, 4);
    EXPECT_TRUE(path_push(src, c0));
    EXPECT_TRUE(path_push(src, c1));
    // Act
    bool copied = path_copy(dst, src);
    // Assert
    EXPECT_FALSE(copied);
    path_free(src);
    path_free(dst);
}

TEST(path, large) {
    // Arrange
    size_t const n = 1000;
    path* p = path_new(n);
    // Act
    for (size_t i = 0; i < n; i++) {
        cell c = cell_from_row_col(i, i, n);
        EXPECT_TRUE(path_push(p, c));
    }
    // Assert
    EXPECT_EQ(p->size, n);
    cell last = cell_from_row_col(n - 1, n - 1, n);
    EXPECT_TRUE(cell_eq(path_at(p, n - 1), last));
    for (size_t i = 0; i < n; i++) {
        cell c = cell_from_row_col(n - 1 - i, n - 1 - i, n);
        EXPECT_TRUE(cell_eq(path_pop(p), c));
    }
    EXPECT_SIZE_EQ(p->size, 0);
    path_free(p);
}

int main(void) {
    RUN_TEST(cell_from_row_column);
    RUN_TEST(cell_from_row_column_zero);
    RUN_TEST(cell_row_of);
    RUN_TEST(cell_col_of);
    RUN_TEST(cell_row_of_col_of_all_cells);
    RUN_TEST(cell_equality);
    RUN_TEST(cell_none);
    RUN_TEST(path_new_small);
    RUN_TEST(path_new_zero);
    RUN_TEST(path_new_size_t_overflow);
    RUN_TEST(path_append);
    RUN_TEST(path_push_order);
    RUN_TEST(path_last);
    RUN_TEST(path_remove_last);
    RUN_TEST(path_clear_reset);
    RUN_TEST(path_copy_into);
    RUN_TEST(path_copy_small_dst);
    RUN_TEST(path_large);
    return test_summary();
}
