#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

#define BYTE 1

struct memory {
    unsigned char *data;
};

struct memory *memory_init(void) {
    struct memory *m = malloc(sizeof(struct memory));
    m->data          = calloc(4096, BYTE);

    return m;
}

int memory_load_rom(struct memory *m, char *file) {
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    size_t total_bytes = fread(m->data + 0x200, sizeof(unsigned char), 4096 - 0x200, fp);
    fclose(fp);

    if (total_bytes == 0) { return 1; }

    return 0;
}

unsigned char memory_get(struct memory *m, int index) {
    return m->data[index];
}

void memory_set(struct memory *m, int index, unsigned char value) {
    m->data[index] = value;
}

unsigned short memory_get_opcode(struct memory *m, int pc) {
    unsigned char byte_1 = memory_get(m, pc);
    unsigned char byte_2 = memory_get(m, pc + 1);

    return (unsigned short)(byte_1 << 8 | byte_2);
}

void memory_free(struct memory *m) {
    free(m->data);
    free(m);
}
