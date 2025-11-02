#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CELLS 10
#define MAX_RULES 256
#define MAX_CONFIGS 1024

// Structure to hold a configuration
typedef struct {
    int rules[MAX_CELLS];
    int n_cells;
} Config;

// Structure to hold class information
typedef struct {
    char class_sequence[MAX_CELLS][20];
    int is_valid;
    int has_class_2_or_5;
} ClassInfo;

// Global lookup tables - organized by (class, rule) -> next_class
typedef struct {
    int rule;
    char next_class[10];
} RuleMapping;

// Each class has its own set of rules and their next classes
RuleMapping class_I_mappings[50];
int class_I_count = 0;
RuleMapping class_II_mappings[50];
int class_II_count = 0;
RuleMapping class_III_mappings[50];
int class_III_count = 0;
RuleMapping class_IV_mappings[50];
int class_IV_count = 0;
RuleMapping class_V_mappings[50];
int class_V_count = 0;
RuleMapping class_VI_mappings[50];
int class_VI_count = 0;

// First rule table
int first_rule_table[16];
char first_rule_class[16][10];

void add_mapping(RuleMapping* arr, int* count, int rule, const char* next_class) {
    arr[*count].rule = rule;
    strcpy(arr[*count].next_class, next_class);
    (*count)++;
}

