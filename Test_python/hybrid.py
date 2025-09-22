import numpy as np
import random
from collections import Counter

# Rule 90 and Rule 150 update functions
def rule90(left, center, right):
    return left ^ right

def rule150(left, center, right):
    return left ^ center ^ right

# Hybrid update function based on a mask (list of 90/150 per cell)
def hybrid_update(state, mask):
    n = len(state)
    new_state = np.zeros_like(state)
    for i in range(n):
        left = state[(i - 1) % n]
        center = state[i]
        right = state[(i + 1) % n]
        if mask[i] == 90:
            new_state[i] = rule90(left, center, right)
        elif mask[i] == 150:
            new_state[i] = rule150(left, center, right)
        else:
            raise ValueError("Mask must contain only 90 or 150")
    return new_state

# Convert between integer <-> binary state
def int_to_state(x, n):
    return np.array([(x >> i) & 1 for i in reversed(range(n))], dtype=np.uint8)

def state_to_int(state):
    return int("".join(map(str, state.tolist())), 2)

# Build state transition graph and analyze cycles for a given mask
def analyze_cycles(n, mask):
    visited = {}
    cycles = []
    for start in range(2**n):
        if start in visited:
            continue
        seen = {}
        current = int_to_state(start, n)
        while True:
            idx = state_to_int(current)
            if idx in seen:
                # cycle detected
                cycle_start = seen[idx]
                cycle = list(seen.keys())[cycle_start:]
                cycles.append(cycle)
                for s in seen.keys():
                    visited[s] = True
                break
            if idx in visited:
                # already known
                break
            seen[idx] = len(seen)
            current = hybrid_update(current, mask)
    return cycles

# Random mask generator
def random_mask(n):
    return [random.choice([90, 150]) for _ in range(n)]

if __name__ == "__main__":
    n = 12
    num_trials = 50  # how many random masks to test
    
    best_len = 0
    best_mask = None
    best_cycles = None
    
    for trial in range(num_trials):
        mask = random_mask(n)
        cycles = analyze_cycles(n, mask)
        lengths = [len(c) for c in cycles]
        max_len = max(lengths)
        
        if max_len > best_len:
            best_len = max_len
            best_mask = mask
            best_cycles = cycles
            print(f"New best found! Cycle length = {best_len}, mask = {mask}")
    
    print("\n==== FINAL BEST RESULT ====")
    print(f"Best mask: {best_mask}")
    print(f"Largest cycle length: {best_len}")
    print(f"Maximal length possible: {2**n - 1}")
    print(f"Is maximal? {'YES' if best_len == 2**n - 1 else 'NO'}")
    
    # Show one sample cycle
    largest = max(best_cycles, key=len)
    print("\nSample largest cycle (first 16 states):")
    for state in largest[:16]:
        print(bin(state)[2:].zfill(n))
    print("... (truncated)")
