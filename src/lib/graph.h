#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifndef GRAPH_DEF
#define GRAPH_DEF static inline
#endif

typedef struct {
    size_t rows, cols;
    bool cells[]; // TODO: use some other type and store cells as bits
} graph;

GRAPH_DEF graph* graph_new(size_t rows, size_t cols);
GRAPH_DEF void graph_free(graph* g);

// TODO: add tests!

#ifdef GRAPH_IMPL

GRAPH_DEF graph* graph_new(size_t rows, size_t cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    graph* const g = malloc(sizeof(graph) + sizeof(bool) * rows * cols);
    if (g == NULL) return NULL;
    g->rows = rows;
    g->cols = cols;

    return g;
}

GRAPH_DEF void graph_free(graph* g) {
    if (g == NULL) return;
    free(g);
}

#endif // GRAPH_IMPL
