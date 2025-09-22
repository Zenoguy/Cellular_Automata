# ========= Rule → Class (from Table 2.7a) =========
rule_to_class = {}

# --- Class I ---
for r in [51, 204, 60, 195, 85, 90, 165, 170,
          102, 105, 150, 153,
          53, 58, 83, 92, 163, 172, 197, 202,
          54, 57, 99, 108, 147, 156, 198, 201,
          86, 89, 101, 106, 149, 154, 166, 169]:
    rule_to_class[r] = "I"

# --- Class II ---
for r in [15, 30, 45, 60, 75, 90, 105, 120,
          135, 150, 165, 180, 195, 210, 225, 240]:
    rule_to_class[r] = "II"

# --- Class III ---
for r in [51, 204, 15, 240, 85, 105, 150, 170,
          90, 102, 153, 165,
          23, 43, 77, 113, 142, 178, 212, 232,
          27, 39, 78, 114, 141, 177, 216, 228,
          86, 89, 101, 106, 149, 154, 166, 169]:
    rule_to_class[r] = "III"

# --- Class IV ---
for r in [60, 195, 90, 165, 105, 150]:
    rule_to_class[r] = "IV"

# --- Class V ---
for r in [51, 204, 85, 170, 102, 153,
          86, 89, 90, 101, 105, 106,
          149, 150, 154, 165, 166, 169]:
    rule_to_class[r] = "V"

# --- Class VI ---
for r in [15, 240, 105, 150, 90, 165]:
    rule_to_class[r] = "VI"


# ========= Rule → Next Class (from Table 2.7a) =========
rule_to_nextclass = {}

# --- Class I rules ---
for r in [51, 204, 60, 195]:
    rule_to_nextclass[r] = "I"
for r in [85, 90, 165, 170]:
    rule_to_nextclass[r] = "II"
for r in [102, 105, 150, 153]:
    rule_to_nextclass[r] = "III"
for r in [53, 58, 83, 92, 163, 172, 197, 202]:
    rule_to_nextclass[r] = "IV"
for r in [54, 57, 99, 108, 147, 156, 198, 201]:
    rule_to_nextclass[r] = "V"
for r in [86, 89, 101, 106, 149, 154, 166, 169]:
    rule_to_nextclass[r] = "VI"

# --- Class II rules ---
for r in [15, 30, 45, 60, 75, 90, 105, 120,
          135, 150, 165, 180, 195, 210, 225, 240]:
    rule_to_nextclass[r] = "I"

# --- Class III rules ---
for r in [51, 204, 15, 240]:
    rule_to_nextclass[r] = "I"
for r in [85, 105, 150, 170]:
    rule_to_nextclass[r] = "II"
for r in [90, 102, 153, 165]:
    rule_to_nextclass[r] = "III"
for r in [23, 43, 77, 113, 142, 178, 212, 232]:
    rule_to_nextclass[r] = "IV"
for r in [27, 39, 78, 114, 141, 177, 216, 228]:
    rule_to_nextclass[r] = "V"
for r in [86, 89, 101, 106, 149, 154, 166, 169]:
    rule_to_nextclass[r] = "VI"

# --- Class IV rules ---
for r in [60, 195]:
    rule_to_nextclass[r] = "I"
for r in [90, 165]:
    rule_to_nextclass[r] = "IV"
for r in [105, 150]:
    rule_to_nextclass[r] = "V"

# --- Class V rules ---
for r in [51, 204]:
    rule_to_nextclass[r] = "I"
for r in [85, 170]:
    rule_to_nextclass[r] = "II"
for r in [102, 153]:
    rule_to_nextclass[r] = "III"
for r in [86, 89, 90, 101, 105, 106, 149, 150, 154, 165, 166, 169]:
    rule_to_nextclass[r] = "V"

# --- Class VI rules ---
for r in [15, 240]:
    rule_to_nextclass[r] = "I"
for r in [105, 150]:
    rule_to_nextclass[r] = "IV"
for r in [90, 165]:
    rule_to_nextclass[r] = "V"

# ========= First Rule Table (Table 2.7b) =========
# Maps R0 rule → its class
first_rule_table = {
    3: "I",
    12: "I",
    5: "II",
    10: "II",
    6: "III",
    9: "III",
}

# ========= Last Rule Table (Table 2.7c) =========
# Maps class of R_{n-1} → set of allowed rules for R_n
last_rule_table = {
    "I": [17, 20, 65, 68],
    "II": [5, 20, 65, 80],
    "III": [5, 17, 68, 80],
    "IV": [20, 65],
    "V": [17, 68],
    "VI": [5, 80],
}
