#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"
#include "keyboard.h"
#include "memory.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define WINDOW_SCALE 1

struct CPU {
    struct memory *m;
    struct registers *r;
    struct stack *s;
    struct screen *scr;

    int *PC;
    int *I;
    unsigned char *delay_timer;
    unsigned char *sound_timer;

    unsigned char *draw_flag;
};

struct CPU *CPU_init(char *filename) {
    struct CPU *cpu = malloc(sizeof(struct CPU));
    cpu->m          = memory_init();
    if (memory_load_rom(cpu->m, filename)) {
        memory_free(cpu->m);
        free(cpu);
        return NULL;
    }

    cpu->r   = registers_init();
    cpu->s   = stack_init(1);
    cpu->scr = screen_init(filename);

    cpu->PC = calloc(2, sizeof(unsigned char));
    cpu->I  = calloc(2, sizeof(unsigned char));

    *(cpu->PC) = 0x200;

    cpu->delay_timer = calloc(1, sizeof(unsigned char));
    cpu->sound_timer = calloc(1, sizeof(unsigned char));
    cpu->draw_flag   = calloc(1, sizeof(unsigned char));

    srand(time(NULL));

    return cpu;
}

int get_bit(unsigned char byte, int index) {
    return byte & (0x80 >> index);
}

int cycle(struct CPU *cpu) {
    // fetch
    unsigned short opcode = memory_get_opcode(cpu->m, *(cpu->PC));

    unsigned short nnn = opcode & 0x0FFF;
    unsigned char n    = opcode & 0x000F;
    unsigned char x    = (opcode & 0x0F00) >> 8;
    unsigned char y    = (opcode & 0x00F0) >> 4;
    unsigned short kk  = opcode & 0x00FF;

    unsigned char value   = 0;
    unsigned char value_2 = 0;
    unsigned short sum    = 0;

    // decode & execute
    switch (opcode) {
    case 0x00E0:
        clear_screen(cpu->scr);
        *(cpu->PC) += 2;
        *(cpu->draw_flag) = 1;
        break;
    case 0x00EE:
        *(cpu->PC) = stack_pop(cpu->s);
        *(cpu->PC) += 2;
        break;
    }

    switch (opcode & 0xF000) {
    case 0x1000: *(cpu->PC) = nnn; break;
    case 0x2000:
        stack_push(cpu->s, *(cpu->PC));
        *(cpu->PC) = nnn;
        break;
    case 0x3000:
        value = registers_get(cpu->r, x);
        if (value == kk) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0x4000:
        value = registers_get(cpu->r, x);
        if (value != kk) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0x5000:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        if (value == value_2) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0x6000:
        registers_set(cpu->r, x, kk);
        *(cpu->PC) += 2;
        break;
    case 0x7000:
        value = registers_get(cpu->r, x);
        registers_set(cpu->r, x, value + kk);
        *(cpu->PC) += 2;
        break;
    }

    switch (opcode & 0xF00F) {
    case 0x8000:
        value = registers_get(cpu->r, y);
        registers_set(cpu->r, x, value);
        *(cpu->PC) += 2;
        break;
    case 0x8001:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        registers_set(cpu->r, x, value | value_2);
        *(cpu->PC) += 2;
        break;
    case 0x8002:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        registers_set(cpu->r, x, value & value_2);
        *(cpu->PC) += 2;
        break;
    case 0x8003:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        registers_set(cpu->r, x, value ^ value_2);
        *(cpu->PC) += 2;
        break;
    case 0x8004:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        sum     = value + value_2;
        registers_set(cpu->r, x, sum);
        registers_set(cpu->r, 0xF, sum > 0xFF ? 1 : 0);
        *(cpu->PC) += 2;
        break;
    case 0x8005:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        sum     = value - value_2;
        registers_set(cpu->r, x, sum);
        registers_set(cpu->r, 0xF, value > value_2 ? 1 : 0);
        *(cpu->PC) += 2;
        break;
    case 0x8006:
        value = registers_get(cpu->r, x);
        registers_set(cpu->r, x, value >> 1);
        registers_set(cpu->r, 0xF, value & 0x1 ? 1 : 0);
        *(cpu->PC) += 2;
        break;
    case 0x8007:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        registers_set(cpu->r, x, value_2 - value);
        registers_set(cpu->r, 0xF, value < value_2 ? 1 : 0);
        *(cpu->PC) += 2;
        break;
    case 0x800E:
        value = registers_get(cpu->r, x);
        registers_set(cpu->r, x, value << 1);
        registers_set(cpu->r, 0xF, value & 0x80 ? 1 : 0);
        *(cpu->PC) += 2;
        break;
    }

    switch (opcode & 0xF000) {
    case 0x9000:
        value   = registers_get(cpu->r, x);
        value_2 = registers_get(cpu->r, y);
        if (value != value_2) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0xA000:
        *(cpu->I) = nnn;
        *(cpu->PC) += 2;
        break;
    case 0xB000:
        value      = registers_get(cpu->r, 0x0);
        *(cpu->PC) = nnn + value;
        break;
    case 0xC000:
        unsigned char rand_val = rand() % 0xFF;
        registers_set(cpu->r, x, rand_val & kk);
        *(cpu->PC) += 2;
        break;
    case 0xD000:
        registers_set(cpu->r, 0xF, 0);

        for (int col = 0; col < n; col++) {
            unsigned char sprite = memory_get(cpu->m, *(cpu->I) + col);

            for (int row = 0; row < 8; row++) {
                int pixel = get_bit(sprite, row);
                if (pixel) {
                    int x_coord = registers_get(cpu->r, x) + row;
                    int y_coord = registers_get(cpu->r, y) + col;

                    if (x_coord >= SCREEN_WIDTH) { x_coord %= SCREEN_WIDTH; }
                    if (y_coord >= SCREEN_HEIGHT) { y_coord %= SCREEN_HEIGHT; }

                    if (get_pixel(cpu->scr, x_coord, y_coord)) {
                        registers_set(cpu->r, 0xF, 1);
                    }

                    set_pixel(cpu->scr, x_coord, y_coord);
                }
            }
        }

        *(cpu->draw_flag) = 1;
        *(cpu->PC) += 2;
        break;
    }

    switch (opcode & 0xF0FF) {
    case 0xE09E:
        value = registers_get(cpu->r, x);
        if (is_key_pressed(value)) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0xE0A1:
        value = registers_get(cpu->r, x);
        if (!is_key_pressed(value)) { *(cpu->PC) += 2; }
        *(cpu->PC) += 2;
        break;
    case 0xF007:
        registers_set(cpu->r, x, *(cpu->delay_timer));
        *(cpu->PC) += 2;
        break;
    case 0xF00A:
        unsigned char key = wait_key_pressed();
        registers_set(cpu->r, x, key);
        *(cpu->PC) += 2;
        break;
    case 0xF015:
        *(cpu->delay_timer) = registers_get(cpu->r, x);
        *(cpu->PC) += 2;
        break;
    case 0xF018:
        *(cpu->sound_timer) = registers_get(cpu->r, x);
        *(cpu->PC) += 2;
        break;
    case 0xF01E:
        *(cpu->I) += registers_get(cpu->r, x);
        *(cpu->PC) += 2;
        break;
    case 0xF029:
        *(cpu->I) = registers_get(cpu->r, x) * 5;
        *(cpu->PC) += 2;
        break;
    case 0xF033:
        value = registers_get(cpu->r, x);
        memory_set(cpu->m, *(cpu->I), value / 100);
        memory_set(cpu->m, *(cpu->I) + 1, value % 100 / 10);
        memory_set(cpu->m, *(cpu->I) + 2, value % 100 % 10);
        *(cpu->PC) += 2;
        break;
    case 0xF055:
        for (int i = 0; i <= x; i++) {
            value = registers_get(cpu->r, i);
            memory_set(cpu->m, *(cpu->I) + i, value);
        }
        *(cpu->PC) += 2;
        break;
    case 0xF065:
        for (int i = 0; i <= x; i++) {
            value = memory_get(cpu->m, *(cpu->I) + i);
            registers_set(cpu->r, i, value);
        }
        *(cpu->PC) += 2;
        break;
    }

    return 0;
}

int render(struct CPU *cpu) {
    if (*(cpu->draw_flag)) {
        *(cpu->draw_flag) = 0;

        draw(cpu->scr);
    }

    if (*(cpu->delay_timer) > 0) { *(cpu->delay_timer)--; }
    // 60 Hz
    SDL_Delay(16);

    return 0;
}

void CPU_free(struct CPU *cpu) {
    memory_free(cpu->m);
    registers_free(cpu->r);
    stack_cleanup(cpu->s);
    screen_destroy(cpu->scr);

    free(cpu->PC);
    free(cpu->I);
    free(cpu->delay_timer);
    free(cpu->sound_timer);
    free(cpu->draw_flag);
    free(cpu);
}
