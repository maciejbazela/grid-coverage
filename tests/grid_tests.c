#define CELL_IMPL
#include "../src/lib/cell.h"

#define GRID_IMPL
#include "../src/lib/grid.h"
#include "common.h"

#define EXPECT_CELL_STATE(actual, expected) \
    EXPECT_EQ((cell_state) (actual), (cell_state) (expected))


TEST(grid, new_small) {
    // Arrange
    // Act
    grid* g = grid_new(3, 4);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_SIZE_EQ(g->rows, 3);
    EXPECT_SIZE_EQ(g->cols, 4);
    EXPECT_SIZE_EQ(g->cell_count, 12);
    grid_free(g);
}

TEST(grid, new_empty_cells) {
    // Arrange
    // Act
    grid* g = grid_new(3, 3);
    // Assert
    EXPECT_TRUE(g != NULL);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            EXPECT_CELL_STATE(grid_at(g, r, c), CELL_FREE);
    grid_free(g);
}

TEST(grid, new_with_blocked_no_cells) {
    // Arrange
    // Act
    grid* g = grid_new_with_blocked(3, 3, NULL, 0);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_SIZE_EQ(g->rows, 3);
    EXPECT_SIZE_EQ(g->cols, 3);
    EXPECT_SIZE_EQ(g->cell_count, 9);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            EXPECT_CELL_STATE(grid_at(g, r, c), CELL_FREE);
    grid_free(g);
}

TEST(grid, new_with_blocked_single) {
    // Arrange
    cell blocked[] = { cell_from_row_col(1, 2, 3) };
    // Act
    grid* g = grid_new_with_blocked(3, 3, blocked, 1);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_CELL_STATE(grid_at(g, 1, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_FREE);
    EXPECT_CELL_STATE(grid_at(g, 2, 2), CELL_FREE);
    grid_free(g);
}

TEST(grid, new_with_blocked_multiple) {
    // Arrange
    cell blocked[] = {
        cell_from_row_col(0, 0, 3),
        cell_from_row_col(0, 2, 3),
        cell_from_row_col(2, 0, 3),
        cell_from_row_col(2, 2, 3),
    };
    // Act
    grid* g = grid_new_with_blocked(3, 3, blocked, 4);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 0, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 2, 0), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 2, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_FREE);
    grid_free(g);
}

TEST(grid, new_with_blocked_out_of_bounds_ignored) {
    // Arrange
    cell blocked[] = { cell_from_row_col(0, 0, 3), (cell){ .idx = 25 }, cell_from_row_col(1, 1, 3) };
    // Act
    grid* g = grid_new_with_blocked(3, 3, blocked, 3);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 0, 1), CELL_FREE);
    grid_free(g);
}

TEST(grid, new_with_blocked_zero_dimensions) {
    // Arrange
    cell blocked[] = { (cell){ .idx = 0 } };
    // Act
    grid* g = grid_new_with_blocked(0, 0, blocked, 1);
    // Assert
    EXPECT_TRUE(g == NULL);
}

TEST(grid, free_null) {
    // Arrange
    // Act
    grid_free(NULL);
    // Assert
}

TEST(grid, set_cell) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(grid_set(g, 1, 2));
    EXPECT_CELL_STATE(grid_at(g, 1, 2), CELL_BLOCKED);
    grid_free(g);
}

TEST(grid, set_multiple_cells) {
    // Arrange
    grid* g = grid_new(4, 4);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(grid_set(g, 0, 0));
    EXPECT_TRUE(grid_set(g, 1, 1));
    EXPECT_TRUE(grid_set(g, 2, 2));
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 2, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 0, 1), CELL_FREE);
    EXPECT_CELL_STATE(grid_at(g, 3, 3), CELL_FREE);
    grid_free(g);
}

TEST(grid, set_out_of_bounds_row) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(grid_set(g, 3, 0));
    EXPECT_CELL_STATE(grid_at(g, 3, 0), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, set_out_of_bounds_col) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(grid_set(g, 0, 3));
    EXPECT_CELL_STATE(grid_at(g, 0, 3), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, unset_cell) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 1, 2));
    // Act
    // Assert
    EXPECT_TRUE(grid_unset(g, 1, 2));
    EXPECT_CELL_STATE(grid_at(g, 1, 2), CELL_FREE);
    grid_free(g);
}

TEST(grid, unset_out_of_bounds_row) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(grid_unset(g, 3, 0));
    EXPECT_CELL_STATE(grid_at(g, 3, 0), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, unset_out_of_bounds_col) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(grid_unset(g, 0, 3));
    EXPECT_CELL_STATE(grid_at(g, 0, 3), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, get_unset_cell) {
    // Arrange
    grid* g = grid_new(2, 2);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_FREE);
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_FREE);
    grid_free(g);
}

