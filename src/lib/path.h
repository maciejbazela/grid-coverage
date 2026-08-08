#pragma once

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"

#ifndef PATH_DEF
#define PATH_DEF static inline
#endif

typedef struct {
    size_t idx;
} cell;

#define CELL_NONE ((cell){ .idx = SIZE_MAX })

#define DIRECTION_COUNT 4

typedef enum {
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_EAST,
    DIRECTION_WEST,
    DIRECTION_INVALID,
} direction;

#define DIRECTION_DELTA_INVALID INT_MIN

PATH_DEF int64_t   direction_row_delta(direction d);
PATH_DEF int64_t   direction_col_delta(direction d);
PATH_DEF char      direction_arrow(direction d);
PATH_DEF direction direction_between(cell from, cell to, size_t cols);

PATH_DEF cell   cell_from_row_col(size_t row, size_t col, size_t cols);
PATH_DEF size_t cell_row(cell c, size_t cols);
PATH_DEF size_t cell_col(cell c, size_t cols);
PATH_DEF bool   cell_eq(cell a, cell b);
PATH_DEF cell   cell_neighbor(cell c, direction d, size_t cols, size_t cell_count);

typedef struct {
    size_t size, capacity;
    cell cells[];
} path;

#define path_empty(p) ((p)->size == 0)
#define path_full(p)  ((p)->size == (p)->capacity)

PATH_DEF path* path_new(size_t capacity);
PATH_DEF void  path_free(path* p);
PATH_DEF bool  path_push(path* p, cell c);
PATH_DEF cell  path_pop(path* p);
PATH_DEF cell  path_top(const path* p);
PATH_DEF cell  path_at(const path* p, size_t i);
PATH_DEF bool  path_copy(path* dst, const path* src);
PATH_DEF void  path_clear(path* p);
PATH_DEF void  path_print(path* p, size_t cols);
PATH_DEF void  path_print_on_grid(path* p, grid* g);

#ifdef PATH_IMPL

PATH_DEF int64_t direction_row_delta(direction d) {
    static const int64_t deltas[DIRECTION_COUNT] = { -1, 1, 0, 0 };
    if (d >= DIRECTION_COUNT) return DIRECTION_DELTA_INVALID;
    return deltas[d];
}

PATH_DEF int64_t direction_col_delta(direction d) {
    static const int64_t deltas[DIRECTION_COUNT] = { 0, 0, 1, -1 };
    if (d >= DIRECTION_COUNT) return DIRECTION_DELTA_INVALID;
    return deltas[d];
}

PATH_DEF char direction_arrow(direction d) {
    static const char arrows[DIRECTION_COUNT] = { '^', 'v', '>', '<' };
    if (d >= DIRECTION_COUNT) return '?';
    return arrows[d];
}

PATH_DEF direction direction_between(cell from, cell to, size_t cols) {
    int64_t const delta = (int64_t) to.idx - (int64_t) from.idx;
    if (delta ==  (int64_t) cols)      return DIRECTION_SOUTH;
    else if (delta == -(int64_t) cols) return DIRECTION_NORTH;
    else if (delta == 1)               return DIRECTION_EAST;
    else if (delta == -1)              return DIRECTION_WEST;
    return DIRECTION_INVALID;
}

PATH_DEF cell cell_from_row_col(size_t row, size_t col, size_t cols) {
    return (cell) { .idx = row * cols + col };
}

PATH_DEF size_t cell_row(cell c, size_t cols) {
    return c.idx / cols;
}

PATH_DEF size_t cell_col(cell c, size_t cols) {
    return c.idx % cols;
}

PATH_DEF bool cell_eq(cell a, cell b) {
    return a.idx == b.idx;
}

