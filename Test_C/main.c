#include "ca.h"
#include <stdio.h>
#include <unistd.h>

// Import rule
extern int conway_rule(int state, int neighbors);

void print_grid(CA *ca) {
    printf("\033[H\033[2J"); // clear
    for (int y = 0; y < ca->height; y++) {
        for (int x = 0; x < ca->width; x++) {
            printf("%c ", ca->grid[y][x] ? '#' : '.');
        }
        printf("\n");
    }
}

int main() {
    CA *ca = ca_create(20, 20);
    ca_randomize(ca, 42);

    for (int step = 0; step < 100; step++) {
        print_grid(ca);
        usleep(200000);
        ca_step(ca, conway_rule);
    }

    ca_free(ca);
    return 0;
}
