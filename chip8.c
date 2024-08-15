#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "memory.h"
#include "registers.h"
#include "stack.h"

int RUNNING = 1;

void run(struct CPU *cpu) {
    SDL_Event event;
    while (RUNNING) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { RUNNING = 0; }
        }

        cycle(cpu);
        render(cpu);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("No file given\n");
        return 1;
    }

    struct CPU *cpu = CPU_init(argv[1]);
    if (!cpu) { return 1; }

    run(cpu);
    return 0;
}
