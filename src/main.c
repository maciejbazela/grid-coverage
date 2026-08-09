#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./lib/lib.h"

#define PARSING_ERROR ((size_t) - 1)
#define LINE_BUF_LENGTH 256

static bool parse_int(string_view sv, int32_t* out) {
    char  buf[64];
    char* end;
    long  value;

    sv = sv_trim(sv);
    if (sv.size >= sizeof(buf)) return false;
    if (!sv_copy_cstr(buf, sv, sizeof(buf))) return false;

    errno = 0;
    value = strtol(buf, &end, 10);
    if (errno == ERANGE) return false;
    if (*end != '\0') return false;
    if (value < INT32_MIN || value > INT32_MAX) return false;

    *out = (int32_t) value;
    return true;
}

static bool parse_pair(string_view sv, int32_t* a, int32_t* b) {
    sv = sv_left_trim(sv);

    string_view first = sv_get_word(sv);
    sv = sv_skip_word(sv);
    sv = sv_left_trim(sv);

    string_view second = sv_get_word(sv);
    sv = sv_skip_word(sv);
    sv = sv_right_trim(sv);

    if (sv.size > 0) return false;
    return parse_int(first, a) && parse_int(second, b);
}

// Expected format:
// ROWS COLS
// CELL1_ROW CELL1_COL
// CELL2_ROW CELL2_COL
// ...
//
// Where (CELL1_ROW, CELL1_COL) indicated that a cell is blocked
static grid* parse_grid_stdin() {
    DEBUG_PRINT("[parse_grid_stdin] reading from stdin\n");
    char line[LINE_BUF_LENGTH];
    int32_t rows, cols;

    if (!fgets(line, sizeof(line), stdin)) {
        DEBUG_PRINT("[parse_grid_stdin] failed to read first line\n");
        return NULL;
    }
    string_view sv_line = sv_trim(sv_from_cstr(line));

    if (!parse_pair(sv_line, &rows, &cols)) {
        DEBUG_PRINT("[parse_grid_stdin] could not parse row/col pair from line: '"SV_PRINT_FMT"'\n", SV_PRINT_ARG(sv_line));
        return NULL;
    }
    if (rows <= 0 || cols <= 0) {
        DEBUG_PRINT("[parse_grid_stdin] invalid dimensions: %d rows, %d cols\n", rows, cols);
        return NULL;
    }

    DEBUG_PRINT("[parse_grid_stdin] grid rows: %d, columns: %d\n", rows, cols);
    grid* const g = grid_new(rows, cols);

    while (fgets(line, sizeof(line), stdin)) {
        sv_line = sv_trim(sv_from_cstr(line));
        int32_t cell_row, cell_col;
        if (!parse_pair(sv_line, &cell_row, &cell_col)) {
            DEBUG_PRINT("[parse_grid_stdin] could not parse line: '"SV_PRINT_FMT"'\n", SV_PRINT_ARG(sv_line));
            continue;
        }
        if (cell_row < 0 || cell_row >= rows) {
            DEBUG_PRINT("[parse_grid_stdin] row %d out of bounds [0, %d)\n", cell_row, rows);
        }
        if (cell_col < 0 || cell_col >= cols) {
            DEBUG_PRINT("[parse_grid_stdin] col %d out of bounds [0, %d)\n", cell_col, cols);
        }
        DEBUG_PRINT("[parse_grid_stdin] cell (%d, %d) is blocked\n", cell_row, cell_col);
        grid_set(g, cell_row, cell_col);
    }
    DEBUG_PRINT("[parse_grid_stdin] done\n");
    return g;
}

static size_t read_line(char buf[LINE_BUF_LENGTH], FILE* f) {
    size_t len = 0;
    for (;;) {
        if (fgets(buf, LINE_BUF_LENGTH, f) == NULL) {
            if (len == 0) return PARSING_ERROR; // Error or empty file
            break;
        }

        char* nl = strchr(buf, '\n');
        if (nl) { // Has newline, check for \r and adjust length then exit
            size_t chunk = (size_t)(nl - buf);
            if (chunk > 0 && buf[chunk - 1] == '\r') chunk--;
            len += chunk;
            break;
        }

        size_t chunk = strlen(buf);
        len += chunk;
        if (chunk < LINE_BUF_LENGTH - 1) break; // EOF (no new line, chunk smaller than max)
    }
    return len;
}

static size_t fill_grid_row(grid* g, size_t row, FILE* f) {
    char   buf[LINE_BUF_LENGTH];
    size_t col = 0;

    for (;;) {
        if (fgets(buf, LINE_BUF_LENGTH, f) == NULL) {
            if (col == 0) return PARSING_ERROR;
            break;
        }

        char*  nl = strchr(buf, '\n');
        size_t chunk;
        if (nl) {
            chunk = (size_t)(nl - buf);
            if (chunk > 0 && buf[chunk - 1] == '\r') chunk--;
        }
        else
            chunk = strlen(buf);

        for (size_t i = 0; i < chunk; ++i) {
            char c = buf[i];
            if (c == '#') {
                if (!grid_set(g, row, col)) return PARSING_ERROR;
            }
            else if (c != '.')
                return (size_t) PARSING_ERROR; // Invalid character
            col++;
        }

        if (nl) break;
        if (chunk < LINE_BUF_LENGTH - 1) break; // EOF
    }
    return col;
}

