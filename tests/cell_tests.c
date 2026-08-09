#define CELL_IMPL
#include "../src/lib/cell.h"
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
    for (size_t r = 0; r < 5; ++r)
        for (size_t c = 0; c < 7; ++c) {
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

TEST(cell, neighbour_of) {
    // Arrange
    cell center = cell_from_row_col(1, 1, 3);
    // Act
    cell north_neighbour = cell_neighbour(center, DIRECTION_NORTH, 3, 9);
    cell east_neighbour  = cell_neighbour(center, DIRECTION_EAST, 3, 9);
    cell south_neighbour = cell_neighbour(center, DIRECTION_SOUTH, 3, 9);
    cell west_neighbour  = cell_neighbour(center, DIRECTION_WEST, 3, 9);
    // Assert
    EXPECT_TRUE(cell_eq(north_neighbour, cell_from_row_col(0, 1, 3)));
    EXPECT_TRUE(cell_eq(east_neighbour,  cell_from_row_col(1, 2, 3)));
    EXPECT_TRUE(cell_eq(south_neighbour, cell_from_row_col(2, 1, 3)));
    EXPECT_TRUE(cell_eq(west_neighbour,  cell_from_row_col(1, 0, 3)));
}

TEST(cell, neighbour_out_of_bounds) {
    // Arrange
    cell top_left  = cell_from_row_col(0, 0, 3);
    cell past_end = (cell){ .idx = 9 };
    // Act
    cell top_left_north = cell_neighbour(top_left, DIRECTION_NORTH, 3, 9);
    cell top_left_west  = cell_neighbour(top_left, DIRECTION_WEST, 3, 9);
    cell top_left_south = cell_neighbour(top_left, DIRECTION_SOUTH, 3, 9);
    cell top_left_east  = cell_neighbour(top_left, DIRECTION_EAST, 3, 9);
    cell past_end_north = cell_neighbour(past_end, DIRECTION_NORTH, 3, 9);
    cell past_end_east  = cell_neighbour(past_end, DIRECTION_EAST, 3, 9);
    cell past_end_south = cell_neighbour(past_end, DIRECTION_SOUTH, 3, 9);
    cell past_end_west  = cell_neighbour(past_end, DIRECTION_WEST, 3, 9);
    // Assert
    EXPECT_TRUE(cell_eq(top_left_north, CELL_NONE));
    EXPECT_TRUE(cell_eq(top_left_west, CELL_NONE));
    EXPECT_FALSE(cell_eq(top_left_south, CELL_NONE));
    EXPECT_FALSE(cell_eq(top_left_east, CELL_NONE));
    EXPECT_TRUE(cell_eq(past_end_north, CELL_NONE));
    EXPECT_TRUE(cell_eq(past_end_east, CELL_NONE));
    EXPECT_TRUE(cell_eq(past_end_west, CELL_NONE));
    EXPECT_TRUE(cell_eq(past_end_south, CELL_NONE));
}

TEST(cell, neighbour_idx_center) {
    // Arrange
    // Act
    size_t north = cell_neighbour_idx(4, DIRECTION_NORTH, 3, 9);
    size_t east  = cell_neighbour_idx(4, DIRECTION_EAST,  3, 9);
    size_t south = cell_neighbour_idx(4, DIRECTION_SOUTH, 3, 9);
    size_t west  = cell_neighbour_idx(4, DIRECTION_WEST,  3, 9);
    // Assert
    EXPECT_SIZE_EQ(north, 1);
    EXPECT_SIZE_EQ(east,  5);
    EXPECT_SIZE_EQ(south, 7);
    EXPECT_SIZE_EQ(west,  3);
}

TEST(cell, neighbour_idx_top_left) {
    // Arrange
    // Act
    size_t north = cell_neighbour_idx(0, DIRECTION_NORTH, 3, 9);
    size_t west  = cell_neighbour_idx(0, DIRECTION_WEST,  3, 9);
    size_t south = cell_neighbour_idx(0, DIRECTION_SOUTH, 3, 9);
    size_t east  = cell_neighbour_idx(0, DIRECTION_EAST,  3, 9);
    // Assert
    EXPECT_SIZE_EQ(north, CELL_NONE_IDX);
    EXPECT_SIZE_EQ(west,  CELL_NONE_IDX);
    EXPECT_SIZE_EQ(south, 3);
    EXPECT_SIZE_EQ(east,  1);
}

TEST(cell, neighbour_idx_wrap_prevent_east) {
    // Arrange
    // Act
    size_t n = cell_neighbour_idx(2, DIRECTION_EAST, 3, 9);
    // Assert
    EXPECT_SIZE_EQ(n, CELL_NONE_IDX);
}

TEST(cell, neighbour_idx_wrap_prevent_west) {
    // Arrange
    // Act
    size_t n = cell_neighbour_idx(3, DIRECTION_WEST, 3, 9);
    // Assert
    EXPECT_SIZE_EQ(n, CELL_NONE_IDX);
}

TEST(cell, neighbour_idx_out_of_bounds) {
    // Arrange
    // Act
    size_t n = cell_neighbour_idx(9, DIRECTION_SOUTH, 3, 9);
    // Assert
    EXPECT_SIZE_EQ(n, CELL_NONE_IDX);
}

TEST(cell, neighbour_idx_invalid_direction) {
    // Arrange
    // Act
    size_t n = cell_neighbour_idx(4, DIRECTION_INVALID, 3, 9);
    // Assert
    EXPECT_SIZE_EQ(n, CELL_NONE_IDX);
}

TEST(direction, row_deltas) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(direction_row_delta(DIRECTION_NORTH), -1);
    EXPECT_EQ(direction_row_delta(DIRECTION_SOUTH), 1);
    EXPECT_EQ(direction_row_delta(DIRECTION_EAST), 0);
    EXPECT_EQ(direction_row_delta(DIRECTION_WEST), 0);
    EXPECT_EQ(direction_row_delta(DIRECTION_INVALID), DIRECTION_DELTA_INVALID);
}

