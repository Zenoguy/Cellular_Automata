#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CELLS 10 // Maximum number of cells supported
#define MAX_STEPS 50 // Maximum steps to detect cycles

// Global variable for number of cells
int CELLS;

// Structure to represent a CA configuration
typedef struct {
    int rules[MAX_CELLS];
    int boundary_type; // 0 = null, 1 = periodic
} CA_Config;

//---
/*
 * Function Prototypes
 * These forward declarations make the code easier to read by listing the
 * functions at the top, even if their full definitions appear later.
 */
long long state_to_int(int state[]);
void int_to_state(long long num, int state[]);
int get_rule_value(int left, int center, int right, int rule);
void evolve_step(int current[], int next[], CA_Config* config);
int find_cycle(long long start_state, CA_Config* config, int* cycle_length);
void print_state(long long state);
void draw_state_diagram(CA_Config* config);

//---
/*
 * State Conversion Functions
 * These functions handle the conversion between a binary array representation
 * of a cellular automaton state and a single integer for easy storage and comparison.
 */

/**
 * @brief Converts a binary array state to a single long long integer.
 *
 * This is done by treating the binary array as a base-2 number.
 *
 * @param state The binary array representing the CA state.
 * @return The corresponding integer value.
 */
long long state_to_int(int state[]) {
    long long result = 0;
    for (int i = 0; i < CELLS; i++) {
        result = result * 2 + state[i];
    }
    return result;
}

/**
 * @brief Converts a long long integer back to a binary array state.
 *
 * This is done by repeatedly taking the number modulo 2.
 *
 * @param num The integer value of the state.
 * @param state The binary array to store the result.
 */
void int_to_state(long long num, int state[]) {
    for (int i = CELLS - 1; i >= 0; i--) {
        state[i] = num % 2;
        num /= 2;
    }
}

//---
/*
 * Cellular Automaton Core Logic
 * These functions handle the mechanics of the cellular automaton, including
 * applying evolution rules and managing boundaries.
 */

/**
 * @brief Gets the output value for a 3-neighbor pattern based on a rule number.
 *
 * The rule number (0-255) is a lookup table for all 8 possible neighbor patterns.
 * This function extracts the correct output for a given pattern.
 *
 * @param left The state of the left neighbor (0 or 1).
 * @param center The state of the center cell (0 or 1).
 * @param right The state of the right neighbor (0 or 1).
 * @param rule The rule number (0-255).
 * @return The new state of the center cell (0 or 1).
 */
int get_rule_value(int left, int center, int right, int rule) {
    // The pattern is a 3-bit binary number (e.g., 110 = 6)
    int pattern = left * 4 + center * 2 + right;
    // We bit-shift the rule to the right by the pattern number,
    // and then use bitwise AND with 1 to get the value of that specific bit.
    return (rule >> pattern) & 1;
}

/**
 * @brief Applies one step of cellular automaton evolution.
 *
 * @param current The current state array.
 * @param next The array to store the new state.
 * @param config The CA configuration (rules and boundary type).
 */
void evolve_step(int current[], int next[], CA_Config* config) {
    for (int i = 0; i < CELLS; i++) {
        int left, right;

        // Determine the neighbors based on the boundary type
        if (config->boundary_type == 0) { // Null boundary
            // Edges are treated as 0s
            left = (i == 0) ? 0 : current[i - 1];
            right = (i == CELLS - 1) ? 0 : current[i + 1];
        } else { // Periodic boundary
            // The left neighbor of the first cell is the last cell, and vice-versa.
            left = current[(i - 1 + CELLS) % CELLS];
            right = current[(i + 1) % CELLS];
        }

        // Apply the rule to the current cell based on its neighbors
        next[i] = get_rule_value(left, current[i], right, config->rules[i]);
    }
}

//---
/*
 * Cycle Detection and Analysis Functions
 * These functions are responsible for finding cycles in the state space
 * and generating a visual representation of the state transitions.
 */

/**
 * @brief Finds a cycle starting from a given state.
 *
 * This function uses a Floyd's cycle-finding algorithm-like approach by storing
 * visited states to detect repetitions.
 *
 * @param start_state The integer value of the starting state.
 * @param config The CA configuration.
 * @param cycle_length A pointer to store the length of the found cycle.
 * @return The number of steps until the cycle begins, or -1 if no cycle is found.
 */
int find_cycle(long long start_state, CA_Config* config, int* cycle_length) {
    int current[MAX_CELLS], next[MAX_CELLS];
    long long visited[MAX_STEPS];
    int step = 0;

    int_to_state(start_state, current);

    while (step < MAX_STEPS) {
        long long current_int = state_to_int(current);

        // Check if we've seen this state before
        for (int i = 0; i < step; i++) {
            if (visited[i] == current_int) {
                *cycle_length = step - i; // Calculate the length of the cycle
                return i;                 // Return the step where the cycle begins
            }
        }

        visited[step] = current_int;
        evolve_step(current, next, config);

        // Copy the new state back to the current state for the next step
        for (int i = 0; i < CELLS; i++) {
            current[i] = next[i];
        }

        step++;
    }

    *cycle_length = 0;
    return -1; // No cycle found within MAX_STEPS
}

