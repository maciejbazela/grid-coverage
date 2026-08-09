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

#define STACK_IMPL
#include "../src/lib/stack.h"

#include "../src/lib/debug.h"

#define PARSE_IMPL
#include "../src/lib/parse.h"

static void add_room_walls_to_frontier(stack* frontier, size_t rr, size_t rc, const grid* g) {
    size_t start = (size_t) rand() % DIRECTION_COUNT;
    for (size_t i = 0; i < DIRECTION_COUNT; ++i) {
        direction d = (direction)((start + i) % DIRECTION_COUNT);
        int64_t wr = (int64_t) rr + direction_row_delta(d);
        int64_t wc = (int64_t) rc + direction_col_delta(d);

        if (wr < 0 || (size_t) wr >= g->rows) continue;
        if (wc < 0 || (size_t) wc >= g->cols) continue;

        if (grid_at(g, (size_t) wr, (size_t) wc) != CELL_BLOCKED) continue;

        stack_push(frontier, (size_t) wr * g->cols + (size_t) wc);
    }
}

// Given a wall cell, compute the indices of the two rooms it separates
static bool wall_rooms(size_t wr, size_t wc, size_t rows, size_t cols, size_t* r1_idx, size_t* r2_idx) {
    if (wr % 2 == 0 && wc % 2 == 1) {
        // Vertical wall, rooms are above and below
        if (wr == 0 || wr + 1 >= rows) return false;
        *r1_idx = (wr - 1) * cols + wc;
        *r2_idx = (wr + 1) * cols + wc;
        return true;
    }
    if (wr % 2 == 1 && wc % 2 == 0) {
        // Horizontal wall, rooms are left and right
        if (wc == 0 || wc + 1 >= cols) return false;
        *r1_idx = wr * cols + (wc - 1);
        *r2_idx = wr * cols + (wc + 1);
        return true;
    }
    return false;
}

// Start from a random room cell and grow the maze by repeatedly popping a frontier wall
// If the wall separates exactly one passage cell from one unvisited room, carve through the wall
// into the new room and push its adjacent walls onto the frontier
static void maze_prim(grid* g, stack* frontier) {
    size_t rows = g->rows;
    size_t cols = g->cols;

    // Pick random room cell (odd row, odd col)
    size_t room_rows = rows > 2 ? (rows - 1) / 2 : 0;
    size_t room_cols = cols > 2 ? (cols - 1) / 2 : 0;
    if (room_rows == 0 || room_cols == 0) return;
    size_t rr = 1 + 2 * ((size_t) rand() % room_rows);
    size_t rc = 1 + 2 * ((size_t) rand() % room_cols);

    grid_unset(g, rr, rc);
    DEBUG_PRINT("[maze_prim] start room (%zu, %zu)\n", rr, rc);
    add_room_walls_to_frontier(frontier, rr, rc, g);

    while (!stack_empty(frontier)) {
        size_t w_idx = stack_pop(frontier);
        size_t wr = w_idx / cols;
        size_t wc = w_idx % cols;

        size_t r1_idx, r2_idx;
        if (!wall_rooms(wr, wc, rows, cols, &r1_idx, &r2_idx)) continue;
        size_t r1_row = r1_idx / cols, r1_col = r1_idx % cols;
        size_t r2_row = r2_idx / cols, r2_col = r2_idx % cols;

        // Only carve if exactly one adjacent room is already a passage
        bool r1_passage = grid_at_flat(g, r1_idx) == CELL_FREE;
        bool r2_passage = grid_at_flat(g, r2_idx) == CELL_FREE;
        if (r1_passage == r2_passage) continue;

        DEBUG_PRINT(
            "[maze_prim] carving wall (%zu, %zu) -> room (%zu, %zu)\n",
            wr, wc, r1_passage ? r2_row : r1_row, r1_passage ? r2_col : r1_col
        );

        grid_unset(g, wr, wc);

        if (r1_passage) {
            grid_unset(g, r2_row, r2_col);
            add_room_walls_to_frontier(frontier, r2_row, r2_col, g);
        }
        else {
            grid_unset(g, r1_row, r1_col);
            add_room_walls_to_frontier(frontier, r1_row, r1_col, g);
        }
    }

    DEBUG_PRINT("[maze_prim] done\n");
}

static void maze_remove_random_walls(grid* g, double p) {
    size_t rows = g->rows;
    size_t cols = g->cols;

    // With probability p, remove walls that separate two rooms which are already connected.
    if (p <= 0.0) return;

    for (size_t wr = 0; wr < rows; ++wr) {
        for (size_t wc = 0; wc < cols; ++wc) {
            if (grid_at(g, wr, wc) != CELL_BLOCKED) continue;

            size_t r1_idx, r2_idx;
            if (!wall_rooms(wr, wc, rows, cols, &r1_idx, &r2_idx)) continue;

            if (grid_at_flat(g, r1_idx) == CELL_FREE && grid_at_flat(g, r2_idx) == CELL_FREE) {
                if ((double) rand() / RAND_MAX < p) {
                    DEBUG_PRINT("[maze_remove_random_walls] extra passage at (%zu, %zu)\n", wr, wc);
                    grid_unset(g, wr, wc);
                }
            }
        }
    }
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

    FILE* out       = NULL;
    grid* g         = NULL;
    stack* frontier = NULL;

    out = fopen(argv[4], "w");
    if (out == NULL) {
        perror(argv[4]);
        return EXIT_FAILURE;
    }

    srand((unsigned) time(NULL));

    frontier = stack_new(rows * cols);
    if (frontier == NULL) goto fail;

    g = grid_new(rows, cols);
    if (g == NULL) goto fail;
    memset(g->cells, 1, rows * cols * sizeof(g->cells[0]));

    maze_prim(g, frontier);
    maze_remove_random_walls(g, p);

    bool ok = use_cell_list ? write_cell_list(g, out) : write_ascii(g, out);
    if (!ok) goto fail;

    stack_free(frontier);
    grid_free(g);
    fclose(out);
    return EXIT_SUCCESS;

fail:
    if (frontier) stack_free(frontier);
    if (g) grid_free(g);
    if (out) fclose(out);
    return EXIT_FAILURE;
}
