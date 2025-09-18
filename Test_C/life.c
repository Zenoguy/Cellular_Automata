#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for usleep

#define N 20   // Grid size (N x N)
#define STEPS 100  // Number of generations
#define DELAY 200000  // Microseconds between updates

// Print the grid
void printGrid(int grid[N][N]) {
    system("clear");  // Use "cls" on Windows
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%c ", grid[i][j] ? '#' : '.');
        }
        printf("\n");
    }
}

// Count live neighbors
int countNeighbors(int grid[N][N], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = (x + i + N) % N; // Wrap around edges
            int ny = (y + j + N) % N;
            count += grid[nx][ny];
        }
    }
    return count;
}

int main() {
    int grid[N][N] = {0};
    int newGrid[N][N] = {0};

    // Random initialization
    srand(42);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = rand() % 2;
        }
    }

    for (int step = 0; step < STEPS; step++) {
        printGrid(grid);
        usleep(DELAY);

        // Apply rules
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int neighbors = countNeighbors(grid, i, j);
                if (grid[i][j]) {
                    newGrid[i][j] = (neighbors == 2 || neighbors == 3);
                } else {
                    newGrid[i][j] = (neighbors == 3);
                }
            }
        }

        // Copy newGrid back into grid
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                grid[i][j] = newGrid[i][j];
            }
        }
    }

    return 0;
}
