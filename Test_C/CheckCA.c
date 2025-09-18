#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CELLS 10
#define MAX_STEPS 50

int CELLS;

typedef struct {
    int rules[MAX_CELLS];
    int boundary_type;
} CA_Config;

long long state_to_int(int state[]) {
    long long result = 0;
    for (int i = 0; i < CELLS; i++) {
        result = result * 2 + state[i];
    }
    return result;
}

void int_to_state(long long num, int state[]) {
    for (int i = CELLS - 1; i >= 0; i--) {
        state[i] = num % 2;
        num /= 2;
    }
}

int get_rule_value(int left, int center, int right, int rule) {
    int pattern = left * 4 + center * 2 + right;
    return (rule >> pattern) & 1;
}

void evolve_step(int current[], int next[], CA_Config* config) {
    for (int i = 0; i < CELLS; i++) {
        int left, right;
        if (config->boundary_type == 0) {
            left = (i == 0) ? 0 : current[i-1];
            right = (i == CELLS-1) ? 0 : current[i+1];
        } else {
            left = current[(i-1+CELLS) % CELLS];
            right = current[(i+1) % CELLS];
        }
        next[i] = get_rule_value(left, current[i], right, config->rules[i]);
    }
}

int find_cycle(long long start_state, CA_Config* config, int* cycle_length) {
    int current[MAX_CELLS], next[MAX_CELLS];
    long long visited[MAX_STEPS];
    int step = 0;
    int_to_state(start_state, current);
    
    while (step < MAX_STEPS) {
        long long current_int = state_to_int(current);
        for (int i = 0; i < step; i++) {
            if (visited[i] == current_int) {
                *cycle_length = step - i;
                return i;
            }
        }
        visited[step] = current_int;
        evolve_step(current, next, config);
        for (int i = 0; i < CELLS; i++) {
            current[i] = next[i];
        }
        step++;
    }
    *cycle_length = 0;
    return -1;
}

// Function to get detailed cycle statistics
void get_detailed_cycle_stats(CA_Config* config, int* total_cycles, int* max_cycle_length, int* cycle_counts) {
    int max_states = (int)pow(2, CELLS);
    int* visited = (int*)calloc(max_states, sizeof(int));
    
    *total_cycles = 0;
    *max_cycle_length = 0;
    for (int i = 0; i < max_states; i++) {
        cycle_counts[i] = 0;
    }
    
    for (long long start = 0; start < max_states; start++) {
        if (visited[start]) continue;
        
        int cycle_length;
        int cycle_start_step = find_cycle(start, config, &cycle_length);
        
        if (cycle_length > 0) {
            *total_cycles += 1;
            if (cycle_length > *max_cycle_length) {
                *max_cycle_length = cycle_length;
            }
            if (cycle_length < max_states) {
                cycle_counts[cycle_length]++;
            }
            
            int current[MAX_CELLS], next[MAX_CELLS];
            int_to_state(start, current);
            for (int step = 0; step < cycle_start_step + cycle_length; step++) {
                long long state_int = state_to_int(current);
                if (state_int < max_states) {
                    visited[state_int] = 1;
                }
                evolve_step(current, next, config);
                for (int i = 0; i < CELLS; i++) {
                    current[i] = next[i];
                }
            }
        }
    }
    free(visited);
}

int main() {
    CA_Config config;
    
    printf("Dynamic Cellular Automata Cycle Summary\n");
    printf("=======================================\n\n");
    
    printf("Enter number of cells (1-%d): ", MAX_CELLS);
    scanf("%d", &CELLS);
    
    if (CELLS < 1 || CELLS > MAX_CELLS) {
        printf("Error: Number of cells must be between 1 and %d\n", MAX_CELLS);
        return 1;
    }
    
    printf("\nBoundary condition:\n");
    printf("0 = Null boundary (edges are 0)\n");
    printf("1 = Periodic boundary (wraps around)\n");
    printf("Enter choice (0 or 1): ");
    scanf("%d", &config.boundary_type);
    
    if (config.boundary_type != 0 && config.boundary_type != 1) {
        printf("Warning: Using null boundary as default\n");
        config.boundary_type = 0;
    }
    
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
    
    int total_combinations = (int)pow(2, CELLS);
    
    for (int combo_num = 0; combo_num < total_combinations; combo_num++) {
        for (int i = 0; i < CELLS; i++) {
            if ((combo_num >> i) & 1) {
                config.rules[i] = 150;
            } else {
                config.rules[i] = 90;
            }
        }
        
        int total_cycles = 0;
        int max_cycle_length = 0;
        int* cycle_counts = (int*)calloc(max_states, sizeof(int));
        
        get_detailed_cycle_stats(&config, &total_cycles, &max_cycle_length, cycle_counts);
        
        printf("Configuration number: %d\n", combo_num + 1);
        printf("Rules [");
        for (int i = 0; i < CELLS; i++) {
            printf("%d%s", config.rules[i], (i == CELLS - 1) ? "" : ", ");
        }
        printf("]: Total Cycles = %d, {", total_cycles);
        
        int first_entry = 1;
        for (int len = 1; len < max_states; len++) {
            if (cycle_counts[len] > 0) {
                if (!first_entry) {
                    printf(", ");
                }
                printf("%d:%d", cycle_counts[len], len);
                first_entry = 0;
            }
        }
        
        printf("} - Max Cycle Length = %d\n\n", max_cycle_length);
        
        free(cycle_counts);
    }
    
    return 0;
}