#include "ca.h"

int conway_rule(int state, int neighbors) {
    if (state == 1) {
        return (neighbors == 2 || neighbors == 3);
    } else {
        return (neighbors == 3);
    }
}

// Generic helper: apply elementary rule (0–255)
int apply_elementary_rule(int left, int self, int right, int rule) {
    int idx = (left << 2) | (self << 1) | right;  // neighborhood as 0–7
    return (rule >> idx) & 1;                     // extract bit
}

// Rule 90
int rule90(int state, int neighbors_unused) {
    // neighbors_unused is ignored, we'll compute manually
    // (Hack: state = cell, neighbors_unused = unused)
    return state; // Placeholder, real logic below
}

// Rule 150
int rule150(int state, int neighbors_unused) {
    return state; // Placeholder, real logic below
}
