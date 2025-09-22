import numpy as np
import random

# ========= Tables from your image =========

# (a) Class relationship: given class of Ri → class of Ri+1
class_transition = {
    "I": "I",
    "II": "I",
    "III": "I",
    "IV": "I",
    "V": "I",
    "VI": "I",  # simplified placeholder, see below
}

# This will need to be filled properly with the mappings from the table (a).
# For now, let's implement a prototype with placeholders.

# (b) First Rule Table
first_rule_table = {
    "I": [3, 12],
    "II": [5, 10],
    "III": [6, 9],
}

# (c) Last Rule Table
last_rule_table = {
    "I": [17, 20, 65, 68],
    "II": [5, 20, 65, 80],
    "III": [5, 17, 68, 80],
    "IV": [20, 65],
    "V": [17, 68],
    "VI": [5, 80],
}

# Helper: apply an elementary CA rule (0–255) to a neighborhood
def apply_rule(rule, left, center, right):
    idx = (left << 2) | (center << 1) | right
    return (rule >> idx) & 1

# Step update for state and rule classes
def step(state, rules, classes):
    n = len(state)
    new_state = np.zeros_like(state)
    new_rules = []
    new_classes = []

    for i in range(n):
        left = state[(i - 1) % n]
        center = state[i]
        right = state[(i + 1) % n]

        rule = rules[i]
        new_state[i] = apply_rule(rule, left, center, right)

        # Update rule class according to transition table
        old_class = classes[i]
        new_class = class_transition.get(old_class, old_class)

        # Pick a new rule from the available set for that class
        if new_class in last_rule_table:
            rule_choices = last_rule_table[new_class]
        else:
            rule_choices = [rule]  # fallback: keep same rule

        new_rule = random.choice(rule_choices)

        new_rules.append(new_rule)
        new_classes.append(new_class)

    return new_state, new_rules, new_classes

# Run CA
def run_ca(n=8, steps=10):
    state = np.random.randint(0, 2, n, dtype=np.uint8)
    classes = [random.choice(list(last_rule_table.keys())) for _ in range(n)]
    rules = [random.choice(last_rule_table[c]) for c in classes]

    print("Initial state:", "".join(map(str, state)))
    print("Initial rules:", rules)
    print("Initial classes:", classes)

    for t in range(steps):
        state, rules, classes = step(state, rules, classes)
        print(f"t={t+1}: {''.join(map(str, state))} | rules={rules}")

if __name__ == "__main__":
    run_ca(n=8, steps=20)
