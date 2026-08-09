#pragma once

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef PARSE_DEF
#define PARSE_DEF static inline
#endif

PARSE_DEF bool parse_size(const char* str, size_t* out);
PARSE_DEF bool parse_int(const char* str, int* out);
PARSE_DEF bool parse_double(const char* str, double* out);
PARSE_DEF bool parse_pair(const char* str, int* a, int* b);

#ifdef PARSE_IMPL

PARSE_DEF bool parse_size(const char* str, size_t* out) {
    char* end;
    errno = 0;
    long value = strtol(str, &end, 10);
    if (errno == ERANGE) return false;
    if (end == str || *end != '\0') return false;
    if (value <= 0) return false;
    *out = (size_t) value;
    return true;
}

PARSE_DEF bool parse_int(const char* str, int* out) {
    char* end;
    errno = 0;
    long value = strtol(str, &end, 10);
    if (errno == ERANGE) return false;
    if (end == str || *end != '\0') return false;
    if (value < INT32_MIN || value > INT32_MAX) return false;
    *out = (int) value;
    return true;
}

PARSE_DEF bool parse_double(const char* str, double* out) {
    char* end;
    errno = 0;
    double value = strtod(str, &end);
    if (errno == ERANGE) return false;
    if (end == str || *end != '\0') return false;
    *out = value;
    return true;
}

PARSE_DEF bool parse_pair(const char* str, int* a, int* b) {
    while (isspace((unsigned char) *str)) ++str;

    char* end;
    errno = 0;
    long va = strtol(str, &end, 10);
    if (errno == ERANGE) return false;
    if (end == str) return false;
    *a = (int) va;

    while (isspace((unsigned char) *end)) ++end;
    if (*end == '\0') return false;

    errno = 0;
    long vb = strtol(end, &end, 10);
    if (errno == ERANGE) return false;

    while (isspace((unsigned char) *end)) ++end;
    if (*end != '\0') return false;

    *b = (int) vb;
    return true;
}

#endif