void initialize_tables() {
    memset(first_rule_table, -1, sizeof(first_rule_table));

    // ===== Class I Mappings =====
    // Class I -> I
    add_mapping(class_I_mappings, &class_I_count, 51, "I");
    add_mapping(class_I_mappings, &class_I_count, 204, "I");
    add_mapping(class_I_mappings, &class_I_count, 60, "I");
    add_mapping(class_I_mappings, &class_I_count, 195, "I");

    // Class I -> II
    add_mapping(class_I_mappings, &class_I_count, 85, "II");
    add_mapping(class_I_mappings, &class_I_count, 90, "II");
    add_mapping(class_I_mappings, &class_I_count, 165, "II");
    add_mapping(class_I_mappings, &class_I_count, 170, "II");

    // Class I -> III
    add_mapping(class_I_mappings, &class_I_count, 102, "III");
    add_mapping(class_I_mappings, &class_I_count, 105, "III");
    add_mapping(class_I_mappings, &class_I_count, 150, "III");
    add_mapping(class_I_mappings, &class_I_count, 153, "III");

    // Class I -> IV
    add_mapping(class_I_mappings, &class_I_count, 53, "IV");
    add_mapping(class_I_mappings, &class_I_count, 58, "IV");
    add_mapping(class_I_mappings, &class_I_count, 83, "IV");
    add_mapping(class_I_mappings, &class_I_count, 92, "IV");
    add_mapping(class_I_mappings, &class_I_count, 163, "IV");
    add_mapping(class_I_mappings, &class_I_count, 172, "IV");
    add_mapping(class_I_mappings, &class_I_count, 197, "IV");
    add_mapping(class_I_mappings, &class_I_count, 202, "IV");

    // Class I -> V
    add_mapping(class_I_mappings, &class_I_count, 54, "V");
    add_mapping(class_I_mappings, &class_I_count, 57, "V");
    add_mapping(class_I_mappings, &class_I_count, 99, "V");
    add_mapping(class_I_mappings, &class_I_count, 108, "V");
    add_mapping(class_I_mappings, &class_I_count, 147, "V");
    add_mapping(class_I_mappings, &class_I_count, 156, "V");
    add_mapping(class_I_mappings, &class_I_count, 198, "V");
    add_mapping(class_I_mappings, &class_I_count, 201, "V");

    // Class I -> VI
    add_mapping(class_I_mappings, &class_I_count, 86, "VI");
    add_mapping(class_I_mappings, &class_I_count, 89, "VI");
    add_mapping(class_I_mappings, &class_I_count, 101, "VI");
    add_mapping(class_I_mappings, &class_I_count, 106, "VI");
    add_mapping(class_I_mappings, &class_I_count, 149, "VI");
    add_mapping(class_I_mappings, &class_I_count, 154, "VI");
    add_mapping(class_I_mappings, &class_I_count, 166, "VI");
    add_mapping(class_I_mappings, &class_I_count, 169, "VI");

    // ===== Class II Mappings (all -> I) =====
    int class_II_rules[] = {15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240};
    for (int i = 0; i < 16; i++) {
        add_mapping(class_II_mappings, &class_II_count, class_II_rules[i], "I");
    }

    // ===== Class III Mappings =====
    // Class III -> I
    add_mapping(class_III_mappings, &class_III_count, 51, "I");
    add_mapping(class_III_mappings, &class_III_count, 204, "I");
    add_mapping(class_III_mappings, &class_III_count, 15, "I");
    add_mapping(class_III_mappings, &class_III_count, 240, "I");

    // Class III -> II
    add_mapping(class_III_mappings, &class_III_count, 85, "II");
    add_mapping(class_III_mappings, &class_III_count, 105, "II");
    add_mapping(class_III_mappings, &class_III_count, 150, "II");
    add_mapping(class_III_mappings, &class_III_count, 170, "II");

    // Class III -> III
    add_mapping(class_III_mappings, &class_III_count, 90, "III");
    add_mapping(class_III_mappings, &class_III_count, 102, "III");
    add_mapping(class_III_mappings, &class_III_count, 153, "III");
    add_mapping(class_III_mappings, &class_III_count, 165, "III");

    // Class III -> IV
    add_mapping(class_III_mappings, &class_III_count, 23, "IV");
    add_mapping(class_III_mappings, &class_III_count, 43, "IV");
    add_mapping(class_III_mappings, &class_III_count, 77, "IV");
    add_mapping(class_III_mappings, &class_III_count, 113, "IV");
    add_mapping(class_III_mappings, &class_III_count, 142, "IV");
    add_mapping(class_III_mappings, &class_III_count, 178, "IV");
    add_mapping(class_III_mappings, &class_III_count, 212, "IV");
    add_mapping(class_III_mappings, &class_III_count, 232, "IV");

    // Class III -> V
    add_mapping(class_III_mappings, &class_III_count, 27, "V");
    add_mapping(class_III_mappings, &class_III_count, 39, "V");
    add_mapping(class_III_mappings, &class_III_count, 78, "V");
    add_mapping(class_III_mappings, &class_III_count, 114, "V");
    add_mapping(class_III_mappings, &class_III_count, 141, "V");
    add_mapping(class_III_mappings, &class_III_count, 177, "V");
    add_mapping(class_III_mappings, &class_III_count, 216, "V");
    add_mapping(class_III_mappings, &class_III_count, 228, "V");

    // Class III -> VI
    add_mapping(class_III_mappings, &class_III_count, 86, "VI");
    add_mapping(class_III_mappings, &class_III_count, 89, "VI");
    add_mapping(class_III_mappings, &class_III_count, 101, "VI");
    add_mapping(class_III_mappings, &class_III_count, 106, "VI");
    add_mapping(class_III_mappings, &class_III_count, 149, "VI");
    add_mapping(class_III_mappings, &class_III_count, 154, "VI");
    add_mapping(class_III_mappings, &class_III_count, 166, "VI");
    add_mapping(class_III_mappings, &class_III_count, 169, "VI");

    // ===== Class IV Mappings =====
    // Class IV -> I
    add_mapping(class_IV_mappings, &class_IV_count, 60, "I");
    add_mapping(class_IV_mappings, &class_IV_count, 195, "I");

    // Class IV -> IV
    add_mapping(class_IV_mappings, &class_IV_count, 90, "IV");
    add_mapping(class_IV_mappings, &class_IV_count, 165, "IV");

    // Class IV -> V
    add_mapping(class_IV_mappings, &class_IV_count, 105, "V");
    add_mapping(class_IV_mappings, &class_IV_count, 150, "V");

    // ===== Class V Mappings =====
    // Class V -> I
    add_mapping(class_V_mappings, &class_V_count, 51, "I");
    add_mapping(class_V_mappings, &class_V_count, 204, "I");

    // Class V -> II
    add_mapping(class_V_mappings, &class_V_count, 85, "II");
    add_mapping(class_V_mappings, &class_V_count, 170, "II");

    // Class V -> III
    add_mapping(class_V_mappings, &class_V_count, 102, "III");
    add_mapping(class_V_mappings, &class_V_count, 153, "III");

    // Class V -> V
    add_mapping(class_V_mappings, &class_V_count, 86, "V");
    add_mapping(class_V_mappings, &class_V_count, 89, "V");
    add_mapping(class_V_mappings, &class_V_count, 90, "V");
    add_mapping(class_V_mappings, &class_V_count, 101, "V");
    add_mapping(class_V_mappings, &class_V_count, 105, "V");
    add_mapping(class_V_mappings, &class_V_count, 106, "V");
    add_mapping(class_V_mappings, &class_V_count, 149, "V");
    add_mapping(class_V_mappings, &class_V_count, 150, "V");
    add_mapping(class_V_mappings, &class_V_count, 154, "V");
    add_mapping(class_V_mappings, &class_V_count, 165, "V");
    add_mapping(class_V_mappings, &class_V_count, 166, "V");
    add_mapping(class_V_mappings, &class_V_count, 169, "V");

    // ===== Class VI Mappings =====
    // Class VI -> I
    add_mapping(class_VI_mappings, &class_VI_count, 15, "I");
    add_mapping(class_VI_mappings, &class_VI_count, 240, "I");

    // Class VI -> IV
    add_mapping(class_VI_mappings, &class_VI_count, 105, "IV");
    add_mapping(class_VI_mappings, &class_VI_count, 150, "IV");

    // Class VI -> V
    add_mapping(class_VI_mappings, &class_VI_count, 90, "V");
    add_mapping(class_VI_mappings, &class_VI_count, 165, "V");

    // ===== First Rule Table =====
    first_rule_table[3] = 1; strcpy(first_rule_class[3], "I");
    first_rule_table[12] = 1; strcpy(first_rule_class[12], "I");
    first_rule_table[5] = 1; strcpy(first_rule_class[5], "II");
    first_rule_table[10] = 1; strcpy(first_rule_class[10], "II");
    first_rule_table[6] = 1; strcpy(first_rule_class[6], "III");
    first_rule_table[9] = 1; strcpy(first_rule_class[9], "III");
}

