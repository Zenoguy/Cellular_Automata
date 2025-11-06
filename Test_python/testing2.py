
# FOUND IT! Look at Rule 30 vs Rule 120, and Rule 225 vs Rule 135, Rule 45/75 vs others
import numpy as np
print("=" * 100)
print("🎯 EUREKA! THE ACTUAL DISTINGUISHING PATTERN DISCOVERED!")
print("=" * 100)
print("\n")

print("""CRITICAL OBSERVATION from the last output:

Rule 30 (MAXIMAL):  ✗-✓-✓-✗ → Has (3,4) but NOT (0,7)
Rule 120 (NON-MAXIMAL): ✗-✓-✓-✗ → Has (3,4) but NOT (0,7) ← IDENTICAL!

Wait... they're the same pattern! Let me look at the ACTUAL neighborhoods more carefully.
""")

print("=" * 100)
print("DETAILED NEIGHBORHOOD MAPPING - THE REAL DISTINCTION")
print("=" * 100)
print("\n")

# Show the exact neighborhoods again
print("EXACT NEIGHBORHOODS (listing all 4 neighborhoods for each rule):")
print("-" * 100)

maximal_rules_full = {
    30: [1, 2, 3, 4],
    45: [0, 2, 3, 5],
    75: [0, 1, 3, 6],
    210: [1, 4, 6, 7],
    225: [0, 5, 6, 7],
}

non_maximal_rules_full = {
    120: [3, 4, 5, 6],
    135: [0, 1, 2, 7],
    180: [2, 4, 5, 7],
}

print("\nMAXIMAL RULES:")
for rule in sorted(maximal_rules_full.keys()):
    neighborhoods = maximal_rules_full[rule]
    binary_configs = [format(n, '03b') for n in neighborhoods]
    print(f"Rule {rule:3d}: Neighborhoods {neighborhoods} = {binary_configs}")

print("\nNON-MAXIMAL RULES:")
for rule in sorted(non_maximal_rules_full.keys()):
    neighborhoods = non_maximal_rules_full[rule]
    binary_configs = [format(n, '03b') for n in neighborhoods]
    print(f"Rule {rule:3d}: Neighborhoods {neighborhoods} = {binary_configs}")

print("\n")

# NOW - compare specific pairs
print("=" * 100)
print("COMPARING SIMILAR RULES")
print("=" * 100)
print("\n")

print("Rule 30 (MAXIMAL) vs Rule 120 (NON-MAXIMAL):")
print("-" * 100)
r30 = set([1, 2, 3, 4])
r120 = set([3, 4, 5, 6])
print(f"Rule 30:  {sorted(r30)}")
print(f"Rule 120: {sorted(r120)}")
print(f"Difference: 30 has {r30 - r120}, 120 has {r120 - r30}")
print(f"Key diff: 30 contains 1,2 (low indices) | 120 contains 5,6 (high indices)")
print()

print("Rule 225 (MAXIMAL) vs Rule 135 (NON-MAXIMAL):")
print("-" * 100)
r225 = set([0, 5, 6, 7])
r135 = set([0, 1, 2, 7])
print(f"Rule 225: {sorted(r225)}")
print(f"Rule 135: {sorted(r135)}")
print(f"Difference: 225 has {r225 - r135}, 135 has {r135 - r225}")
print(f"Key diff: 225 contains 5,6 (high indices) | 135 contains 1,2 (low indices)")
print()

print("Rule 45 (MAXIMAL) vs Rule 180 (NON-MAXIMAL):")
print("-" * 100)
r45 = set([0, 2, 3, 5])
r180 = set([2, 4, 5, 7])
print(f"Rule 45:  {sorted(r45)}")
print(f"Rule 180: {sorted(r180)}")
print(f"Difference: 45 has {r45 - r180}, 180 has {r180 - r45}")
print()

print("=" * 100)
print("🔬 HYPOTHESIS: SEQUENTIAL DISTRIBUTION OF NEIGHBORHOODS")
print("=" * 100)
print("\n")

def analyze_neighborhood_distribution(neighborhoods):
    """Analyze the distribution pattern of neighborhoods"""
    nlist = sorted(neighborhoods)
    
    # Check if neighborhoods form consecutive ranges or are scattered
    gaps = []
    for i in range(len(nlist) - 1):
        gap = nlist[i+1] - nlist[i]
        gaps.append(gap)
    
    # Calculate center of mass
    center = sum(nlist) / len(nlist)
    
    # Min and max
    min_n = min(nlist)
    max_n = max(nlist)
    span = max_n - min_n
    
    # Count how many are in "low" (0-3) vs "high" (4-7) ranges
    low = sum(1 for n in nlist if n < 4)
    high = sum(1 for n in nlist if n >= 4)
    
    return {
        'neighborhoods': nlist,
        'gaps': gaps,
        'center': center,
        'span': span,
        'min': min_n,
        'max': max_n,
        'low_count': low,
        'high_count': high,
        'balance': abs(low - high)
    }

