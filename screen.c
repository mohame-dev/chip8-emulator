#include <SDL2/SDL.h>
// #include <math.h>

#include "keyboard.h"
#include "screen.h"

#define WIDTH 64
#define HEIGHT 32
#define SCALE 20
#define COLOR_DRAWN 0xFFFFFFFF


struct screen {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 *pixels;
    int *pitch;
};

struct screen *screen_init(char *filename) {
    struct screen *s = malloc(sizeof(struct screen));
    s->window = SDL_CreateWindow(filename, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 WIDTH * SCALE, HEIGHT * SCALE, 0);
    s->renderer = SDL_CreateRenderer(s->window, -1, 0);
    SDL_RenderSetLogicalSize(s->renderer, WIDTH * SCALE, HEIGHT * SCALE);
    s->texture = SDL_CreateTexture(s->renderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    s->pixels = calloc(WIDTH * HEIGHT, sizeof(Uint32));
    s->pitch  = calloc(1, sizeof(int));

    return s;
}

void set_pixel(struct screen *s, int x, int y) {
    s->pixels[y * WIDTH + x] ^= COLOR_DRAWN;
}

int get_pixel(struct screen *s, int x, int y) {
    return s->pixels[y * WIDTH + x] ? 1 : 0;
}

void draw(struct screen *s) {
    SDL_UpdateTexture(s->texture, NULL, s->pixels, WIDTH * sizeof(Uint32));
    SDL_RenderClear(s->renderer);
    SDL_RenderCopy(s->renderer, s->texture, NULL, NULL);
    SDL_RenderPresent(s->renderer);
}

void clear_screen(struct screen *s) {
    SDL_memset(s->pixels, 0, WIDTH * HEIGHT * sizeof(Uint32));
}

void screen_destroy(struct screen *s) {
    SDL_DestroyTexture(s->texture);
    SDL_DestroyRenderer(s->renderer);
    SDL_DestroyWindow(s->window);
    free(s->pixels);
    free(s->pitch);
    free(s);
}
