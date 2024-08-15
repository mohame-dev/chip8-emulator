#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

struct stack {
    int *data;
    size_t size;
    int pushes;
    int pops;
    size_t capacity;
    size_t max_elements;
};

struct stack *stack_init(size_t capacity) {
    if (!capacity) capacity = 1;

    struct stack *s = malloc(sizeof(struct stack));
    if (!s) return NULL;
    s->data = malloc(capacity * sizeof(int));
    if (!s->data) {
        free(s);
        return NULL;
    }
    s->size         = 0;
    s->pushes       = 0;
    s->pops         = 0;
    s->capacity     = capacity;
    s->max_elements = 0;
    return s;
}

void stack_cleanup(struct stack *s) {
    free(s->data);
    free(s);
}

void stack_stats(const struct stack *s) {
    fprintf(stderr, "stats %d %d %ld\n", s->pushes, s->pops, s->max_elements);
}

int stack_push(struct stack *s, int c) {
    if (!s || c < 0) return 1;

    /* resize the stack with realloc when the stack is full*/
    if (s->size == s->capacity) {
        s->capacity += 5000;
        s->data = realloc(s->data, s->capacity * sizeof(int));
        if (!s->data) return 1;
    }

    s->data[s->size] = c;
    if (s->size++ == s->max_elements) s->max_elements++;
    s->pushes++;
    return 0;
}

int stack_pop(struct stack *s) {
    if (!s || stack_empty(s)) return -1;
    s->size--;
    s->pops++;
    return s->data[s->size];
}

int stack_peek(const struct stack *s) {
    if (!s || stack_empty(s)) return -1;
    return s->data[s->size - 1];
}

int stack_empty(const struct stack *s) {
    if (!s) return -1;
    if (stack_size(s) > 0) return 0;
    if (!stack_size(s)) return 1;
    return -1;
}

size_t stack_size(const struct stack *s) {
    return s->size;
}
