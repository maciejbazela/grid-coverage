#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"

#ifndef GRID_DEF
#define GRID_DEF static inline
#endif

typedef struct {
    size_t rows, cols;
    size_t cell_count;
    bool cells[];
} grid;

typedef enum {
    CELL_OUT_OF_BOUNDS,
    CELL_FREE,
    CELL_BLOCKED,
} cell_state;

GRID_DEF grid*      grid_new(size_t rows, size_t cols);
GRID_DEF grid*      grid_new_with_blocked(size_t rows, size_t cols, const cell* blocked, size_t blocked_count);
GRID_DEF void       grid_free(grid* g);
GRID_DEF bool       grid_set(grid* g, size_t row, size_t col);
GRID_DEF bool       grid_unset(grid* g, size_t row, size_t col);
GRID_DEF cell_state grid_at(const grid* g, size_t row, size_t col);
GRID_DEF cell_state grid_at_flat(const grid* g, size_t idx);
GRID_DEF void       grid_print(const grid* g);

#ifdef GRID_IMPL

GRID_DEF grid* grid_new(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) return NULL;
    if (rows > SIZE_MAX / cols) return NULL;
    if (rows * cols > (SIZE_MAX - sizeof(grid)) / sizeof(bool)) return NULL;

    grid* const g = calloc(1, sizeof(grid) + sizeof(bool) * rows * cols);
    if (g == NULL) return NULL;
    g->rows = rows;
    g->cols = cols;
    g->cell_count = rows * cols;

    return g;
}

GRID_DEF grid* grid_new_with_blocked(size_t rows, size_t cols, const cell* blocked, size_t blocked_count) {
    grid* g = grid_new(rows, cols);
    if (g == NULL) return NULL;

    for (size_t i = 0; i < blocked_count; ++i) {
        size_t idx = blocked[i].idx;
        if (idx < g->cell_count)
            g->cells[idx] = true;
    }

    return g;
}

GRID_DEF void grid_free(grid* g) {
    if (g == NULL) return;
    free(g);
}

GRID_DEF bool grid_set(grid* g, size_t row, size_t col) {
    if (row >= g->rows) return false;
    if (col >= g->cols) return false;
    size_t stride = row * g->cols + col;
    g->cells[stride] = true;
    return true;
}

GRID_DEF bool grid_unset(grid* g, size_t row, size_t col) {
    if (row >= g->rows) return false;
    if (col >= g->cols) return false;
    size_t stride = row * g->cols + col;
    g->cells[stride] = false;
    return true;
}

GRID_DEF cell_state grid_at(const grid* g, size_t row, size_t col) {
    if (row >= g->rows || col >= g->cols) return CELL_OUT_OF_BOUNDS;
    size_t stride = row * g->cols + col;
    return g->cells[stride] ? CELL_BLOCKED : CELL_FREE;
}

GRID_DEF cell_state grid_at_flat(const grid* g, size_t idx) {
    if (idx >= g->cell_count) return CELL_OUT_OF_BOUNDS;
    return g->cells[idx] ? CELL_BLOCKED : CELL_FREE;
}

GRID_DEF void grid_print(const grid* g) {
    printf("Grid(%zu, %zu)\n", g->rows, g->cols);
    for (size_t row = 0; row < g->rows; ++row) {
        for (size_t col = 0; col < g->cols; ++col) {
            size_t stride = row * g->cols + col;
            if (g->cells[stride]) printf("#");
            else printf(".");
        }
        printf("\n");
    }
}

#endif // GRID_IMPL