TEST(grid, overwrite_cell) {
    // Arrange
    grid* g = grid_new(2, 2);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(grid_set(g, 0, 0));
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_BLOCKED);
    EXPECT_TRUE(grid_set(g, 0, 0));
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_BLOCKED);
    grid_free(g);
}

TEST(grid, zero_dimensions) {
    // Arrange
    // Act
    grid* g = grid_new(0, 0);
    // Assert
    EXPECT_TRUE(g == NULL);
}

TEST(grid, new_overflow) {
    // Arrange
    // Act
    grid* g = grid_new(SIZE_MAX, 1);
    // Assert
    EXPECT_TRUE(g == NULL);
}

TEST(grid, large_grid) {
    // Arrange
    // Act
    grid* g = grid_new(100, 100);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 99, 99));
    EXPECT_CELL_STATE(grid_at(g, 99, 99), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 98, 99), CELL_FREE);
    grid_free(g);
}

TEST(grid, non_square_stride) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_CELL_STATE(grid_at(g, 0, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 1, 0), CELL_FREE);
    grid_free(g);
}

TEST(grid, non_square_stride_corner) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(grid_set(g, 1, 2));
    EXPECT_CELL_STATE(grid_at(g, 1, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_FREE);
    EXPECT_CELL_STATE(grid_at(g, 0, 2), CELL_FREE);
    grid_free(g);
}

TEST(grid, flat_index) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 2));
    // Act
    // Assert
    EXPECT_CELL_STATE(grid_at_flat(g, 2), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at_flat(g, 0), CELL_FREE);
    grid_free(g);
}

TEST(grid, flat_index_out_of_bounds) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_CELL_STATE(grid_at_flat(g, 6), CELL_OUT_OF_BOUNDS);
    EXPECT_CELL_STATE(grid_at_flat(g, 1000), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, flat_matches) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 1, 0));
    EXPECT_TRUE(grid_set(g, 1, 1));
    // Act
    // Assert
    for (size_t r = 0; r < 2; ++r)
        for (size_t c = 0; c < 3; ++c)
            EXPECT_CELL_STATE(grid_at_flat(g, r * 3 + c), grid_at(g, r, c));
    grid_free(g);
}

TEST(grid, states_distinct) {
    // Arrange
    grid* g = grid_new(2, 2);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 1, 1));
    // Act
    // Assert
    EXPECT_CELL_STATE(grid_at(g, 1, 1), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at(g, 0, 0), CELL_FREE);
    EXPECT_CELL_STATE(grid_at(g, 2, 0), CELL_OUT_OF_BOUNDS);
    EXPECT_CELL_STATE(grid_at(g, 0, 2), CELL_OUT_OF_BOUNDS);
    EXPECT_CELL_STATE(grid_at_flat(g, 3), CELL_BLOCKED);
    EXPECT_CELL_STATE(grid_at_flat(g, 0), CELL_FREE);
    EXPECT_CELL_STATE(grid_at_flat(g, 4), CELL_OUT_OF_BOUNDS);
    grid_free(g);
}

TEST(grid, print_smoke_test) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 1, 1));
    // Act
    grid_print(g);
    // Assert
    grid_free(g);
}

int main(void) {
    RUN_TEST(grid, new_small);
    RUN_TEST(grid, new_empty_cells);
    RUN_TEST(grid, new_with_blocked_no_cells);
    RUN_TEST(grid, new_with_blocked_single);
    RUN_TEST(grid, new_with_blocked_multiple);
    RUN_TEST(grid, new_with_blocked_out_of_bounds_ignored);
    RUN_TEST(grid, new_with_blocked_zero_dimensions);
    RUN_TEST(grid, free_null);
    RUN_TEST(grid, set_cell);
    RUN_TEST(grid, set_multiple_cells);
    RUN_TEST(grid, set_out_of_bounds_row);
    RUN_TEST(grid, set_out_of_bounds_col);
    RUN_TEST(grid, unset_cell);
    RUN_TEST(grid, unset_out_of_bounds_row);
    RUN_TEST(grid, unset_out_of_bounds_col);
    RUN_TEST(grid, get_unset_cell);
    RUN_TEST(grid, overwrite_cell);
    RUN_TEST(grid, zero_dimensions);
    RUN_TEST(grid, new_overflow);
    RUN_TEST(grid, large_grid);
    RUN_TEST(grid, non_square_stride);
    RUN_TEST(grid, non_square_stride_corner);
    RUN_TEST(grid, flat_index);
    RUN_TEST(grid, flat_index_out_of_bounds);
    RUN_TEST(grid, flat_matches);
    RUN_TEST(grid, states_distinct);
    RUN_TEST(grid, print_smoke_test);
    return test_summary();
}