PATH_DEF cell cell_neighbor(cell c, direction d, size_t cols, size_t cell_count) {
    if (d >= DIRECTION_COUNT) return CELL_NONE;
    if (cols == 0 || c.idx >= cell_count) return CELL_NONE;
    int64_t const delta = direction_row_delta(d) * (int64_t) cols + direction_col_delta(d);
    if (delta == 1  && c.idx % cols == cols - 1) return CELL_NONE;
    if (delta == -1 && c.idx % cols == 0)        return CELL_NONE;
    int64_t const n = (int64_t) c.idx + delta;
    if (n < 0 || (uint64_t) n >= (uint64_t) cell_count) return CELL_NONE;
    return (cell) { .idx = (size_t) n };
}

PATH_DEF path* path_new(size_t capacity) {
    if (capacity == 0) return NULL;
    if (capacity > (SIZE_MAX - sizeof(path)) / sizeof(cell)) return NULL;

    path* const p = malloc(sizeof(path) + sizeof(cell) * capacity);
    if (p == NULL) return NULL;
    p->size = 0;
    p->capacity = capacity;

    return p;
}

PATH_DEF void path_free(path* p) {
    if (p == NULL) return;
    free(p);
}

PATH_DEF bool path_push(path* p, cell c) {
    if (path_full(p)) return false;
    p->cells[p->size++] = c;
    return true;
}

PATH_DEF cell path_pop(path* p) {
    if (path_empty(p)) return CELL_NONE;
    return p->cells[--p->size];
}

PATH_DEF cell path_top(const path* p) {
    if (path_empty(p)) return CELL_NONE;
    return p->cells[p->size - 1];
}

PATH_DEF cell path_at(const path* p, size_t i) {
    if (i >= p->size) return CELL_NONE;
    return p->cells[i];
}

PATH_DEF bool path_copy(path* dst, const path* src) {
    if (dst->capacity < src->size) return false;
    memcpy(dst->cells, src->cells, src->size * sizeof(cell));
    dst->size = src->size;
    return true;
}

PATH_DEF void path_clear(path* p) {
    p->size = 0;
}

#define PATH_PRINT_MAX_ROW 8

PATH_DEF void path_print(path* p, size_t cols) {
    printf("Path(%ld) {\n", p->size);
    printf("    ");
    size_t in_row = 0;
    for (size_t i = 0; i < p->size - 1; ++i) {
        if (in_row == PATH_PRINT_MAX_ROW) {
            printf("\n    ");
            in_row = 0;
        }
        size_t row = cell_row(p->cells[i], cols);
        size_t col = cell_col(p->cells[i], cols);
        printf("(%ld, %ld), ", row, col);
        in_row++;
    }
    // Last cell without comma
    printf("(%ld, %ld)\n}\n", cell_row(p->cells[p->size - 1], cols), cell_col(p->cells[p->size - 1], cols));
}

PATH_DEF void path_print_on_grid(path* p, grid* g) {
    char* const buf = malloc(g->cell_count);
    if (buf == NULL) return;

    for (size_t i = 0; i < g->cell_count; ++i)
        buf[i] = g->cells[i] ? '#' : '.';

    if (p->size > 0) {
        for (size_t i = 1; i + 1 < p->size; ++i) {
            size_t const idx = p->cells[i].idx;
            if (idx >= g->cell_count) continue;
            direction d = direction_between(p->cells[i], p->cells[i + 1], g->cols);
            if (buf[idx] != '.') { // This cell has already been visited, mark it as backtracked - X
                buf[idx] = 'X';
                continue;
            }
            buf[idx] = direction_arrow(d);
        }
        if (p->cells[0].idx < g->cell_count)
            buf[p->cells[0].idx] = 'S';
        if (p->cells[p->size - 1].idx < g->cell_count)
            buf[p->cells[p->size - 1].idx] = 'E';
    }

    printf("Grid(%ld, %ld) with path of size %ld {\n", g->rows, g->cols, p->size);
    for (size_t row = 0; row < g->rows; ++row) {
        printf("  ");
        for (size_t col = 0; col < g->cols; ++col)
            printf("%c", buf[row * g->cols + col]);
        printf("\n");
    }
    printf("}\n");
    free(buf);
}

#endif // PATH_IMPL
