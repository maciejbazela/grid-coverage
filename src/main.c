#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#define STRING_VIEW_IMPL
#include "./lib/string_view.h"

#define GRAPH_IMPL
#include "./lib/graph.h"

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
static graph* parse_graph() {

    char line[LINE_BUF_LENGTH];
    int32_t rows, cols;

    if (!fgets(line, sizeof(line), stdin)) return NULL;
    string_view sv_line = sv_from_cstr(line);
    sv_line = sv_trim(sv_line);
    if (!parse_pair(sv_line, &rows, &cols)) return NULL;
    if (rows <= 0 || cols <= 0) return NULL;

    printf("Graph rows: %d, columns: %d\n", rows, cols);
    graph* const g = graph_new(rows, cols);

    while (fgets(line, sizeof(line), stdin)) {
        sv_line = sv_from_cstr(line);
        sv_line = sv_trim(sv_line);
        int32_t cell_row, cell_col;
        if (!parse_pair(sv_line, &cell_row, &cell_col)) {
            printf("WARNING! Could not parse line: '"SV_PRINT_FMT"'\n", SV_PRINT_ARG(sv_line));
            continue;
        }
        if (cell_row < 0 || cell_row >= rows) {
            printf("WARNING! row: %d out of bounds [0, %d]\n", cell_row, rows);
        }
        if (cell_col < 0 || cell_col >= cols) {
            printf("WARNING! col: %d out of bounds [0, %d]\n", cell_col, cols);
        }
        printf("    Cell (%d, %d) is blocked!\n", cell_row, cell_col);
    }
    return g;
}

int main() {
    graph* g = parse_graph();
    if (g == NULL) return EXIT_FAILURE;

    graph_free(g);
    return EXIT_SUCCESS;
}