TEST(direction, col_deltas) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(direction_col_delta(DIRECTION_NORTH), 0);
    EXPECT_EQ(direction_col_delta(DIRECTION_SOUTH), 0);
    EXPECT_EQ(direction_col_delta(DIRECTION_EAST), 1);
    EXPECT_EQ(direction_col_delta(DIRECTION_WEST), -1);
    EXPECT_EQ(direction_col_delta(DIRECTION_INVALID), DIRECTION_DELTA_INVALID);
}

TEST(direction, arrows) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(direction_arrow(DIRECTION_NORTH), '^');
    EXPECT_EQ(direction_arrow(DIRECTION_SOUTH), 'v');
    EXPECT_EQ(direction_arrow(DIRECTION_EAST), '>');
    EXPECT_EQ(direction_arrow(DIRECTION_WEST), '<');
    EXPECT_EQ(direction_arrow(DIRECTION_INVALID), '?');
}

TEST(direction, between_cells) {
    // Arrange
    cell center = cell_from_row_col(1, 1, 3);
    cell north = cell_from_row_col(0, 1, 3);
    cell south = cell_from_row_col(2, 1, 3);
    cell east = cell_from_row_col(1, 2, 3);
    cell west = cell_from_row_col(1, 0, 3);
    // Act
    // Assert
    EXPECT_EQ(direction_between(center, south, 3), (direction) DIRECTION_SOUTH);
    EXPECT_EQ(direction_between(center, north, 3), (direction) DIRECTION_NORTH);
    EXPECT_EQ(direction_between(center, east, 3), (direction) DIRECTION_EAST);
    EXPECT_EQ(direction_between(center, west, 3), (direction) DIRECTION_WEST);
}

TEST(direction, between_invalid) {
    // Arrange
    cell center = cell_from_row_col(1, 1, 3);
    cell diagonal = cell_from_row_col(0, 2, 3);
    cell far = cell_from_row_col(2, 0, 3);
    cell oob = (cell){ .idx = 9 };
    // Act
    // Assert
    EXPECT_EQ(direction_between(center, diagonal, 3), (direction) DIRECTION_INVALID);
    EXPECT_EQ(direction_between(center, far, 3), (direction) DIRECTION_INVALID);
    EXPECT_EQ(direction_between(center, oob, 3), (direction) DIRECTION_INVALID);
}

int main(void) {
    RUN_TEST(cell, from_row_column);
    RUN_TEST(cell, from_row_column_zero);
    RUN_TEST(cell, row_of);
    RUN_TEST(cell, col_of);
    RUN_TEST(cell, row_of_col_of_all_cells);
    RUN_TEST(cell, equality);
    RUN_TEST(cell, none);
    RUN_TEST(cell, neighbour_of);
    RUN_TEST(cell, neighbour_out_of_bounds);
    RUN_TEST(cell, neighbour_idx_center);
    RUN_TEST(cell, neighbour_idx_top_left);
    RUN_TEST(cell, neighbour_idx_wrap_prevent_east);
    RUN_TEST(cell, neighbour_idx_wrap_prevent_west);
    RUN_TEST(cell, neighbour_idx_out_of_bounds);
    RUN_TEST(cell, neighbour_idx_invalid_direction);
    RUN_TEST(direction, row_deltas);
    RUN_TEST(direction, col_deltas);
    RUN_TEST(direction, arrows);
    RUN_TEST(direction, between_cells);
    RUN_TEST(direction, between_invalid);
    return test_summary();
}
