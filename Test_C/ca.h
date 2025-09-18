#ifndef CA_H
#define CA_H

typedef struct {
    int width, height;
    int **grid, **next;
} CA;

CA *ca_create(int width, int height);
void ca_free(CA *ca);
int ca_count_neighbors(CA *ca, int x, int y);
void ca_step(CA *ca, int (*rule)(int, int));

// 👇 Add this line
void ca_step_1d(CA *ca, int rule);

void ca_randomize(CA *ca, unsigned seed);
void ca_clear(CA *ca);

#endif