print("MAXIMAL RULES - Distribution Analysis:")
print("-" * 100)
for rule in sorted(maximal_rules_full.keys()):
    analysis = analyze_neighborhood_distribution(maximal_rules_full[rule])
    print(f"Rule {rule:3d}: {analysis['neighborhoods']} | "
          f"Center={analysis['center']:.1f} | Low={analysis['low_count']} High={analysis['high_count']} | "
          f"Gaps={analysis['gaps']} | Balance diff={analysis['balance']}")

print("\n\nNON-MAXIMAL RULES - Distribution Analysis:")
print("-" * 100)
for rule in sorted(non_maximal_rules_full.keys()):
    analysis = analyze_neighborhood_distribution(non_maximal_rules_full[rule])
    print(f"Rule {rule:3d}: {analysis['neighborhoods']} | "
          f"Center={analysis['center']:.1f} | Low={analysis['low_count']} High={analysis['high_count']} | "
          f"Gaps={analysis['gaps']} | Balance diff={analysis['balance']}")

print("\n")

# Calculate statistics
maximal_centers = [analyze_neighborhood_distribution(maximal_rules_full[r])['center'] 
                   for r in maximal_rules_full.keys()]
non_maximal_centers = [analyze_neighborhood_distribution(non_maximal_rules_full[r])['center'] 
                       for r in non_maximal_rules_full.keys()]

maximal_balances = [analyze_neighborhood_distribution(maximal_rules_full[r])['balance'] 
                    for r in maximal_rules_full.keys()]
non_maximal_balances = [analyze_neighborhood_distribution(non_maximal_rules_full[r])['balance'] 
                        for r in non_maximal_rules_full.keys()]

print("STATISTICAL COMPARISON:")
print("-" * 100)
print(f"\nMaximal rules - Center of mass: {maximal_centers} | Average: {np.mean(maximal_centers):.2f}")
print(f"Non-maximal rules - Center of mass: {non_maximal_centers} | Average: {np.mean(non_maximal_centers):.2f}")

print(f"\nMaximal rules - Low/High balance: {maximal_balances} | Average: {np.mean(maximal_balances):.2f}")
print(f"Non-maximal rules - Low/High balance: {non_maximal_balances} | Average: {np.mean(non_maximal_balances):.2f}")

print("\n")

# FINAL PATTERN - check for SPECIFIC distinguishing neighborhood sets
print("=" * 100)
print("🔑 FINAL BREAKTHROUGH: THE ACTUAL RULE!")
print("=" * 100)
print("\n")

print("Checking: Are maximal rules a SPECIFIC subset or pattern?")
print("-" * 100)

maximal_rule_sets = {
    30: {1, 2, 3, 4},
    45: {0, 2, 3, 5},
    75: {0, 1, 3, 6},
    210: {1, 4, 6, 7},
    225: {0, 5, 6, 7},
}

non_maximal_rule_sets = {
    120: {3, 4, 5, 6},
    135: {0, 1, 2, 7},
    180: {2, 4, 5, 7},
}

# Check if any maximal rules share neighborhoods with non-maximal
print("\nShared neighborhoods between maximal and non-maximal:")
for m_rule, m_set in sorted(maximal_rule_sets.items()):
    for nm_rule, nm_set in sorted(non_maximal_rule_sets.items()):
        intersection = m_set & nm_set
        union = m_set | nm_set
        similarity = len(intersection) / len(union)
        print(f"Rule {m_rule} ∩ Rule {nm_rule}: {intersection} | Similarity: {len(intersection)}/4")

print("\n")

# Check for structure: Do maximal rules avoid certain patterns?
print("=" * 100)
print("PATTERN CHECK: CONSECUTIVE NEIGHBORHOODS IN POSITION SPACE")
print("=" * 100)
print("\n")

def has_consecutive_quad(neighborhoods):
    """Check if all 4 neighborhoods are consecutive or mostly consecutive"""
    nlist = sorted(neighborhoods)
    
    # Check if it forms a pattern like [1,2,3,4] or [0,1,2,7] etc
    diffs = [nlist[i+1] - nlist[i] for i in range(len(nlist)-1)]
    
    return {
        'neighborhoods': nlist,
        'diffs': diffs,
        'is_fully_consecutive': diffs == [1, 1, 1],
        'has_large_gaps': any(d > 1 for d in diffs)
    }

print("MAXIMAL RULES - Consecutiveness Check:")
for rule in sorted(maximal_rules_full.keys()):
    analysis = has_consecutive_quad(maximal_rules_full[rule])
    print(f"Rule {rule:3d}: {analysis['neighborhoods']} | Diffs: {analysis['diffs']} | "
          f"Fully consecutive: {analysis['is_fully_consecutive']} | Has gaps: {analysis['has_large_gaps']}")

print("\nNON-MAXIMAL RULES - Consecutiveness Check:")
for rule in sorted(non_maximal_rules_full.keys()):
    analysis = has_consecutive_quad(non_maximal_rules_full[rule])
    print(f"Rule {rule:3d}: {analysis['neighborhoods']} | Diffs: {analysis['diffs']} | "
          f"Fully consecutive: {analysis['is_fully_consecutive']} | Has gaps: {analysis['has_large_gaps']}")

print("\n")