// Expected format: visual representation of the grid
// . empty cell
// # blocked cell
// Every line must have the same number of characters, otherwise this will fail and return NULL
static grid* parse_grid_file(const char* path) {
    DEBUG_PRINT("[parse_grid_file] opening '%s'\n", path);
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return NULL;
    }
    grid* g = NULL;

    char   buf[LINE_BUF_LENGTH];
    size_t rows = 0, cols = 0;

    for (;;) {
        size_t len = read_line(buf, f);
        if (len == PARSING_ERROR) break;
        DEBUG_PRINT("[parse_grid_file] read line %zu (len=%zu)\n", rows, len);
        if (rows == 0) {
            cols = len;
            if (cols == 0) {
                DEBUG_PRINT("[parse_grid_file] empty first row, aborting\n");
                goto fail;
            }
            DEBUG_PRINT("[parse_grid_file] cols determined as %zu\n", cols);
        }
        else if (len != cols) {
            DEBUG_PRINT("[parse_grid_file] row %zu has length %zu, expected %zu, aborting\n", rows, len, cols);
            goto fail;
        }
        rows++;
    }
    if (ferror(f) || rows == 0) {
        DEBUG_PRINT("[parse_grid_file] error reading file or zero rows\n");
        goto fail;
    }

    DEBUG_PRINT("[parse_grid_file] counted %zu rows x %zu cols\n", rows, cols);
    rewind(f);

    g = grid_new(rows, cols);
    if (g == NULL) {
        DEBUG_PRINT("[parse_grid_file] failed to allocate grid\n");
        goto fail;
    }

    for (size_t row = 0; row < rows; ++row) {
        size_t len = fill_grid_row(g, row, f);
        if (len == PARSING_ERROR) {
            DEBUG_PRINT("[parse_grid_file] fill_grid_row returned PARSING_ERROR\n");
            goto fail;
        }
        if (len != cols) {
            DEBUG_PRINT("[parse_grid_file] fill_grid_row row %zu returned %zu, expected %zu\n", row, len, cols);
            goto fail;
        }
        DEBUG_PRINT("[parse_grid_file] filled row %zu (%zu cols)\n", row, len);
    }

    DEBUG_PRINT("[parse_grid_file] done\n");
    fclose(f);
    return g;

fail:
    if (g) grid_free(g);
    if (f) fclose(f);
    return NULL;
}

static path* random_wrapper(const grid* g, size_t max_steps, cell start, const grid_context* ctx) {
    return walk_solve_random(g, max_steps, start, ctx, (unsigned) time(NULL));
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s ALGORITHM MAX_STEPS [file_path]\n", argv[0]);
        printf("  ALGORITHM: deep, random, warnsdorff, heuristic\n");
        return EXIT_FAILURE;
    }

    const char* algo_name = argv[1];
    int32_t parsed_steps;
    if (!parse_int(sv_from_cstr(argv[2]), &parsed_steps) || parsed_steps < 0) {
        printf("Invalid MAX_STEPS: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    size_t max_steps = (size_t) parsed_steps;

    grid* g = NULL;
    grid_context* ctx = NULL;
    path* p = NULL;

    if (argc >= 4)
        g = parse_grid_file(argv[3]);
    else
        g = parse_grid_stdin();
    if (g == NULL) return EXIT_FAILURE;

    DEBUG_SCOPE {
        printf("grid parsed successfully!\n");
        grid_print(g);
    }

    ctx = grid_preprocess(g);
    if (ctx == NULL) goto fail;

    traverse_fn inner;
    if (strcmp(algo_name, "deep") == 0)
        inner = walk_solve_deep;
    else if (strcmp(algo_name, "random") == 0)
        inner = random_wrapper;
    else if (strcmp(algo_name, "warnsdorff") == 0)
        inner = walk_solve_warnsdorff;
    else if (strcmp(algo_name, "heuristic") == 0)
        inner = walk_solve_heuristic;
    else {
        printf("Unknown algorithm: %s\n", algo_name);
        goto fail;
    }
    DEBUG_PRINT("Running %s from all good starts\n", algo_name);
    p = walk_solve_multistart(g, max_steps - 1, inner, ctx);
    if (p == NULL) {
        printf("No path found\n");
        goto fail;
    }

    printf("Best result for algorithm: 'walk_solve_%s':\n", algo_name);
    for (size_t i = 0; i < p->size; ++i) {
        cell c = path_at(p, i);
        printf("(%zu, %zu)", cell_row(c, g->cols), cell_col(c, g->cols));
        if (i + 1 < p->size) printf(", ");
    }
    printf(", %zu\n", path_unique_cells(p));

#ifdef DEBUG
    if (g->cols <= 200 && g->rows <= 400) path_print_on_grid(p, g);
#endif

    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
    return EXIT_SUCCESS;

fail:
    if (p) path_free(p);
    if (ctx) grid_context_free(ctx);
    if (g) grid_free(g);
    return EXIT_FAILURE;
}
