#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CELLS 10
#define MAX_STEPS 512
#define MAX_CONFIGS 1024

// Non-linear rules for Class II and V
int nonlinear_class_II[] = {30, 45, 75, 120, 135, 180, 210, 225};
int nonlinear_class_II_count = 8;

int nonlinear_class_V[] = {51, 204, 85, 170, 102, 153, 86, 89, 101, 106, 149, 154, 166, 169};
int nonlinear_class_V_count = 14;

// Global variable for number of cells
int CELLS;

// Structure to represent a CA configuration
typedef struct {
    int rules[MAX_CELLS];
    int boundary_type; // 0 = null, 1 = periodic
} CA_Config;

// Structure to hold positions that need replacement
typedef struct {
    int position;
    char class[10];
} ReplacePos;

// CA Evolution Functions (from original code)
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

        if (config->boundary_type == 0) { // Null boundary
            left = (i == 0) ? 0 : current[i - 1];
            right = (i == CELLS - 1) ? 0 : current[i + 1];
        } else { // Periodic boundary
            left = current[(i - 1 + CELLS) % CELLS];
            right = current[(i + 1) % CELLS];
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

int get_max_cycle_length(CA_Config* config) {
    int max_states = (int)pow(2, CELLS);
    int* visited = (int*)calloc(max_states, sizeof(int));
    int max_cycle_length = 0;

    for (long long start = 0; start < max_states; start++) {
        if (visited[start]) continue;

        int cycle_length;
        int cycle_start_step = find_cycle(start, config, &cycle_length);

        if (cycle_length > 0) {
            if (cycle_length > max_cycle_length) {
                max_cycle_length = cycle_length;
            }

            // Mark all states in this cycle as visited
            int current[MAX_CELLS];
            int_to_state(start, current);

            for (int step = 0; step <= cycle_start_step + cycle_length; step++) {
                long long state_int = state_to_int(current);
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
        }
    }

    free(visited);
    return max_cycle_length;
}

void generate_combinations(CA_Config* base_config, ReplacePos* replace_positions, int num_positions,
                           int* replacement_rules[], int* replacement_counts, int current_pos,
                           CA_Config* current_config, CA_Config** maximal_configs, int* maximal_count) {

    if (current_pos == num_positions) {
        // All positions filled, check max cycle length
        int max_cycle = get_max_cycle_length(current_config);
        int target_max = (int)pow(2, CELLS) - 1;

        if (max_cycle == target_max) {
            // Found a maximal configuration
            maximal_configs[*maximal_count] = (CA_Config*)malloc(sizeof(CA_Config));
            memcpy(maximal_configs[*maximal_count], current_config, sizeof(CA_Config));
            (*maximal_count)++;
        }
        return;
    }

    // Try all replacement rules for current position
    int pos = replace_positions[current_pos].position;
    int count = replacement_counts[current_pos];

    for (int i = 0; i < count; i++) {
        current_config->rules[pos] = replacement_rules[current_pos][i];
        generate_combinations(base_config, replace_positions, num_positions,
                              replacement_rules, replacement_counts, current_pos + 1,
                              current_config, maximal_configs, maximal_count);
    }
                           }

                           int main() {
                               printf("CA Non-linear Rule Replacement Analyzer\n");
                               printf("============================================================\n");
                               printf("Reading from CA Configuration Class Filter output...\n\n");

                               char line[512];
                               int current_n = 0;

                               while (fgets(line, sizeof(line), stdin)) {
                                   // Check for N= line
                                   if (sscanf(line, "N = %d", &current_n) == 1 ||
                                       sscanf(line, "N=%d", &current_n) == 1) {
                                       CELLS = current_n;
                                   printf("============================================================\n");
                                   printf("N = %d (Target max cycle length: %d)\n", CELLS, (int)pow(2, CELLS) - 1);
                                   printf("============================================================\n");
                                   continue;
                                       }

                                       // Check for Rules: line
                                       if (strstr(line, "Rules:") != NULL) {
                                           CA_Config base_config;
                                           base_config.boundary_type = 0; // Null boundary
                                           base_config.rules[0] = 0;
                                           int n_rules = 0;

                                           // Parse rules
                                           char* token = strtok(line, " ");
                                           while (token != NULL && n_rules < MAX_CELLS) {
                                               int num;
                                               if (sscanf(token, "%d", &num) == 1) {
                                                   base_config.rules[n_rules++] = num;
                                               }
                                               token = strtok(NULL, " ");
                                           }

                                           // Read next line to get classes
                                           if (!fgets(line, sizeof(line), stdin)) break;

                                           if (strstr(line, "Classes:") != NULL) {
                                               // Parse classes
                                               char classes[MAX_CELLS][20];
                                               int class_count = 0;

                                               char* class_start = strstr(line, "Classes:") + 8;
                                               token = strtok(class_start, ",");
                                               while (token != NULL && class_count < MAX_CELLS) {
                                                   // Trim whitespace
                                                   while (*token == ' ') token++;
                                                   char* end = token + strlen(token) - 1;
                                                   while (end > token && (*end == ' ' || *end == '\n')) end--;
                                                   *(end + 1) = '\0';

                                                   strcpy(classes[class_count++], token);
                                                   token = strtok(NULL, ",");
                                               }

                                               // Find positions to replace (Class II or V, not DC)
                                               ReplacePos replace_positions[MAX_CELLS];
                                               int* replacement_rules[MAX_CELLS];
                                               int replacement_counts[MAX_CELLS];
                                               int num_replace_positions = 0;

                                               for (int i = 0; i < class_count && i < n_rules; i++) {
                                                   if (strcmp(classes[i], "II") == 0) {
                                                       replace_positions[num_replace_positions].position = i;
                                                       strcpy(replace_positions[num_replace_positions].class, "II");
                                                       replacement_rules[num_replace_positions] = nonlinear_class_II;
                                                       replacement_counts[num_replace_positions] = nonlinear_class_II_count;
                                                       num_replace_positions++;
                                                   } else if (strcmp(classes[i], "V") == 0) {
                                                       replace_positions[num_replace_positions].position = i;
                                                       strcpy(replace_positions[num_replace_positions].class, "V");
                                                       replacement_rules[num_replace_positions] = nonlinear_class_V;
                                                       replacement_counts[num_replace_positions] = nonlinear_class_V_count;
                                                       num_replace_positions++;
                                                   }
                                               }

                                               if (num_replace_positions > 0) {
                                                   printf("\nOriginal config: ");
                                                   for (int i = 0; i < n_rules; i++) {
                                                       printf("%d ", base_config.rules[i]);
                                                   }
                                                   printf("\nReplacing %d position(s) with non-linear rules...\n", num_replace_positions);

                                                   // Calculate total combinations
                                                   int total_combinations = 1;
                                                   for (int i = 0; i < num_replace_positions; i++) {
                                                       total_combinations *= replacement_counts[i];
                                                   }
                                                   printf("Testing %d combinations...\n", total_combinations);

                                                   // Generate all combinations and find maximal configs
                                                   CA_Config** maximal_configs = (CA_Config**)malloc(total_combinations * sizeof(CA_Config*));
                                                   int maximal_count = 0;

                                                   CA_Config current_config = base_config;
                                                   generate_combinations(&base_config, replace_positions, num_replace_positions,
                                                                         replacement_rules, replacement_counts, 0,
                                                                         &current_config, maximal_configs, &maximal_count);

                                                   if (maximal_count > 0) {
                                                       printf("Found %d maximal configuration(s):\n", maximal_count);
                                                       for (int i = 0; i < maximal_count; i++) {
                                                           printf("  ");
                                                           for (int j = 0; j < n_rules; j++) {
                                                               printf("%d ", maximal_configs[i]->rules[j]);
                                                           }
                                                           printf("\n");
                                                           free(maximal_configs[i]);
                                                       }
                                                   } else {
                                                       printf("No maximal configurations found.\n");
                                                   }

                                                   free(maximal_configs);
                                                   printf("\n");
                                               }
                                           }
                                       }
                               }

                               return 0;
                           }
