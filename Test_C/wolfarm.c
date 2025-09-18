#include <stdio.h>
#include <stdlib.h>

#define SIZE 80     // number of cells in a row
#define STEPS 40    // number of generations

// Get new state of a cell based on rule
int applyRule(int left, int self, int right, int rule) {
    int pattern = (left << 2) | (self << 1) | right; // 3-bit number
    return (rule >> pattern) & 1; // extract corresponding bit
}

void printRow(int cells[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        printf(cells[i] ? "█" : " "); // █ = alive, space = dead
    }
    printf("\n");
}

int main() {
    int rule ; // Change this to test different rules (0–255)
    printf("Enter rule number (0-255): ");
    scanf("%d", &rule);
    if (rule < 0 || rule > 255) {
        printf("Error: Rule must be between 0 and 255\n");
        return 1;
    }

    int cells[SIZE] = {0};
    int newCells[SIZE] = {0};

    // Initial condition: single alive cell in the middle
    cells[SIZE / 2] = 1;

    for (int step = 0; step < STEPS; step++) {
        printRow(cells);

        // Compute next generation
        for (int i = 0; i < SIZE; i++) {
            int left  = (i == 0) ? 0 : cells[i - 1];
            int self  = cells[i];
            int right = (i == SIZE - 1) ? 0 : cells[i + 1];
            newCells[i] = applyRule(left, self, right, rule);
        }

        // Copy back
        for (int i = 0; i < SIZE; i++)
            cells[i] = newCells[i];
    }

    return 0;
}
