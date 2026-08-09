#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CELL_IMPL
#include "../src/lib/cell.h"

#define GRID_IMPL
#include "../src/lib/grid.h"

#define PARSE_IMPL
#include "../src/lib/parse.h"

#include "../src/lib/debug.h"

static bool write_ascii(const grid* g, FILE* f) {
    for (size_t row = 0; row < g->rows; ++row) {
        for (size_t col = 0; col < g->cols; ++col) {
            fputc(grid_at(g, row, col) == CELL_BLOCKED ? '#' : '.', f);
        }
        fputc('\n', f);
    }
    return !ferror(f);
}

static bool write_cell_list(const grid* g, FILE* f) {
    if (fprintf(f, "%zu %zu\n", g->rows, g->cols) < 0) return false;
    for (size_t row = 0; row < g->rows; ++row) {
        for (size_t col = 0; col < g->cols; ++col) {
            if (grid_at(g, row, col) == CELL_BLOCKED)
                if (fprintf(f, "%zu %zu\n", row, col) < 0) return false;
        }
    }
    return !ferror(f);
}

int main(int argc, char* argv[]) {
    bool use_cell_list = argc > 5 && strcmp(argv[5], "CELL_LIST") == 0;

    if (argc < 5) {
        printf("Usage: %s ROWS COLS P OUTPUT_FILE [CELL_LIST]\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t rows, cols;
    double p;

    if (!parse_size(argv[1], &rows)) {
        printf("Invalid ROWS: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!parse_size(argv[2], &cols)) {
        printf("Invalid COLS: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    if (!parse_double(argv[3], &p)) {
        printf("Invalid P: %s\n", argv[3]);
        return EXIT_FAILURE;
    }
    if (p < 0.0 || p > 1.0) {
        printf("P must be in [0.0, 1.0], got %f\n", p);
        return EXIT_FAILURE;
    }

    FILE* out = NULL;
    grid* g   = NULL;

    out = fopen(argv[4], "w");
    if (out == NULL) {
        perror(argv[4]);
        return EXIT_FAILURE;
    }

    srand((unsigned) time(NULL));

    g = grid_new(rows, cols);
    if (g == NULL) goto fail;

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if ((double) rand() / RAND_MAX < p)
                grid_set(g, row, col);
        }
    }

    DEBUG_PRINT("[random] filled grid with blocked probability %f\n", p);

    bool ok = use_cell_list ? write_cell_list(g, out) : write_ascii(g, out);
    if (!ok) goto fail;

    grid_free(g);
    fclose(out);
    return EXIT_SUCCESS;

fail:
    if (g) grid_free(g);
    if (out) fclose(out);
    return EXIT_FAILURE;
}
