#define CELL_IMPL
#include "../src/lib/cell.h"

#define GRID_IMPL
#include "../src/lib/grid.h"

#define PATH_IMPL
#include "../src/lib/path.h"
#include "common.h"

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

TEST(path, free_null) {
    // Arrange
    // Act
    path_free(NULL);
    // Assert
}

TEST(path, push_works) {
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

TEST(path, pop_works) {
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

TEST(path, pop_on_empty) {
    // Arrange
    path* p = path_new(1);
    EXPECT_SIZE_EQ(p->size, 0);
    // Act
    cell popped = path_pop(p);
    // Assert
    EXPECT_TRUE(cell_eq(popped, CELL_NONE));
    EXPECT_SIZE_EQ(p->size, 0);
    path_free(p);
}

TEST(path, top_works) {
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

TEST(path, top_on_empty) {
    // Arrange
    path* p = path_new(1);
    // Act
    // Assert
    EXPECT_TRUE(cell_eq(path_top(p), CELL_NONE));
    EXPECT_SIZE_EQ(p->size, 0);
    path_free(p);
}

TEST(path, at_exceeding_size) {
    // Arrange
    path* p = path_new(1);
    // Act
    // Assert
    EXPECT_TRUE(cell_eq(path_at(p, 0), CELL_NONE));
    EXPECT_TRUE(cell_eq(path_at(p, 1), CELL_NONE));
    EXPECT_TRUE(cell_eq(path_at(p, 10000), CELL_NONE));
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

TEST(path, empty_and_full) {
    // Arrange
    path* p = path_new(2);
    cell c0 = cell_from_row_col(0, 0, 4);
    cell c1 = cell_from_row_col(1, 1, 4);
    // Act
    // Assert
    EXPECT_TRUE(path_empty(p));
    EXPECT_FALSE(path_full(p));
    path_push(p, c0);
    EXPECT_FALSE(path_empty(p));
    EXPECT_FALSE(path_full(p));
    path_push(p, c1);
    EXPECT_FALSE(path_empty(p));
    EXPECT_TRUE(path_full(p));
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
    for (size_t i = 0; i < n; ++i) {
        cell c = cell_from_row_col(i, i, n);
        EXPECT_TRUE(path_push(p, c));
    }
    // Assert
    EXPECT_EQ(p->size, n);
    cell last = cell_from_row_col(n - 1, n - 1, n);
    EXPECT_TRUE(cell_eq(path_at(p, n - 1), last));
    for (size_t i = 0; i < n; ++i) {
        cell c = cell_from_row_col(n - 1 - i, n - 1 - i, n);
        EXPECT_TRUE(cell_eq(path_pop(p), c));
    }
    EXPECT_SIZE_EQ(p->size, 0);
    path_free(p);
}

TEST(path, print_smoke_test) {
    // Arrange
    path* p = path_new(12);
    EXPECT_TRUE(p != NULL);
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 0, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 1, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(1, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 3, 4)));
    // Act
    path_print(p, 4);
    // Assert
    path_free(p);
}

TEST(path, print_smoke_test_with_long_cols) {
    // Arrange
    path* p = path_new(32);
    EXPECT_TRUE(p != NULL);
    for (size_t i = 0; i < 32; i++) {
        EXPECT_TRUE(path_push(p, cell_from_row_col(0, i, 32)));
    }
    // Act
    path_print(p, 32);
    // Assert
    path_free(p);
}


TEST(path, print_on_grid_smoke_test) {
    // Arrange
    grid* g = grid_new(3, 4);
    path* p = path_new(12);
    EXPECT_TRUE(g != NULL && p != NULL);
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 0, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 1, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(1, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 3, 4)));
    // Act
    path_print_on_grid(p, g);
    // Assert
    grid_free(g);
    path_free(p);
}

TEST(path, print_on_grid_smoke_test_with_invalid) {
    // Arrange
    grid* g = grid_new(4, 4);
    path* p = path_new(12);
    EXPECT_TRUE(g != NULL && p != NULL);
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 0, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 1, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(1, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(3, 3, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 3, 4)));
    // Act
    path_print_on_grid(p, g);
    // Assert
    grid_free(g);
    path_free(p);
}

TEST(path, print_on_grid_smoke_test_with_backtrack) {
    // Arrange
    grid* g = grid_new(3, 4);
    path* p = path_new(12);
    EXPECT_TRUE(g != NULL && p != NULL);
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 0, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 1, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(1, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(0, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(1, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 2, 4)));
    EXPECT_TRUE(path_push(p, cell_from_row_col(2, 3, 4)));
    // Act
    path_print_on_grid(p, g);
    // Assert
    grid_free(g);
    path_free(p);
}

TEST(path, push_idx_works) {
    // Arrange
    path* p = path_new(3);
    // Act
    bool ok = path_push_idx(p, 42);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_SIZE_EQ(p->size, 1);
    EXPECT_TRUE(cell_eq(path_top(p), (cell){ .idx = 42 }));
    path_free(p);
}

TEST(path, push_idx_full) {
    // Arrange
    path* p = path_new(1);
    path_push_idx(p, 0);
    // Act
    bool ok = path_push_idx(p, 1);
    // Assert
    EXPECT_FALSE(ok);
    EXPECT_SIZE_EQ(p->size, 1);
    path_free(p);
}

TEST(path, push_idx_and_push_consistent) {
    // Arrange
    path* p = path_new(4);
    // Act
    path_push_idx(p, 10);
    path_push(p, (cell){ .idx = 20 });
    path_push_idx(p, 30);
    // Assert
    EXPECT_SIZE_EQ(p->size, 3);
    EXPECT_TRUE(cell_eq(path_at(p, 0), (cell){ .idx = 10 }));
    EXPECT_TRUE(cell_eq(path_at(p, 1), (cell){ .idx = 20 }));
    EXPECT_TRUE(cell_eq(path_at(p, 2), (cell){ .idx = 30 }));
    path_free(p);
}

int main(void) {
    RUN_TEST(path, new_small);
    RUN_TEST(path, new_zero);
    RUN_TEST(path, new_size_t_overflow);
    RUN_TEST(path, free_null);
    RUN_TEST(path, push_works);
    RUN_TEST(path, push_order);
    RUN_TEST(path, pop_works);
    RUN_TEST(path, pop_on_empty);
    RUN_TEST(path, top_works);
    RUN_TEST(path, top_on_empty);
    RUN_TEST(path, at_exceeding_size);
    RUN_TEST(path, clear_reset);
    RUN_TEST(path, empty_and_full);
    RUN_TEST(path, copy_into);
    RUN_TEST(path, copy_small_dst);
    RUN_TEST(path, large);
    RUN_TEST(path, push_idx_works);
    RUN_TEST(path, push_idx_full);
    RUN_TEST(path, push_idx_and_push_consistent);
    RUN_TEST(path, print_smoke_test);
    RUN_TEST(path, print_smoke_test_with_long_cols);
    RUN_TEST(path, print_on_grid_smoke_test);
    RUN_TEST(path, print_on_grid_smoke_test_with_invalid);
    RUN_TEST(path, print_on_grid_smoke_test_with_backtrack);
    return test_summary();
}
