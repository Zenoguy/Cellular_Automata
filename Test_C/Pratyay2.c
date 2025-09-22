#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELLS 4
#define MAX_STATES 16
#define MAX_STEPS 20

// CA Configuration
typedef struct {
    int rules[CELLS];
    int boundary_type;
} CA_Config;

// Rule classification data
typedef struct {
    int rule;
    int class;
    int is_linear;
    char* description;
} RuleInfo;

// Rule database with class information (based on Wolfram's classification)
RuleInfo rule_database[] = {
    // Class I (Fixed point/simple patterns)
    {0, 1, 1, "All zeros"},
    {8, 1, 1, "Simple fixed patterns"},
    {32, 1, 1, "Simple fixed patterns"},
    {136, 1, 1, "Simple fixed patterns"},
    
    // Class II (Periodic patterns)
    {1, 2, 1, "Simple periodic"},
    {4, 2, 1, "Simple periodic"},
    {5, 2, 1, "Simple periodic"},
    {19, 2, 1, "Simple periodic"},
    {51, 2, 1, "Simple periodic"},
    {204, 2, 1, "Simple periodic"},
    
    // Class III (Chaotic/Random-like)
    {18, 3, 0, "Chaotic - Non-linear"},
    {22, 3, 0, "Chaotic - Non-linear"},
    {30, 3, 0, "Chaotic - Non-linear"}, 
    {45, 3, 0, "Chaotic - Non-linear"},
    {60, 3, 0, "Chaotic - Non-linear"},
    {75, 3, 0, "Chaotic - Non-linear"},
    {89, 3, 0, "Chaotic - Non-linear"},
    {90, 3, 1, "Chaotic - Linear (XOR)"}, // Rule 90 is linear but chaotic
    {99, 3, 0, "Chaotic - Non-linear"},
    {101, 3, 0, "Chaotic - Non-linear"},
    {105, 3, 0, "Chaotic - Non-linear"},
    {122, 3, 0, "Chaotic - Non-linear"},
    {126, 3, 0, "Chaotic - Non-linear"},
    {129, 3, 0, "Chaotic - Non-linear"},
    {135, 3, 0, "Chaotic - Non-linear"},
    {150, 3, 1, "Chaotic - Linear (XOR)"}, // Rule 150 is linear but chaotic
    {165, 3, 0, "Chaotic - Non-linear"},
    {195, 3, 0, "Chaotic - Non-linear"},
    
    // Class IV (Complex/Edge of chaos)
    {54, 4, 0, "Complex - Non-linear"},
    {110, 4, 0, "Complex - Non-linear"},
    {124, 4, 0, "Complex - Non-linear"},
    {137, 4, 0, "Complex - Non-linear"},
    {193, 4, 0, "Complex - Non-linear"},
};

int rule_db_size = sizeof(rule_database) / sizeof(RuleInfo);

// Get rule information
RuleInfo* get_rule_info(int rule) {
    for (int i = 0; i < rule_db_size; i++) {
        if (rule_database[i].rule == rule) {
            return &rule_database[i];
        }
    }
    
    // Default classification for unknown rules
    static RuleInfo unknown = {0, 3, 0, "Unknown classification"};
    unknown.rule = rule;
    return &unknown;
}

// Non-linear alternatives for linear rules
int nonlinear_alternatives[] = {18, 22, 30, 45, 60, 75, 89, 99, 101, 105, 122, 126, 129, 135, 165, 195};
int nonlinear_count = sizeof(nonlinear_alternatives) / sizeof(int);

// Convert binary array to decimal
int state_to_int(int state[CELLS]) {
    int result = 0;
    for (int i = 0; i < CELLS; i++) {
        result = result * 2 + state[i];
    }
    return result;
}

// Convert decimal to binary array
void int_to_state(int num, int state[CELLS]) {
    for (int i = CELLS - 1; i >= 0; i--) {
        state[i] = num % 2;
        num /= 2;
    }
}

// Get rule output for given neighborhood
int get_rule_value(int left, int center, int right, int rule) {
    int pattern = left * 4 + center * 2 + right;
    return (rule >> pattern) & 1;
}

// Apply one evolution step
void evolve_step(int current[CELLS], int next[CELLS], CA_Config* config) {
    for (int i = 0; i < CELLS; i++) {
        int left, right;
        
        if (config->boundary_type == 0) { // Null boundary
            left = (i == 0) ? 0 : current[i-1];
            right = (i == CELLS-1) ? 0 : current[i+1];
        } else { // Periodic boundary
            left = current[(i-1+CELLS) % CELLS];
            right = current[(i+1) % CELLS];
        }
        
        next[i] = get_rule_value(left, current[i], right, config->rules[i]);
    }
}

