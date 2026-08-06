#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#define STRING_VIEW_IMPL
#include "./lib/string_view.h"

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
static bool parse_graph() {
    char line[LINE_BUF_LENGTH];
    int32_t rows, cols;

    if (!fgets(line, sizeof(line), stdin)) return false;
    if (!parse_pair(sv_from_cstr(line), &rows, &cols)) return false;
    if (rows <= 0 || cols <= 0) return false;

    printf("Graph rows: %d, columns: %d\n", rows, cols);

    while (fgets(line, sizeof(line), stdin)) {
        int32_t cell_row, cell_col;
        if (!parse_pair(sv_from_cstr(line), &cell_row, &cell_col)) return false;
        if (cell_row < 0 || cell_col < 0 || cell_row >= rows || cell_col >= cols) return false;
        printf("    Cell (%d, %d) is blocked!\n", cell_row, cell_col);
    }
    return true;
}

int main() {
    if (!parse_graph()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
