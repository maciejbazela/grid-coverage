#define GRID_IMPL
#include "../src/lib/grid.h"

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

TEST(path, unique_cells_empty) {
    // Arrange
    path* p = path_new(4);
    // Act
    size_t unique = path_unique_cells(p);
    // Assert
    EXPECT_SIZE_EQ(unique, 0);
    path_free(p);
}

TEST(path, unique_cells_single) {
    // Arrange
    path* p = path_new(4);
    cell c0 = cell_from_row_col(0, 0, 4);
    path_push(p, c0);
    // Act
    size_t unique = path_unique_cells(p);
    // Assert
    EXPECT_SIZE_EQ(unique, 1);
    path_free(p);
}

TEST(path, unique_cells_all_distinct) {
    // Arrange
    path* p = path_new(4);
    path_push(p, cell_from_row_col(0, 0, 4));
    path_push(p, cell_from_row_col(0, 1, 4));
    path_push(p, cell_from_row_col(0, 2, 4));
    // Act
    size_t unique = path_unique_cells(p);
    // Assert
    EXPECT_SIZE_EQ(unique, 3);
    EXPECT_SIZE_EQ(p->size, 3);
    path_free(p);
}

TEST(path, unique_cells_with_backtrack) {
    // Arrange
    path* p = path_new(8);
    path_push(p, cell_from_row_col(0, 0, 4));
    path_push(p, cell_from_row_col(0, 1, 4));
    path_push(p, cell_from_row_col(0, 2, 4));
    path_push(p, cell_from_row_col(0, 1, 4));
    path_push(p, cell_from_row_col(0, 0, 4));
    // Act
    size_t unique = path_unique_cells(p);
    // Assert
    EXPECT_SIZE_EQ(unique, 3);
    EXPECT_SIZE_EQ(p->size, 5);
    path_free(p);
}

TEST(path, unique_cells_same_cell_repeated) {
    // Arrange
    path* p = path_new(8);
    path_push(p, cell_from_row_col(0, 0, 4));
    path_push(p, cell_from_row_col(0, 0, 4));
    path_push(p, cell_from_row_col(0, 0, 4));
    // Act
    size_t unique = path_unique_cells(p);
    // Assert
    EXPECT_SIZE_EQ(unique, 1);
    EXPECT_SIZE_EQ(p->size, 3);
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
    RUN_TEST(path, unique_cells_empty);
    RUN_TEST(path, unique_cells_single);
    RUN_TEST(path, unique_cells_all_distinct);
    RUN_TEST(path, unique_cells_with_backtrack);
    RUN_TEST(path, unique_cells_same_cell_repeated);
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
