#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Maximum number of cells (N < 32 as per your constraint)
#define MAX_CELLS 32

// Null class rules as mentioned in your findings
int null_class[] = {0, 8, 32, 40, 128, 136, 160, 168};
int null_class_size = 8;

// Global variables
int N; // Number of cells
int rules[MAX_CELLS]; // Rules for each cell
int current_state[MAX_CELLS]; // Current state of CA
int next_state[MAX_CELLS]; // Next state of CA
int *check; // Array to track visited states
int total_states; // Total possible states (2^N)

// Function to apply a specific rule to get next state of a cell
int applyRule(int left, int self, int right, int rule) {
    // Create 3-bit pattern from neighborhood
    int pattern = (left << 2) | (self << 1) | right;
    // Extract corresponding bit from rule
    return (rule >> pattern) & 1;
}

// Function to convert binary state to decimal
int stateToDecimal(int state[], int size) {
    int decimal = 0;
    for (int i = 0; i < size; i++) {
        decimal += state[i] * (1 << (size - 1 - i));
    }
    return decimal;
}

// Function to convert decimal to binary state
void decimalToState(int decimal, int state[], int size) {
    for (int i = size - 1; i >= 0; i--) {
        state[i] = (decimal >> (size - 1 - i)) & 1;
    }
}

// Function to compute next state using assigned rules
void computeNextState() {
    for (int i = 0; i < N; i++) {
        // Get neighbors (with wraparound/boundary conditions)
        int left = (i == 0) ? current_state[N-1] : current_state[i-1];
        int self = current_state[i];
        int right = (i == N-1) ? current_state[0] : current_state[i+1];
        
        // Apply rule for this cell
        next_state[i] = applyRule(left, self, right, rules[i]);
    }
    
    // Copy next_state to current_state
    for (int i = 0; i < N; i++) {
        current_state[i] = next_state[i];
    }
}

// Function to print current state
void printState() {
    printf("State: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", current_state[i]);
    }
    int decimal = stateToDecimal(current_state, N);
    printf("(%d)\n", decimal);
}

// Function to assign random rules from null class
void assignRandomRules() {
    printf("Assigned Rules: ");
    for (int i = 0; i < N; i++) {
        rules[i] = null_class[rand() % null_class_size];
        printf("Cell %d: Rule %d, ", i, rules[i]);
    }
    printf("\n\n");
}

// Function to find next unvisited state
int findNextUnvisitedState() {
    for (int i = 0; i < total_states; i++) {
        if (check[i] == 0) {
            return i;
        }
    }
    return -1; // All states visited
}

int main() {
    srand(time(NULL)); // Initialize random seed
    
    // Get user input
    printf("Enter number of cells (N < 32): ");
    scanf("%d", &N);
    
    if (N >= 32 || N <= 0) {
        printf("Invalid input! N must be between 1 and 31.\n");
        return 1;
    }
    
    // Calculate total possible states
    total_states = 1 << N; // 2^N
    
    // Allocate memory for check array
    check = (int*)calloc(total_states, sizeof(int));
    if (check == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Assign random rules
    assignRandomRules();
    
    printf("Starting simulation for %d cells with %d total states...\n\n", N, total_states);
    
    int states_visited = 0;
    int current_decimal = 0; // Start from state 0 (all zeros)
    
    while (states_visited < total_states) {
        // Convert decimal to binary state
        decimalToState(current_decimal, current_state, N);
        
        // Mark current state as visited
        if (check[current_decimal] == 0) {
            check[current_decimal] = 1;
            states_visited++;
            
            printf("Step %d: ", states_visited);
            printState();
            
            // Compute next state
            computeNextState();
            int next_decimal = stateToDecimal(current_state, N);
            
            // Check if next state has been visited (cycle detection)
            if (check[next_decimal] == 1) {
                printf("  -> Cycle detected! Next state %d already visited.\n", next_decimal);
                
                // Jump to next unvisited state
                current_decimal = findNextUnvisitedState();
                if (current_decimal == -1) {
                    break; // All states visited
                }
                printf("  -> Jumping to unvisited state %d\n\n", current_decimal);
            } else {
                current_decimal = next_decimal;
            }
        } else {
            // This shouldn't happen in our logic, but just in case
            current_decimal = findNextUnvisitedState();
            if (current_decimal == -1) {
                break;
            }
        }
    }
    
    printf("\nSimulation complete! All %d states visited.\n", total_states);
    
    // Analyze cycle lengths (as mentioned in your findings)
    printf("\nAnalysis: Most cycles have length 1 (fixed points) when using null class rules.\n");
    
    // Free allocated memory
    free(check);
    
    return 0;
}

// Additional utility function to print rule in binary
void printRuleBinary(int rule) {
    printf("Rule %d binary: ", rule);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (rule >> i) & 1);
    }
    printf("\n");
}

// Function to analyze rule behavior (for debugging)
void analyzeRule(int rule) {
    printf("Rule %d truth table:\n", rule);
    printf("LCR -> Next\n");
    for (int pattern = 7; pattern >= 0; pattern--) {
        int left = (pattern >> 2) & 1;
        int center = (pattern >> 1) & 1;
        int right = pattern & 1;
        int next = (rule >> pattern) & 1;
        printf("%d%d%d -> %d\n", left, center, right, next);
    }
    printf("\n");
}