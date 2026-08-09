#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CELL_IMPL
#include "../src/lib/cell.h"

#define GRID_IMPL
#include "../src/lib/grid.h"

#define QUEUE_IMPL
#include "../src/lib/queue.h"

#define PARSE_IMPL
#include "../src/lib/parse.h"

#include "../src/lib/debug.h"

#define MAX_RERUNS 5

static size_t find_random_unvisited(const bool* visited, size_t cell_count) {
    size_t idx;
    do {
        idx = (size_t) rand() % cell_count;
    } while(visited[idx]);
    return idx;
}

static size_t bfs_grow(const grid* g, bool* visited, size_t* comp_cells, size_t start, size_t max_size) {
    queue* q = queue_new(g->cell_count);
    if (q == NULL) return 0;

    size_t comp_size = 0;
    queue_push(q, start);
    visited[start] = true;
    comp_cells[comp_size++] = start;

    while (!queue_empty(q) && comp_size < max_size) {
        size_t v = queue_pop(q);

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            size_t n_idx = cell_neighbour_idx(v, d, g->cols, g->cell_count);
            if (n_idx == CELL_NONE_IDX) continue;
            if (visited[n_idx]) continue;

            visited[n_idx] = true;
            comp_cells[comp_size++] = n_idx;
            queue_push(q, n_idx);

            if (comp_size >= max_size) break;
        }
    }

    DEBUG_PRINT("[bfs_grow] start %zu grew to %zu (max %zu)\n", start, comp_size, max_size);
    queue_free(q);
    return comp_size;
}

static void mark_border_as_visited(const grid* g, bool* visited, const size_t* comp_cells, size_t comp_size) {
    for (size_t i = 0; i < comp_size; ++i) {
        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            size_t n_idx = cell_neighbour_idx(comp_cells[i], d, g->cols, g->cell_count);
            if (n_idx == CELL_NONE_IDX) continue;
            if (visited[n_idx]) continue;
            visited[n_idx] = true;
        }
    }
}

// Try to place k open components of random sizes in a blocked grid
// Each component is grown by BFS from a random unvisited cell, then its perimeter is marked visited so components do not touch
// Retries up to MAX_RERUNS times until k components are placed.
static grid* generate_components(size_t rows, size_t cols, int k, int min_size, int max_size) {
    size_t cell_count = rows * cols;
    size_t size_range = (size_t)(max_size - min_size);

    bool* visited       = malloc(cell_count * sizeof(bool));
    size_t* comp_cells  = malloc((size_t) max_size * sizeof(size_t));
    if (visited == NULL || comp_cells == NULL) goto fail;

    size_t components = 0;
    int    reruns     = 0;

    while (components < (size_t) k && reruns <= MAX_RERUNS) {
        grid* g = grid_new(rows, cols);
        if (g == NULL) goto fail;

        memset(g->cells, 1, cell_count * sizeof(g->cells[0]));
        memset(visited, 0, cell_count * sizeof(bool));
        components = 0;

        DEBUG_PRINT("[generate_components] starting attempt (rerun %d)\n", reruns);

        while (components < (size_t) k) {
            size_t size  = (size_t) min_size + (size_t) rand() % (size_range + 1);
            size_t start = find_random_unvisited(visited, cell_count);
            if (start == CELL_NONE_IDX) break;

            size_t c_size = bfs_grow(g, visited, comp_cells, start, size);
            if (c_size == 0) break;

            for (size_t i = 0; i < c_size; ++i)
                grid_unset(g, comp_cells[i] / cols, comp_cells[i] % cols);

            mark_border_as_visited(g, visited, comp_cells, c_size);
            DEBUG_PRINT("[generate_components] component %zu: %zu cells\n", components, c_size);

            ++components;
        }

        if (components < (size_t) k) {
            grid_free(g);
            ++reruns;
            continue;
        }

        DEBUG_PRINT("[generate_components] done, wrote %zu components\n", components);
        free(comp_cells);
        free(visited);
        return g;
    }

fail:
    if (visited) free(visited);
    if (comp_cells) free(comp_cells);
    return NULL;
}

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
    bool use_cell_list = argc > 7 && strcmp(argv[7], "CELL_LIST") == 0;

    if (argc < 7) {
        printf("Usage: %s ROWS COLS K MIN_SIZE MAX_SIZE OUTPUT_FILE [CELL_LIST]\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t rows, cols;
    int    k, min_size, max_size;

    if (!parse_size(argv[1], &rows)) {
        printf("Invalid ROWS: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!parse_size(argv[2], &cols)) {
        printf("Invalid COLS: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    if (!parse_int(argv[3], &k)) {
        printf("Invalid K: %s\n", argv[3]);
        return EXIT_FAILURE;
    }
    if (!parse_int(argv[4], &min_size)) {
        printf("Invalid MIN_SIZE: %s\n", argv[4]);
        return EXIT_FAILURE;
    }
    if (!parse_int(argv[5], &max_size)) {
        printf("Invalid MAX_SIZE: %s\n", argv[5]);
        return EXIT_FAILURE;
    }

    if (k < 0) {
        printf("K must be >= 0, got %d\n", k);
        return EXIT_FAILURE;
    }
    if (min_size < 1 || min_size > max_size) {
        printf("MIN_SIZE must be >= 1 and <= MAX_SIZE\n");
        return EXIT_FAILURE;
    }

    FILE* out = fopen(argv[6], "w");
    if (out == NULL) {
        perror(argv[6]);
        return EXIT_FAILURE;
    }

    srand((unsigned) time(NULL));

    grid* g = generate_components(rows, cols, k, min_size, max_size);
    if (g == NULL) {
        fprintf(stderr, "Could not generate %d components after %d retries\n", k, MAX_RERUNS);
        fclose(out);
        return EXIT_FAILURE;
    }

    bool ok = use_cell_list ? write_cell_list(g, out) : write_ascii(g, out);
    grid_free(g);
    fclose(out);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
