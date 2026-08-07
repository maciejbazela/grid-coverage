#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef PATH_DEF
#define PATH_DEF static inline
#endif

typedef struct {
    size_t idx;
} cell;

#define CELL_NONE ((cell){ .idx = SIZE_MAX })

PATH_DEF cell   cell_from_row_col(size_t row, size_t col, size_t cols);
PATH_DEF size_t cell_row(cell c, size_t cols);
PATH_DEF size_t cell_col(cell c, size_t cols);
PATH_DEF bool   cell_eq(cell a, cell b);

typedef struct {
    size_t size, capacity;
    cell data[];
} path;

PATH_DEF path* path_new(size_t capacity);
PATH_DEF void  path_free(path* p);
PATH_DEF bool  path_push(path* p, cell c);
PATH_DEF cell  path_pop(path* p);
PATH_DEF cell  path_top(const path* p);
PATH_DEF cell  path_at(const path* p, size_t i);
PATH_DEF bool  path_copy(path* dst, const path* src);
PATH_DEF void  path_clear(path* p);

#ifdef PATH_IMPL

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
    if (p->size >= p->capacity) return false;
    p->data[p->size++] = c;
    return true;
}

PATH_DEF cell path_pop(path* p) {
    return p->data[--p->size];
}

PATH_DEF cell path_top(const path* p) {
    return p->data[p->size - 1];
}

PATH_DEF cell path_at(const path* p, size_t i) {
    return p->data[i];
}

PATH_DEF bool path_copy(path* dst, const path* src) {
    if (dst->capacity < src->size) return false;
    memcpy(dst->data, src->data, src->size * sizeof(cell));
    dst->size = src->size;
    return true;
}

PATH_DEF void path_clear(path* p) {
    p->size = 0;
}

#endif // PATH_IMPL
