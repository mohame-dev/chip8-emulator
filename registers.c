#include <stdlib.h>

#include "registers.h"

#define BYTE 1

struct registers {
    unsigned char *data;
};

struct registers *registers_init(void) {
    struct registers *r = malloc(sizeof(struct registers));
    r->data             = calloc(16, BYTE);

    return r;
}

void registers_set(struct registers *r, unsigned char index, unsigned char value) {
    r->data[index] = value;
}

unsigned char registers_get(struct registers *r, unsigned short index) {
    return r->data[index];
}

void registers_free(struct registers *r) {
    free(r->data);
    free(r);
}