/**
 * @brief Prints a state in a binary format.
 *
 * @param state The integer value of the state.
 */
void print_state(long long state) {
    for (int i = CELLS - 1; i >= 0; i--) {
        printf("%d", (int)((state >> i) & 1));
    }
}

/**
 * @brief Draws a state transition diagram for a given CA configuration.
 *
 * This function analyzes all possible initial states and finds their cycles.
 * It also provides a summary of cycle lengths.
 *
 * @param config The CA configuration.
 */
void draw_state_diagram(CA_Config* config) {
    int max_states = (int)pow(2, CELLS);
    // 'visited' array tracks which states have been analyzed to avoid redundant work.
    int* visited = (int*)calloc(max_states, sizeof(int));
    // 'cycle_counts' array tallies cycles of different lengths.
    int* cycle_counts = (int*)calloc(max_states, sizeof(int));

    printf("\n=== STATE TRANSITION DIAGRAM ===\n");
    printf("Number of cells: %d\n", CELLS);
    printf("Rules: ");
    for (int i = 0; i < CELLS; i++) {
        printf("%d ", config->rules[i]);
    }
    printf("\nBoundary: %s\n\n", config->boundary_type ? "Periodic" : "Null");

    // Analyze all possible starting states
    for (long long start = 0; start < max_states; start++) {
        // Skip states that have already been visited as part of another path.
        if (visited[start]) continue;

        int cycle_length;
        int cycle_start_step = find_cycle(start, config, &cycle_length);

        // If a cycle is found
        if (cycle_length > 0) {
            // Trace the path and mark all states on it as visited
            int current[MAX_CELLS];
            int_to_state(start, current);

            printf("Starting from ");
            print_state(start);
            printf(": ");

            // Trace and print the path leading to and through the cycle
            for (int step = 0; step <= cycle_start_step + cycle_length; step++) {
                long long state_int = state_to_int(current);

                if (step > 0) printf(" -> ");
                print_state(state_int);

                if (step == cycle_start_step && cycle_length > 1) {
                    printf(" [cycle starts]");
                }

                if (state_int < max_states) {
                    visited[state_int] = 1;
                }

                if (step < cycle_start_step + cycle_length) {
                    int next[MAX_CELLS];
                    evolve_step(current, next, config);
                    for (int i = 0; i < CELLS; i++) {
                        current[i] = next[i];
                    }
                }
            }

            printf("\n  -> Cycle length: %d\n\n", cycle_length);
            if (cycle_length < max_states) {
                cycle_counts[cycle_length]++;
            }
        }
    }

    // Summary of cycle lengths
    printf("=== CYCLE SUMMARY ===\n");
    for (int len = 1; len < max_states; len++) {
        if (cycle_counts[len] > 0) {
            printf("%d cycle(s) of length %d\n", cycle_counts[len], len);
        }
    }

    // Free dynamically allocated memory
    free(visited);
    free(cycle_counts);
}

//---
/*
 * Main Function
 * This is the entry point of the program. It handles user input for configuration
 * and orchestrates the analysis of different rule combinations.
 */
int main() {
    CA_Config config;

    printf("Dynamic Cellular Automata Cycle Analyzer\n");
    printf("=======================================\n\n");

    // Get number of cells from the user
    printf("Enter number of cells (1-%d): ", MAX_CELLS);
    scanf("%d", &CELLS);

    if (CELLS < 1 || CELLS > MAX_CELLS) {
        printf("Error: Number of cells must be between 1 and %d\n", MAX_CELLS);
        return 1;
    }

    // Get boundary condition from the user
    printf("\nBoundary condition:\n");
    printf("0 = Null boundary (edges are 0)\n");
    printf("1 = Periodic boundary (wraps around)\n");
    printf("Enter choice (0 or 1): ");
    scanf("%d", &config.boundary_type);

    if (config.boundary_type != 0 && config.boundary_type != 1) {
        printf("Warning: Using null boundary as default\n");
        config.boundary_type = 0;
    }

    // Check if the number of states is too large for the analysis to be quick
    int max_states = (int)pow(2, CELLS);
    if (max_states > 1024) {
        printf("\nWarning: %d cells creates %d possible states. This may take a while...\n",
               CELLS, max_states);
        printf("Continue? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            printf("Aborted.\n");
            return 0;
        }
    }

    // Iterate through all 2^CELLS combinations of Rule 150 and 90
    int total_combinations = (int)pow(2, CELLS);

    for (int combo_num = 0; combo_num < total_combinations; combo_num++) {

        // Assign rules based on the current combination number using a bitmask.
        // If the i-th bit of combo_num is 1, the rule is 150; otherwise, it's 90.
        for (int i = 0; i < CELLS; i++) {
            if ((combo_num >> i) & 1) {
                config.rules[i] = 150;
            } else {
                config.rules[i] = 90;
            }
        }

        // Print the current configuration being analyzed
        printf("\n--- Analyzing Configuration %d ---\n", combo_num + 1);
        printf("Rules: ");
        for (int i = 0; i < CELLS; i++) {
            printf("%d ", config.rules[i]);
        }
        printf("\n");

        // Draw the state transition diagram for this specific rule configuration
        draw_state_diagram(&config);
    }

    return 0;
}