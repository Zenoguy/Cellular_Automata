#include "ca.h"
#include <stdlib.h>
#include <time.h>

// Allocate 2D array
static int **alloc_grid(int w, int h) {
    int **grid = malloc(h * sizeof(int *));
    for (int i = 0; i < h; i++) {
        grid[i] = calloc(w, sizeof(int));
    }
    return grid;
}

static void free_grid(int **grid, int h) {
    for (int i = 0; i < h; i++) free(grid[i]);
    free(grid);
}

CA *ca_create(int width, int height) {
    CA *ca = malloc(sizeof(CA));
    ca->width = width;
    ca->height = height;
    ca->grid = alloc_grid(width, height);
    ca->next = alloc_grid(width, height);
    return ca;
}

void ca_free(CA *ca) {
    free_grid(ca->grid, ca->height);
    free_grid(ca->next, ca->height);
    free(ca);
}




int ca_count_neighbors(CA *ca, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + ca->width) % ca->width;
            int ny = (y + dy + ca->height) % ca->height;
            count += ca->grid[ny][nx];
        }
    }
    return count;
}

void ca_step(CA *ca, int (*rule)(int state, int neighbors)) {
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            int state = ca->grid[y][x];
            int neighbors = ca_count_neighbors(ca, x, y);
            ca->next[y][x] = rule(state, neighbors);
        }
    }
    // Swap grids
    int **tmp = ca->grid;
    ca->grid = ca->next;
    ca->next = tmp;
}


void ca_step_1d(CA *ca, int rule) {
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            int left  = ca->grid[y][(x - 1 + ca->width) % ca->width];
            int self  = ca->grid[y][x];
            int right = ca->grid[y][(x + 1) % ca->width];
            int idx = (left << 2) | (self << 1) | right;
            ca->next[y][x] = (rule >> idx) & 1;
        }
    }
    int **tmp = ca->grid;
    ca->grid = ca->next;
    ca->next = tmp;
}

void ca_randomize(CA *ca, unsigned seed) {
    srand(seed);
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            ca->grid[y][x] = rand() % 2;
        }
    }
}

void ca_clear(CA *ca) {
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            ca->grid[y][x] = 0;
        }
    }
}