const char* get_next_class(const char* current_class, int rule) {
    RuleMapping* mappings = NULL;
    int count = 0;

    if (strcmp(current_class, "I") == 0) {
        mappings = class_I_mappings;
        count = class_I_count;
    } else if (strcmp(current_class, "II") == 0) {
        mappings = class_II_mappings;
        count = class_II_count;
    } else if (strcmp(current_class, "III") == 0) {
        mappings = class_III_mappings;
        count = class_III_count;
    } else if (strcmp(current_class, "IV") == 0) {
        mappings = class_IV_mappings;
        count = class_IV_count;
    } else if (strcmp(current_class, "V") == 0) {
        mappings = class_V_mappings;
        count = class_V_count;
    } else if (strcmp(current_class, "VI") == 0) {
        mappings = class_VI_mappings;
        count = class_VI_count;
    }

    for (int i = 0; i < count; i++) {
        if (mappings[i].rule == rule) {
            return mappings[i].next_class;
        }
    }

    return NULL;
}

int is_valid_last_rule(int rule, const char* last_class) {
    // Class I or Class IV: last cell should be 150
    if (strcmp(last_class, "I") == 0 || strcmp(last_class, "IV") == 0) {
        return (rule == 150);
    }
    // Class II: last cell should be 90 or 150
    else if (strcmp(last_class, "II") == 0) {
        return (rule == 90 || rule == 150);
    }
    // Class III or Class VI: last cell should be 90
    else if (strcmp(last_class, "III") == 0 || strcmp(last_class, "VI") == 0) {
        return (rule == 90);
    }
    // Class V: accept both 90 and 150
    else if (strcmp(last_class, "V") == 0) {
        return (rule == 90 || rule == 150);
    }
    return 0;
}

