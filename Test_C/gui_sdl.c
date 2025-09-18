// gui_sdl.c
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ca.h"

#define CELL_SIZE 8
#define GRID_W 100
#define GRID_H 80
#define WINDOW_TITLE "Cellular Automata (SDL2)"

// Example rule from rules.c
extern int conway_rule(int state, int neighbors);

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    int window_w = GRID_W * CELL_SIZE;
    int window_h = GRID_H * CELL_SIZE;
    SDL_Window *win = SDL_CreateWindow(WINDOW_TITLE,
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       window_w, window_h,
                                       0);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    CA *ca = ca_create(GRID_W, GRID_H);
    ca_randomize(ca, (unsigned)time(NULL));

    int running = 1;
    int paused = 0;            // start running; set to 1 if you prefer start paused
    Uint32 last_update = SDL_GetTicks();
    int delay_ms = 100;        // simulation delay

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_SPACE: paused = !paused; break;
                        case SDLK_c: ca_clear(ca); break;
                        case SDLK_r: ca_randomize(ca, 0); break;
                        case SDLK_n: if (paused) ca_step(ca, conway_rule); break;
                        case SDLK_UP: if (delay_ms > 10) delay_ms -= 10; break;
                        case SDLK_DOWN: delay_ms += 10; break;
                        case SDLK_ESCAPE: running = 0; break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    int gx = e.button.x / CELL_SIZE;
                    int gy = e.button.y / CELL_SIZE;
                    if (gx >= 0 && gx < ca->width && gy >= 0 && gy < ca->height) {
                        ca->grid[gy][gx] = !ca->grid[gy][gx]; // toggle
                    }
                    break;
                }
                case SDL_MOUSEMOTION:
                    // draw while dragging left button
                    if (e.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                        int gx = e.motion.x / CELL_SIZE;
                        int gy = e.motion.y / CELL_SIZE;
                        if (gx >= 0 && gx < ca->width && gy >= 0 && gy < ca->height) {
                            ca->grid[gy][gx] = 1;
                        }
                    }
                    break;
            }
        }

        Uint32 now = SDL_GetTicks();
        if (!paused && now - last_update >= (Uint32)delay_ms) {
            ca_step_1d(ca, 150); 
            last_update = now;
        }

        // render background
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // draw cells
        SDL_Rect cell = {0,0, CELL_SIZE, CELL_SIZE};
        for (int y = 0; y < ca->height; y++) {
            for (int x = 0; x < ca->width; x++) {
                if (ca->grid[y][x]) {
                    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                    cell.x = x * CELL_SIZE;
                    cell.y = y * CELL_SIZE;
                    SDL_RenderFillRect(ren, &cell);
                }
            }
        }

        // optional grid lines (light gray)
        SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
        for (int x = 0; x <= ca->width; x++) {
            SDL_RenderDrawLine(ren, x * CELL_SIZE, 0, x * CELL_SIZE, ca->height * CELL_SIZE);
        }
        for (int y = 0; y <= ca->height; y++) {
            SDL_RenderDrawLine(ren, 0, y * CELL_SIZE, ca->width * CELL_SIZE, y * CELL_SIZE);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(10); // small sleep to avoid maxing CPU
    }

    ca_free(ca);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
