#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef SV_DEF
#define SV_DEF static inline
#endif

#define SV_PRINT_FMT     "%.*s"
#define SV_PRINT_ARG(sv) (int) sv.size, sv.data

typedef struct {
    size_t size;
    const char* data;
} string_view;

SV_DEF string_view sv_from_cstr(const char* str);
SV_DEF string_view sv_left_trim(string_view sv);
SV_DEF string_view sv_right_trim(string_view sv);
SV_DEF string_view sv_trim(string_view sv);
SV_DEF string_view sv_get_word(string_view sv);
SV_DEF string_view sv_skip_word(string_view sv);
SV_DEF bool sv_copy_cstr(char* dst, string_view sv, size_t dst_size);

// TODO: add tests!

#ifdef STRING_VIEW_IMPL

SV_DEF string_view sv_from_cstr(const char* str) {
    return (string_view) { .size = strlen(str), .data = str };
}

SV_DEF string_view sv_left_trim(string_view sv) {
    while (sv.size > 0 && isspace(sv.data[0])) {
        sv.data++;
        sv.size--;
    }
    return sv;
}

SV_DEF string_view sv_right_trim(string_view sv) {
    while (sv.size > 0 && isspace(sv.data[sv.size - 1]))
        sv.size--;
    return sv;
}

SV_DEF string_view sv_trim(string_view sv) {
    return sv_right_trim(sv_left_trim(sv));
}

SV_DEF string_view sv_get_word(string_view sv) {
    size_t word_len = 0;
    while (word_len < sv.size && !isspace(sv.data[word_len])) {
        sv.size--;
        word_len++;
    }
    return (string_view) { .size = word_len, .data = sv.data };
}

SV_DEF string_view sv_skip_word(string_view sv) {
    while (sv.size > 0 && !isspace(sv.data[0])) {
        sv.data++;
        sv.size--;
    }
    return sv;
}

SV_DEF bool sv_copy_cstr(char* dst, string_view sv, size_t dst_size) {
    if (sv.size + 1 > dst_size) return false;
    memcpy(dst, sv.data, sv.size);
    dst[sv.size] = '\0';
    return true;
}

#endif // STRING_VIEW_IMPL