ClassInfo validate_and_classify_config(Config* config) {
    ClassInfo info;
    info.is_valid = 1;  // Always valid now (no validation)
    info.has_class_2_or_5 = 0;

    if (config->n_cells < 2) {
        info.is_valid = 0;
        return info;
    }

    // Step 1: Get first rule class
    int first_4_bits = config->rules[0] & 0xF;
    if (first_rule_table[first_4_bits] == -1) {
        info.is_valid = 0;
        return info;
    }

    strcpy(info.class_sequence[0], "DC");  // Don't care
    char current_class[10];
    strcpy(current_class, first_rule_class[first_4_bits]);

    // Step 2: Process middle rules (R1 to Rn-2)
    for (int i = 1; i < config->n_cells - 1; i++) {
        int rule = config->rules[i];

        // Get next class for this rule in the current class context
        const char* next = get_next_class(current_class, rule);
        if (next == NULL) {
            info.is_valid = 0;
            return info;  // Invalid configuration
        }

        strcpy(info.class_sequence[i], current_class);

        // Check if this rule is 90 or 150 in Class II or V
        if ((rule == 90 || rule == 150) &&
            (strcmp(current_class, "II") == 0 || strcmp(current_class, "V") == 0)) {
            info.has_class_2_or_5 = 1;
            }

            // Move to next class
            strcpy(current_class, next);
    }

    // Step 3: Last rule is don't care
    strcpy(info.class_sequence[config->n_cells - 1], "DC");

    return info;
}

int main() {
    initialize_tables();

    printf("CA Configuration Class Filter\n");
    printf("============================================================\n");
    printf("Enter configurations (format: Rules: n1 n2 n3 ...)\n");
    printf("Enter 'N=X' to specify number of cells\n");
    printf("Press Ctrl+D (Linux/Mac) or Ctrl+Z (Windows) when done\n\n");

    char line[256];
    int current_n = 0;
    int filtered_count = 0;
    int valid_count = 0;

    while (fgets(line, sizeof(line), stdin)) {
        // Check for N= line
        if (sscanf(line, "N = %d", &current_n) == 1 ||
            sscanf(line, "N=%d", &current_n) == 1) {
            if (filtered_count > 0 || valid_count > 0) {
                printf("\n--- Summary ---\n");
                printf("Configurations with Class II or V: %d\n\n", filtered_count);
            }
            printf("============================================================\n");
        printf("N = %d\n", current_n);
        printf("============================================================\n");
        filtered_count = 0;
        valid_count = 0;
        continue;
            }

            // Check for Rules: line
            if (strstr(line, "Rules:") != NULL) {
                Config config;
                config.n_cells = 0;

                char* token = strtok(line, " ");
                while (token != NULL && config.n_cells < MAX_CELLS) {
                    int num;
                    if (sscanf(token, "%d", &num) == 1) {
                        config.rules[config.n_cells++] = num;
                    }
                    token = strtok(NULL, " ");
                }

                if (config.n_cells > 0) {
                    ClassInfo info = validate_and_classify_config(&config);

                    if (info.has_class_2_or_5) {
                        filtered_count++;

                        printf("\nRules: ");
                        for (int i = 0; i < config.n_cells; i++) {
                            printf("%d ", config.rules[i]);
                        }
                        printf("\nClasses: ");
                        for (int i = 0; i < config.n_cells; i++) {
                            printf("%s", info.class_sequence[i]);
                            if (i < config.n_cells - 1) printf(", ");
                        }
                        printf("\n");
                    }
                }
            }
    }

    if (filtered_count > 0 || valid_count > 0) {
        printf("\n--- Summary ---\n");
        printf("Configurations with Class II or V: %d\n", filtered_count);
    }

    return 0;
}
