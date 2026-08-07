#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "./lib/debug.h"

#define STRING_VIEW_IMPL
#include "./lib/string_view.h"

#define GRID_IMPL
#include "./lib/grid.h"

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

#define LINE_BUF_LENGTH 256

// Expected format:
// ROWS COLS
// CELL1_ROW CELL1_COL
// CELL2_ROW CELL2_COL
// ...
//
// Where (CELL1_ROW, CELL1_COL) indicated that a cell is blocked
static grid* parse_grid_stdin() {
    char line[LINE_BUF_LENGTH];
    int32_t rows, cols;

    if (!fgets(line, sizeof(line), stdin)) return NULL;
    string_view sv_line = sv_trim(sv_from_cstr(line));

    if (!parse_pair(sv_line, &rows, &cols)) return NULL;
    if (rows <= 0 || cols <= 0) return NULL;

    DEBUG_PRINT("Grid rows: %d, columns: %d\n", rows, cols);
    grid* const g = grid_new(rows, cols);

    while (fgets(line, sizeof(line), stdin)) {
        sv_line = sv_trim(sv_from_cstr(line));
        int32_t cell_row, cell_col;
        if (!parse_pair(sv_line, &cell_row, &cell_col)) {
            DEBUG_PRINT("WARNING! Could not parse line: '"SV_PRINT_FMT"'\n", SV_PRINT_ARG(sv_line));
            continue;
        }
        if (cell_row < 0 || cell_row >= rows) {
            DEBUG_PRINT("WARNING! row: %d out of bounds [0, %d]\n", cell_row, rows);
        }
        if (cell_col < 0 || cell_col >= cols) {
            DEBUG_PRINT("WARNING! col: %d out of bounds [0, %d]\n", cell_col, cols);
        }
        DEBUG_PRINT("    Cell (%d, %d) is blocked!\n", cell_row, cell_col);
        grid_set(g, cell_row, cell_col);
    }
    return g;
}

static size_t read_line(char buf[LINE_BUF_LENGTH], FILE* f) {
    size_t len = 0;
    for (;;) {
        if (fgets(buf, LINE_BUF_LENGTH, f) == NULL) {
            if (len == 0) return (size_t) -1; // Error or empty file
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
            if (col == 0) return (size_t) -1;
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
                if (!grid_set(g, row, col)) return (size_t) -1;
            }
            else if (c != '.')
                return (size_t) -1;
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
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return NULL;
    }

    char   buf[LINE_BUF_LENGTH];
    size_t rows = 0, cols = 0;

    for (;;) {
        size_t len = read_line(buf, f);
        if (len == (size_t) -1) break;
        if (rows == 0) {
            cols = len;
            if (cols == 0) { // Empty first row
                fclose(f);
                return NULL;
            }
        }
        else if (len != cols) {
            fclose(f);
            return NULL;
        }
        rows++;
    }
    if (ferror(f) || rows == 0) {
        fclose(f);
        return NULL;
    }

    rewind(f);

    grid* const g = grid_new(rows, cols);
    if (g == NULL) {
        fclose(f);
        return NULL;
    }

    for (size_t row = 0; row < rows; ++row) {
        size_t len = fill_grid_row(g, row, f);
        if (len != cols) {
            grid_free(g);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return g;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        grid* g = parse_grid_stdin();
        if (g == NULL) return EXIT_FAILURE;
        grid_print(g);
        grid_free(g);
    }
    else {
        grid* g = parse_grid_file(argv[1]);
        if (g == NULL) return EXIT_FAILURE;
        grid_print(g);
        grid_free(g);
    }
    return EXIT_SUCCESS;
}
