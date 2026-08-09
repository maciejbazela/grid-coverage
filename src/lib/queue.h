#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef QUEUE_DEF
#define QUEUE_DEF static inline
#endif

typedef struct {
    size_t head, tail;
    size_t size, capacity;
    size_t elements[];
} queue;

QUEUE_DEF queue* queue_new(size_t capacity);
QUEUE_DEF void   queue_free(queue* q);
QUEUE_DEF bool   queue_push(queue* q, size_t val);
QUEUE_DEF size_t queue_pop(queue* q);
QUEUE_DEF size_t queue_front(const queue* q);
QUEUE_DEF void   queue_clear(queue* q);

#define queue_empty(q) ((q)->size == 0)
#define queue_full(q)  ((q)->size == (q)->capacity)
#define QUEUE_NONE     SIZE_MAX

#ifdef QUEUE_IMPL

QUEUE_DEF queue* queue_new(size_t capacity) {
    if (capacity == 0) return NULL;
    if (capacity > (SIZE_MAX - sizeof(queue)) / sizeof(size_t)) return NULL;

    queue* q = malloc(sizeof(queue) + capacity * sizeof(size_t));
    if (!q) return NULL;
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    q->capacity = capacity;

    return q;
}

QUEUE_DEF void queue_free(queue* q) {
    if (q == NULL) return;
    free(q);
}

QUEUE_DEF bool queue_push(queue* q, size_t val) {
    if (queue_full(q)) return false;
    q->elements[q->tail] = val;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    return true;
}

QUEUE_DEF size_t queue_pop(queue* q) {
    if (queue_empty(q)) return QUEUE_NONE;
    size_t val = q->elements[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    return val;
}

QUEUE_DEF size_t queue_front(const queue* q) {
    if (queue_empty(q)) return QUEUE_NONE;
    return q->elements[q->head];
}

QUEUE_DEF void queue_clear(queue* q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

#endif // QUEUE_IMPL
