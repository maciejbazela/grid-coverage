#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef GRAPH_DEF
#define GRAPH_DEF static inline
#endif

typedef struct {
    size_t rows, cols;
    bool cells[]; // TODO: use some other type and store cells as bits
} graph;

GRAPH_DEF graph* graph_new(size_t rows, size_t cols);
GRAPH_DEF bool   graph_set(graph* g, size_t row, size_t col);
GRAPH_DEF bool   graph_at(graph* g, size_t row, size_t col);
GRAPH_DEF void   graph_print(graph* g);
GRAPH_DEF void   graph_free(graph* g);

#ifdef GRAPH_IMPL

GRAPH_DEF graph* graph_new(size_t rows, size_t cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    graph* const g = malloc(sizeof(graph) + sizeof(bool) * rows * cols);
    if (g == NULL) return NULL;
    g->rows = rows;
    g->cols = cols;

    return g;
}

GRAPH_DEF bool graph_set(graph* g, size_t row, size_t col) {
    if (row >= g->rows) return false;
    if (col >= g->cols) return false;
    size_t stride = row * g->rows + col;
    g->cells[stride] = true;
    return true;
}

GRAPH_DEF bool graph_at(graph* g, size_t row, size_t col) {
    if (row >= g->rows) return false;
    if (col >= g->cols) return false;
    size_t stride = row * g->rows + col;
    return g->cells[stride];
}

GRAPH_DEF void graph_print(graph* g) {
    printf("Graph(%ld, %ld) {\n", g->rows, g->cols);
    for (size_t row = 0; row < g->rows; ++row) {
        for(size_t col = 0; col < g->cols; ++col) {
            size_t stride = row * g->rows + col;
            if (g->cells[stride]) printf("#");
            else printf(".");
        }
        printf("\n");
    }
}

GRAPH_DEF void graph_free(graph* g) {
    if (g == NULL) return;
    free(g);
}

#endif // GRAPH_IMPL
