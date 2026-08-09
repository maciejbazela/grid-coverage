#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "grid.h"

#ifndef PATH_DEF
#define PATH_DEF static inline
#endif

typedef struct {
    size_t size, capacity;
    cell cells[];
} path;

#define path_empty(p) ((p)->size == 0)
#define path_full(p)  ((p)->size == (p)->capacity)

PATH_DEF path* path_new(size_t capacity);
PATH_DEF void  path_free(path* p);
PATH_DEF bool  path_push_idx(path* p, size_t idx);
PATH_DEF bool  path_push(path* p, cell c);
PATH_DEF cell  path_pop(path* p);
PATH_DEF cell  path_top(const path* p);
PATH_DEF cell  path_at(const path* p, size_t i);
PATH_DEF bool  path_copy(path* dst, const path* src);
PATH_DEF void  path_clear(path* p);
PATH_DEF void  path_print(const path* p, size_t cols);
PATH_DEF void  path_print_on_grid(const path* p, const grid* g);

#ifdef PATH_IMPL

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

PATH_DEF bool path_push_idx(path* p, size_t idx) {
    return path_push(p, (cell){ .idx = idx });
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

PATH_DEF void path_print(const path* p, size_t cols) {
    printf("Path(%zu) {\n", p->size);
    printf("    ");
    size_t in_row = 0;
    for (size_t i = 0; i < p->size - 1; ++i) {
        if (in_row == PATH_PRINT_MAX_ROW) {
            printf("\n    ");
            in_row = 0;
        }
        size_t row = cell_row(p->cells[i], cols);
        size_t col = cell_col(p->cells[i], cols);
        printf("(%zu, %zu), ", row, col);
        ++in_row;
    }
    // Last cell without comma
    printf("(%zu, %zu)\n}\n", cell_row(p->cells[p->size - 1], cols), cell_col(p->cells[p->size - 1], cols));
}

PATH_DEF void path_print_on_grid(const path* p, const grid* g) {
    char* const buf = malloc(g->cell_count);
    if (buf == NULL) return;

    for (size_t i = 0; i < g->cell_count; ++i)
        buf[i] = g->cells[i] ? '#' : '.';

    size_t start_idx = CELL_NONE_IDX;
    size_t end_idx = CELL_NONE_IDX;

    if (p->size > 0) {
        start_idx = p->cells[0].idx;
        end_idx = p->cells[p->size - 1].idx;

        for (size_t i = 1; i + 1 < p->size; ++i) {
            size_t const idx = p->cells[i].idx;
            if (idx >= g->cell_count) continue;
            direction d = direction_between(p->cells[i], p->cells[i + 1], g->cols);
            if (buf[idx] != '.') {
                buf[idx] = 'X';
                continue;
            }
            buf[idx] = direction_arrow(d);
        }

        if (start_idx < g->cell_count && p->size > 1) {
            direction d = direction_between(p->cells[0], p->cells[1], g->cols);
            buf[start_idx] = d == DIRECTION_INVALID ? 'S' : direction_arrow(d);
        }
        else if (start_idx < g->cell_count) {
            buf[start_idx] = 'S';
        }

        if (end_idx < g->cell_count)
            buf[end_idx] = 'E';
    }

    printf("Grid(%zu, %zu) with path of size %zu\n", g->rows, g->cols, p->size);
    for (size_t row = 0; row < g->rows; ++row) {
        for (size_t col = 0; col < g->cols; ++col) {
            size_t const idx = row * g->cols + col;
            char const c = buf[idx];
            bool const is_path_cell = (c != '#' && c != '.');

            if (is_path_cell && idx == start_idx)
                printf("\033[1;32m%c\033[0m", c);
            else if (is_path_cell && idx == end_idx)
                printf("\033[1;31m%c\033[0m", c);
            else if (is_path_cell)
                printf("\033[1;37m%c\033[0m", c);
            else
                printf("%c", c);
        }
        printf("\n");
    }
    free(buf);
}

#endif // PATH_IMPL
