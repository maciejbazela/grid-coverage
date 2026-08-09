#pragma once

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CELL_DEF
#define CELL_DEF static inline
#endif

typedef struct {
    size_t idx;
} cell;

#define CELL_NONE_IDX SIZE_MAX
#define CELL_NONE ((cell){ .idx = CELL_NONE_IDX })

#define DIRECTION_COUNT 4

typedef enum {
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_EAST,
    DIRECTION_WEST,
    DIRECTION_INVALID,
} direction;

#define DIRECTION_DELTA_INVALID INT_MIN

CELL_DEF int64_t   direction_row_delta(direction d);
CELL_DEF int64_t   direction_col_delta(direction d);
CELL_DEF char      direction_arrow(direction d);
CELL_DEF direction direction_between(cell from, cell to, size_t cols);

CELL_DEF cell   cell_from_row_col(size_t row, size_t col, size_t cols);
CELL_DEF size_t cell_row(cell c, size_t cols);
CELL_DEF size_t cell_col(cell c, size_t cols);
CELL_DEF bool   cell_eq(cell a, cell b);
CELL_DEF size_t cell_neighbour_idx(size_t idx, direction d, size_t cols, size_t cell_count);
CELL_DEF cell   cell_neighbour(cell c, direction d, size_t cols, size_t cell_count);

#ifdef CELL_IMPL

CELL_DEF int64_t direction_row_delta(direction d) {
    static const int64_t deltas[DIRECTION_COUNT] = { -1, 1, 0, 0 };
    if (d >= DIRECTION_COUNT) return DIRECTION_DELTA_INVALID;
    return deltas[d];
}

CELL_DEF int64_t direction_col_delta(direction d) {
    static const int64_t deltas[DIRECTION_COUNT] = { 0, 0, 1, -1 };
    if (d >= DIRECTION_COUNT) return DIRECTION_DELTA_INVALID;
    return deltas[d];
}

CELL_DEF char direction_arrow(direction d) {
    static const char arrows[DIRECTION_COUNT] = { '^', 'v', '>', '<' };
    if (d >= DIRECTION_COUNT) return '?';
    return arrows[d];
}

CELL_DEF direction direction_between(cell from, cell to, size_t cols) {
    int64_t const delta = (int64_t) to.idx - (int64_t) from.idx;
    if (delta ==  (int64_t) cols)      return DIRECTION_SOUTH;
    else if (delta == -(int64_t) cols) return DIRECTION_NORTH;
    else if (delta == 1)               return DIRECTION_EAST;
    else if (delta == -1)              return DIRECTION_WEST;
    return DIRECTION_INVALID;
}

CELL_DEF cell cell_from_row_col(size_t row, size_t col, size_t cols) {
    return (cell) { .idx = row * cols + col };
}

CELL_DEF size_t cell_row(cell c, size_t cols) {
    return c.idx / cols;
}

CELL_DEF size_t cell_col(cell c, size_t cols) {
    return c.idx % cols;
}

CELL_DEF bool cell_eq(cell a, cell b) {
    return a.idx == b.idx;
}

CELL_DEF size_t cell_neighbour_idx(size_t idx, direction d, size_t cols, size_t cell_count) {
    if (d >= DIRECTION_COUNT) return CELL_NONE_IDX;
    if (cols == 0 || idx >= cell_count) return CELL_NONE_IDX;
    int64_t const delta = direction_row_delta(d) * (int64_t) cols + direction_col_delta(d);
    if (delta == 1  && idx % cols == cols - 1) return CELL_NONE_IDX;
    if (delta == -1 && idx % cols == 0)        return CELL_NONE_IDX;
    int64_t const n_idx = (int64_t) idx + delta;
    if (n_idx < 0 || (uint64_t) n_idx >= (uint64_t) cell_count) return CELL_NONE_IDX;
    return n_idx;
}

CELL_DEF cell cell_neighbour(cell c, direction d, size_t cols, size_t cell_count) {
    return (cell) { .idx = cell_neighbour_idx(c.idx, d, cols, cell_count) };
}

#endif // CELL_IMPL
