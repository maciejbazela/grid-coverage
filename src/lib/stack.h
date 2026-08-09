#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef STACK_DEF
#define STACK_DEF static inline
#endif

typedef struct {
    size_t size, capacity;
    size_t elements[];
} stack;

STACK_DEF stack* stack_new(size_t capacity);
STACK_DEF void   stack_free(stack* s);
STACK_DEF bool   stack_push(stack* s, size_t val);
STACK_DEF size_t stack_pop(stack* s);
STACK_DEF size_t stack_top(const stack* s);
STACK_DEF void   stack_clear(stack* s);

#define stack_empty(s) ((s)->size == 0)
#define stack_full(s)  ((s)->size == (s)->capacity)
#define STACK_NONE     SIZE_MAX

#ifdef STACK_IMPL

STACK_DEF stack* stack_new(size_t capacity) {
    if (capacity == 0) return NULL;
    if (capacity > (SIZE_MAX - sizeof(stack)) / sizeof(size_t)) return NULL;

    stack* s = malloc(sizeof(stack) + capacity * sizeof(size_t));
    if (s == NULL) return NULL;
    s->size = 0;
    s->capacity = capacity;

    return s;
}

STACK_DEF void stack_free(stack* s) {
    if (s == NULL) return;
    free(s);
}

STACK_DEF bool stack_push(stack* s, size_t val) {
    if (stack_full(s)) return false;
    s->elements[s->size++] = val;
    return true;
}

STACK_DEF size_t stack_pop(stack* s) {
    if (stack_empty(s)) return STACK_NONE;
    return s->elements[--s->size];
}

STACK_DEF size_t stack_top(const stack* s) {
    if (stack_empty(s)) return STACK_NONE;
    return s->elements[s->size - 1];
}

STACK_DEF void stack_clear(stack* s) {
    s->size = 0;
}

#endif // STACK_IMPL
