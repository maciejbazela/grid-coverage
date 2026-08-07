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
    for (size_t r = 0; r < 3; r++)
        for (size_t c = 0; c < 3; c++)
            EXPECT_CELL_STATE(grid_at(g, r, c), CELL_FREE);
    grid_free(g);
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
    for (size_t r = 0; r < 2; r++)
        for (size_t c = 0; c < 3; c++)
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

TEST(grid, print_smoke) {
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
    RUN_TEST(grid_new_small);
    RUN_TEST(grid_new_empty_cells);
    RUN_TEST(grid_set_cell);
    RUN_TEST(grid_set_multiple_cells);
    RUN_TEST(grid_set_out_of_bounds_row);
    RUN_TEST(grid_set_out_of_bounds_col);
    RUN_TEST(grid_get_unset_cell);
    RUN_TEST(grid_overwrite_cell);
    RUN_TEST(grid_zero_dimensions);
    RUN_TEST(grid_large_grid);
    RUN_TEST(grid_non_square_stride);
    RUN_TEST(grid_non_square_stride_corner);
    RUN_TEST(grid_flat_index);
    RUN_TEST(grid_flat_index_out_of_bounds);
    RUN_TEST(grid_flat_matches);
    RUN_TEST(grid_states_distinct);
    RUN_TEST(grid_print_smoke);
    return test_summary();
}