// Find all cycles and their lengths
void analyze_cycles(CA_Config* config, int* max_cycle, int* total_cycles) {
    int visited[MAX_STATES] = {0};
    int cycle_lengths[MAX_STATES] = {0};
    int cycle_count = 0;
    *max_cycle = 0;
    
    for (int start = 0; start < MAX_STATES; start++) {
        if (visited[start]) continue;
        
        int current[CELLS], next[CELLS];
        int path[MAX_STEPS];
        int step = 0;
        
        int_to_state(start, current);
        
        // Trace evolution path
        while (step < MAX_STEPS) {
            int current_int = state_to_int(current);
            
            // Check if we've seen this state before
            for (int i = 0; i < step; i++) {
                if (path[i] == current_int) {
                    int cycle_length = step - i;
                    cycle_lengths[cycle_count] = cycle_length;
                    if (cycle_length > *max_cycle) {
                        *max_cycle = cycle_length;
                    }
                    
                    // Mark all states in this cycle as visited
                    for (int j = i; j < step; j++) {
                        if (path[j] < MAX_STATES) {
                            visited[path[j]] = 1;
                        }
                    }
                    cycle_count++;
                    goto next_start;
                }
            }
            
            path[step] = current_int;
            visited[current_int] = 1;
            
            evolve_step(current, next, config);
            for (int i = 0; i < CELLS; i++) {
                current[i] = next[i];
            }
            step++;
        }
        
        next_start:;
    }
    
    *total_cycles = cycle_count;
}

// Check if CA has maximal length cycle
int is_maximal_length_ca(CA_Config* config) {
    int max_cycle, total_cycles;
    analyze_cycles(config, &max_cycle, &total_cycles);
    return max_cycle == MAX_STATES;
}

// Print binary representation
void print_binary(int num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

// Print rule with class information
void print_rule_with_class(int rule) {
    RuleInfo* info = get_rule_info(rule);
    printf("Rule %3d (Class %s%d) [%s] - %s\n", 
           rule, 
           info->is_linear ? "L" : "N",
           info->class,
           info->is_linear ? "Linear" : "Non-linear",
           info->description);
}

// Test configuration with detailed analysis
void test_configuration(CA_Config* config, char* config_name) {
    printf("\n" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    printf("Testing Configuration: %s\n", config_name);
    printf("" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    
    printf("\nRules and Classifications:\n");
    for (int i = 0; i < CELLS; i++) {
        printf("Cell %d: ", i + 1);
        print_rule_with_class(config->rules[i]);
    }
    
    printf("\nBoundary: %s\n", config->boundary_type ? "Periodic" : "Null");
    
    int max_cycle, total_cycles;
    analyze_cycles(config, &max_cycle, &total_cycles);
    
    printf("\nCycle Analysis:\n");
    printf("Maximum cycle length: %d\n", max_cycle);
    printf("Total number of cycles: %d\n", total_cycles);
    
    if (max_cycle == MAX_STATES) {
        printf("✓ This IS a MAXIMAL LENGTH CA! (cycles through all %d states)\n", MAX_STATES);
    } else {
        printf("✗ This is NOT a maximal length CA.\n");
        printf("  (Maximum cycle length is %d, need %d for maximal length)\n", max_cycle, MAX_STATES);
    }
}

// Suggest non-linear replacements
void suggest_nonlinear_replacements(int linear_rule) {
    printf("\nNon-linear alternatives for Rule %d:\n", linear_rule);
    printf("Try replacing with: ");
    
    int suggestions = 0;
    for (int i = 0; i < nonlinear_count && suggestions < 5; i++) {
        printf("%d ", nonlinear_alternatives[i]);
        suggestions++;
    }
    printf("\n");
}

int main() {
    printf("Enhanced Maximal Length CA Analyzer\n");
    printf("==================================\n");
    
    // Original configuration [90, 150, 105, 195]
    CA_Config original;
    original.rules[0] = 90;
    original.rules[1] = 150;
    original.rules[2] = 105;
    original.rules[3] = 195;
    original.boundary_type = 1; // Periodic
    
    test_configuration(&original, "Original [90, 150, 105, 195]");
    
    // Check which rules are linear
    printf("\n" "Linear Rules Analysis:\n");
    printf("" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "\n");
    for (int i = 0; i < CELLS; i++) {
        RuleInfo* info = get_rule_info(original.rules[i]);
        if (info->is_linear) {
            printf("Cell %d: Rule %d is LINEAR - can be replaced with non-linear\n", 
                   i + 1, original.rules[i]);
            suggest_nonlinear_replacements(original.rules[i]);
        } else {
            printf("Cell %d: Rule %d is already NON-LINEAR\n", i + 1, original.rules[i]);
        }
    }
    
    // Test replacement of Rule 150 (linear) with Rule 30 (non-linear)
    printf("\n" "FUTURE WORK EXAMPLE:\n");
    printf("Replacing Rule 150 (linear) with Rule 30 (non-linear)\n");
    
    CA_Config modified;
    modified.rules[0] = 90;   // Keep
    modified.rules[1] = 30;   // Replace 150 with 30 (famous chaotic rule)
    modified.rules[2] = 105;  // Keep
    modified.rules[3] = 195;  // Keep
    modified.boundary_type = 1;
    
    test_configuration(&modified, "Modified [90, 30, 105, 195]");
    
    // Test another replacement
    CA_Config modified2;
    modified2.rules[0] = 90;
    modified2.rules[1] = 110;  // Replace with Rule 110 (Class IV - complex)
    modified2.rules[2] = 105;
    modified2.rules[3] = 195;
    modified2.boundary_type = 1;
    
    test_configuration(&modified2, "Modified [90, 110, 105, 195]");
    
    printf("\n" "RESEARCH DIRECTIONS:\n");
    printf("" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    printf("1. Systematically replace each linear rule with non-linear alternatives\n");
    printf("2. Test Class III (chaotic) vs Class IV (complex) replacements\n");
    printf("3. Check if maximal length property is preserved\n");
    printf("4. Analyze how different classes affect cycle structure\n");
    printf("5. Compare null vs periodic boundary effects\n");
    
    return 0;
}