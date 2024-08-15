#include <SDL2/SDL.h>

#include "keyboard.h"

SDL_Keycode convert_keycode_to_SDL(unsigned char hex_key) {
    switch (hex_key) {
    case 0x1: return SDLK_1;
    case 0x2: return SDLK_2;
    case 0x3: return SDLK_3;
    case 0xC: return SDLK_4;
    case 0x4: return SDLK_q;
    case 0x5: return SDLK_w;
    case 0x6: return SDLK_e;
    case 0xD: return SDLK_r;
    case 0x7: return SDLK_a;
    case 0x8: return SDLK_s;
    case 0x9: return SDLK_d;
    case 0xE: return SDLK_f;
    case 0xA: return SDLK_z;
    case 0x0: return SDLK_x;
    case 0xB: return SDLK_c;
    case 0xF: return SDLK_v;
    default: return SDLK_UNKNOWN;
    }
}

unsigned char convert_keycode_to_hex(SDL_Keycode sdl_key) {
    switch (sdl_key) {
    case SDLK_1: return 0x1;
    case SDLK_2: return 0x2;
    case SDLK_3: return 0x3;
    case SDLK_4: return 0xC;
    case SDLK_q: return 0x4;
    case SDLK_w: return 0x5;
    case SDLK_e: return 0x6;
    case SDLK_r: return 0xD;
    case SDLK_a: return 0x7;
    case SDLK_s: return 0x8;
    case SDLK_d: return 0x9;
    case SDLK_f: return 0xE;
    case SDLK_z: return 0xA;
    case SDLK_x: return 0x0;
    case SDLK_c: return 0xB;
    case SDLK_v: return 0xF;
    default: return 0;
    }
}

int is_key_pressed(unsigned char key) {
    SDL_Keycode converted_key       = convert_keycode_to_SDL(key);
    const Uint8 *current_key_states = SDL_GetKeyboardState(NULL);
    return current_key_states[SDL_GetScancodeFromKey(converted_key)];
}

unsigned char wait_key_pressed(void) {
    SDL_Event event;
    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            RUNNING = 0;
            return 0;
        }

        if (event.type == SDL_KEYUP) {
            if (convert_keycode_to_hex(event.key.keysym.sym) == 0) { continue; }
            return convert_keycode_to_hex(event.key.keysym.sym);
        }
    }

    return 0;
}
