import itertools
from typing import Set, List, Tuple, Dict, Optional

# Your existing dictionaries
rule_to_classes = {
    15: {"II", "III", "VI"},
    20: {"III"},
    23: {"III"},
    27: {"III"},
    30: {"II"},
    39: {"III"},
    43: {"III"},
    45: {"II"},
    51: {"I", "III", "V"},
    53: {"I"},
    54: {"I"},
    57: {"I"},
    58: {"I"},
    60: {"I", "II", "IV"},
    65: {"III"},
    68: {"III"},
    75: {"II"},
    77: {"III"},
    78: {"III"},
    80: {"III"},
    83: {"I"},
    85: {"I", "II", "III", "V"},
    86: {"I", "III", "V"},
    89: {"I", "III", "V"},
    90: {"I", "II", "III", "IV", "V", "VI"},
    92: {"I"},
    99: {"I"},
    101: {"I", "III", "V"},
    102: {"I", "III", "V"},
    105: {"I", "III", "IV", "V", "VI"},
    106: {"I", "III", "V"},
    108: {"I"},
    113: {"III"},
    114: {"III"},
    120: {"II"},
    135: {"II"},
    141: {"III"},
    142: {"III"},
    147: {"I"},
    149: {"I", "III", "V"},
    150: {"II", "III", "IV", "V", "VI"},
    153: {"I", "III", "V"},
    154: {"I", "III", "V"},
    156: {"I"},
    163: {"I"},
    165: {"I", "II", "III", "V", "VI"},
    166: {"I", "III", "V"},
    169: {"I", "III", "V"},
    170: {"I", "II", "V"},
    172: {"I"},
    177: {"III"},
    178: {"III"},
    180: {"II"},
    195: {"I", "IV"},
    196: {"III"},
    197: {"I"},
    198: {"I"},
    201: {"I", "V"},
    202: {"I"},
    204: {"I", "III"},
    210: {"II"},
    212: {"III"},
    216: {"III"},
    225: {"II"},
    228: {"III"},
    232: {"III"},
    240: {"II", "III", "VI"},
    5: {"II", "III", "VI"},
    17: {"I", "V"},
    # Note: 20, 65, 68, 80 already exist above
}

rule_to_nextclass = {
    15: "II", 20: "III", 23: "III", 27: "III", 30: "II", 39: "III", 43: "III", 45: "II",
    51: "I", 53: "I", 54: "I", 57: "I", 58: "I", 60: "IV", 65: "III", 68: "III",
    75: "II", 77: "III", 78: "III", 80: "III", 83: "I", 85: "II", 86: "I", 89: "I",
    90: "IV", 92: "I", 99: "I", 101: "I", 102: "III", 105: "IV", 106: "I", 108: "I",
    113: "III", 114: "III", 120: "II", 135: "II", 141: "III", 142: "III", 147: "I",
    149: "I", 150: "IV", 153: "III", 154: "I", 156: "I", 163: "I", 165: "II",
    166: "I", 169: "I", 170: "II", 172: "I", 177: "III", 178: "III", 180: "II",
    195: "IV", 196: "III", 197: "I", 198: "I", 201: "I", 202: "I", 204: "I",
    210: "II", 212: "III", 216: "III", 225: "II", 228: "III", 232: "III", 240: "II",
    5: "II", 17: "I"
}

last_rule_table = {
    "I": [17, 20, 65, 68],
    "II": [5, 20, 65, 80],
    "III": [5, 17, 68, 80],
    "IV": [20, 65],
    "V": [17, 68],
    "VI": [5, 80],
}

class MaximalRCAGenerator:
    def __init__(self, n_cells: int, n_bits: int = 1, 
                 coverage_bonus: float = 2.0, 
                 diversity_weight: float = 0.1,
                 aim_for_full_coverage: bool = None):
        """
        Initialize maximal-length RCA generator.
        
        Args:
            n_cells: Number of cells in CA
            n_bits: Number of bits per cell (default 1 for binary CA)
            coverage_bonus: Weight for unvisited states (higher = prioritize exploration)
            diversity_weight: Weight for state diversity (higher = more variety)
            aim_for_full_coverage: Whether to aim for full state space coverage (auto-detect if None)
        """
        self.n_cells = n_cells
        self.n_bits = n_bits
        self.max_states = 2 ** (n_cells * n_bits)  # Total possible states
        self.visited_states: Set[Tuple[int, ...]] = set()
        self.state_history: List[Tuple[int, ...]] = []
        
        # Configurable scoring weights
        self.coverage_bonus = coverage_bonus
        self.diversity_weight = diversity_weight
        
        # Auto-determine if we should aim for full coverage
        if aim_for_full_coverage is None:
            # Aim for full coverage if state space is manageable (< 1024 states)
            self.aim_for_full_coverage = self.max_states <= 1024
        else:
            self.aim_for_full_coverage = aim_for_full_coverage
        
        print(f"Initialized RCA generator:")
        print(f"  State space: {self.max_states} possible states")
        print(f"  Full coverage mode: {self.aim_for_full_coverage}")
        print(f"  Scoring: coverage_bonus={coverage_bonus}, diversity_weight={diversity_weight}")
        
    def ca_step(self, state: List[int], rule: int) -> List[int]:
        """Apply CA rule to current state."""
        n = len(state)
        new_state = [0] * n
        
        # Convert rule to binary lookup table
        rule_table = [(rule >> i) & 1 for i in range(8)]
        
        for i in range(n):
            # Get 3-bit neighborhood (with periodic boundaries)
            left = state[(i-1) % n]
            center = state[i]
            right = state[(i+1) % n]
            
            # Convert to neighborhood index (0-7)
            neighborhood = (left << 2) | (center << 1) | right
            new_state[i] = rule_table[neighborhood]
            
        return new_state
    
    def state_to_tuple(self, state: List[int]) -> Tuple[int, ...]:
        """Convert state list to hashable tuple."""
        return tuple(state)
    
    def generate_initial_state(self, R1_class: str, strategy: str = "class_based") -> List[int]:
        """
        Generate better initial states based on R1_class and strategy.
        
        Args:
            R1_class: The starting class to influence initial state
            strategy: 'random', 'alternating', 'class_based', or 'diverse'
        """
        import random
        
        if strategy == "random":
            return [random.randint(0, (1 << self.n_bits) - 1) for _ in range(self.n_cells)]
        
        elif strategy == "alternating":
            return [i % 2 for i in range(self.n_cells)]  # Original method
        
        elif strategy == "class_based":
            # Use class characteristics to influence initial state
            class_seeds = {
                "I": 0b10101010,    # Alternating pattern
                "II": 0b11001100,   # Block pattern  
                "III": 0b11100011,  # Edge pattern
                "IV": 0b11110000,   # Half pattern
                "V": 0b11111000,    # Gradient pattern
                "VI": 0b10011001    # Mixed pattern
            }
            seed = class_seeds.get(R1_class, 0b10101010)
            return [(seed >> (i % 8)) & 1 for i in range(self.n_cells)]
        
        elif strategy == "diverse":
            # Generate state that maximizes initial diversity
            state = []
            for i in range(self.n_cells):
                # Use position-dependent pattern for diversity
                val = (i * 3 + hash(R1_class)) % 2
                state.append(val)
            return state
        
        else:
            raise ValueError(f"Unknown strategy: {strategy}")
    
    def calculate_state_coverage(self, sequence: List[int], initial_state: List[int]) -> float:
        """Calculate what fraction of state space is covered by this sequence."""
        current_state = initial_state[:]
        states_seen = set()
        
        for rule in sequence:
            state_tuple = self.state_to_tuple(current_state)
            states_seen.add(state_tuple)
            current_state = self.ca_step(current_state, rule)
        
        # Use actual max_states if aiming for full coverage
        denominator = self.max_states if self.aim_for_full_coverage else min(self.max_states, 10000)
        return len(states_seen) / denominator
    
    def pick_next_rule_maximal(self, prev_rule: int, current_state: List[int]) -> int:
        """
        Pick next rule to maximize state space coverage with improved scoring.
        """
        next_class = rule_to_nextclass[prev_rule]
        candidates = [r for r, classes in rule_to_classes.items() if next_class in classes]
        
        if not candidates:
            raise ValueError(f"No candidate rules for next_class {next_class}")
        
        best_rule = None
        best_score = -float('inf')
        
        for candidate_rule in candidates:
            # Test what new state this rule would produce
            test_state = self.ca_step(current_state, candidate_rule)
            state_tuple = self.state_to_tuple(test_state)
            
            score = 0
            
            # 1. Coverage bonus (prioritize unvisited states)
            if state_tuple not in self.visited_states:
                score += self.coverage_bonus
            else:
                # Small penalty for revisited states, scaled by visit frequency
                revisit_count = self.state_history.count(state_tuple)
                score += self.coverage_bonus * (0.1 / (1 + revisit_count))
            
            # 2. Diversity score (Hamming distance from recent states)
            if self.state_history:
                diversity_score = 0
                history_len = min(20, len(self.state_history))  # Check more history
                for prev_state in self.state_history[-history_len:]:
                    hamming_dist = sum(a != b for a, b in zip(state_tuple, prev_state))
                    diversity_score += hamming_dist / self.n_cells  # Normalize by state size
                
                # Average diversity over recent history
                avg_diversity = diversity_score / history_len
                score += self.diversity_weight * avg_diversity
            
            # 3. Full coverage bonus (extra incentive when close to full coverage)
            if self.aim_for_full_coverage:
                coverage_ratio = len(self.visited_states) / self.max_states
                if coverage_ratio > 0.8:  # When we're close to full coverage
                    full_coverage_bonus = (1 - coverage_ratio) * 2.0
                    score += full_coverage_bonus
            
            # 4. Rule diversity bonus (prefer less-used rules)
            rule_usage = sum(1 for r in self.state_history if r == candidate_rule)
            rule_diversity_bonus = 0.05 / (1 + rule_usage)
            score += rule_diversity_bonus
            
            if score > best_score:
                best_score = score
                best_rule = candidate_rule
        
        return best_rule or min(candidates)  # Fallback to deterministic choice
    
    def generate_maximal_rca(self, R1_class: str, max_length: Optional[int] = None, 
                           initial_strategy: str = "class_based") -> List[int]:
        """
        Generate maximal-length RCA sequence using state-space tracking.
        
        Args:
            R1_class: Starting class for R1
            max_length: Maximum sequence length (None for automatic)
            initial_strategy: Strategy for initial state generation
        
        Returns:
            List of CA rules forming maximal-length sequence
        """
        # Auto-determine max_length based on state space and coverage goals
        if max_length is None:
            if self.aim_for_full_coverage:
                # For full coverage, allow enough length to potentially visit all states
                max_length = min(self.max_states * 2, 5000)
            else:
                # For partial coverage, use reasonable default
                max_length = min(1000, self.max_states // 10)
        
        print(f"Target max_length: {max_length}")
        
        # Generate better initial state
        current_state = self.generate_initial_state(R1_class, initial_strategy)
        print(f"Initial state ({initial_strategy}): {current_state}")
        
        # Step 1: Pick first rule deterministically
        R0_candidates = [r for r, classes in rule_to_classes.items() if R1_class in classes]
        if not R0_candidates:
            raise ValueError(f"No candidates for R0 with R1_class={R1_class}")
        R0 = min(R0_candidates)
        
        sequence = [R0]
        prev_rule = R0
        
        # Apply first rule and track state
        current_state = self.ca_step(current_state, R0)
        state_tuple = self.state_to_tuple(current_state)
        self.visited_states.add(state_tuple)
        self.state_history.append(state_tuple)
        
        # Step 2: Generate intermediate rules with maximal coverage
        stagnation_counter = 0
        last_coverage = 0
        best_coverage = 0
        
        for i in range(1, max_length - 1):
            try:
                next_rule = self.pick_next_rule_maximal(prev_rule, current_state)
                sequence.append(next_rule)
                
                # Update state
                current_state = self.ca_step(current_state, next_rule)
                state_tuple = self.state_to_tuple(current_state)
                
                # Advanced stagnation detection
                current_coverage = len(self.visited_states) / (self.max_states if self.aim_for_full_coverage else min(self.max_states, 10000))
                
                if state_tuple in self.visited_states:
                    stagnation_counter += 1
                    # More aggressive stopping if we're not making coverage progress
                    if current_coverage == last_coverage:
                        stagnation_counter += 2  # Extra penalty for no coverage growth
                else:
                    stagnation_counter = max(0, stagnation_counter - 1)  # Decay stagnation
                    best_coverage = max(best_coverage, current_coverage)
                
                # Break conditions
                if self.aim_for_full_coverage and len(self.visited_states) == self.max_states:
                    print(f"🎉 FULL STATE SPACE COVERAGE achieved at length {len(sequence)}!")
                    break
                elif stagnation_counter > (100 if self.aim_for_full_coverage else 50):
                    print(f"Breaking due to stagnation at length {len(sequence)} (coverage: {current_coverage:.4f})")
                    break
                
                self.visited_states.add(state_tuple)
                self.state_history.append(state_tuple)
                prev_rule = next_rule
                last_coverage = current_coverage
                
                # Progress reporting with better metrics
                if i % (50 if self.aim_for_full_coverage else 100) == 0:
                    unique_rules = len(set(sequence))
                    rule_diversity = unique_rules / len(sequence)
                    print(f"Length {i}: Coverage {current_coverage:.4f} ({len(self.visited_states)}/{self.max_states if self.aim_for_full_coverage else 'target'}), "
                          f"Rule diversity: {rule_diversity:.3f}, Stagnation: {stagnation_counter}")
                
            except ValueError as e:
                print(f"Stopping early due to: {e}")
                break
        
        # Step 3: Add final rule using last_rule_table
        if len(sequence) > 1:
            last_classes = rule_to_classes[prev_rule]
            Rn = None
            for cls in last_classes:
                if cls in last_rule_table:
                    Rn = min(last_rule_table[cls])
                    break
            if Rn is not None:
                sequence.append(Rn)
        
        final_coverage = len(self.visited_states) / (self.max_states if self.aim_for_full_coverage else min(self.max_states, 10000))
        
        print(f"\n🏁 Generated maximal RCA sequence:")
        print(f"   Length: {len(sequence)}")
        print(f"   Unique states visited: {len(self.visited_states)}")
        print(f"   State space coverage: {final_coverage:.6f}")
        if self.aim_for_full_coverage:
            print(f"   Full coverage: {'✅ YES' if len(self.visited_states) == self.max_states else '❌ NO'}")
        
        return sequence
    
    def analyze_sequence_properties(self, sequence: List[int]) -> Dict:
        """Analyze properties of the generated sequence."""
        unique_rules = len(set(sequence))
        rule_distribution = {rule: sequence.count(rule) for rule in set(sequence)}
        
        # Class distribution
        class_counts = {}
        for rule in sequence:
            for cls in rule_to_classes.get(rule, []):
                class_counts[cls] = class_counts.get(cls, 0) + 1
        
        return {
            'length': len(sequence),
            'unique_rules': unique_rules,
            'rule_diversity': unique_rules / len(sequence),
            'rule_distribution': rule_distribution,
            'class_distribution': class_counts,
            'unique_states_visited': len(self.visited_states),
            'state_coverage': len(self.visited_states) / (self.max_states if self.aim_for_full_coverage else min(self.max_states, 10000))
        }

# Example usage
if __name__ == "__main__":
    print("=== MAXIMAL-LENGTH RCA GENERATOR ===\n")
    
    # Example 1: Small state space with full coverage
    print("Example 1: Full coverage on small state space")
    print("-" * 50)
    small_generator = MaximalRCAGenerator(n_cells=4, coverage_bonus=3.0, diversity_weight=0.1)
    sequence = small_generator.generate_maximal_rca("III", initial_strategy="class_based")
    
    properties = small_generator.analyze_sequence_properties(sequence)
    print(f"Results: Length={properties['length']}, Coverage={properties['state_coverage']:.4f}")
    print(f"Sequence preview: {sequence[:15]}...")
    
    # Example 2: Larger state space with optimized exploration
    print("\n\nExample 2: Optimized exploration on larger state space")
    print("-" * 50)
    large_generator = MaximalRCAGenerator(n_cells=6, coverage_bonus=5.0, diversity_weight=0.15)
    sequence2 = large_generator.generate_maximal_rca("II", max_length=1000, initial_strategy="diverse")
    
    properties2 = large_generator.analyze_sequence_properties(sequence2)
    print(f"Results: Length={properties2['length']}, Coverage={properties2['state_coverage']:.4f}")
    print(f"Unique states visited: {properties2['unique_states_visited']}")
    
    # Example 3: Strategy comparison
    print("\n\nExample 3: Comparing initial state strategies")
    print("-" * 50)
    strategies = ["alternating", "class_based", "diverse", "random"]
    for strategy in strategies:
        gen = MaximalRCAGenerator(n_cells=5, coverage_bonus=2.5, diversity_weight=0.1)
        seq = gen.generate_maximal_rca("I", max_length=300, initial_strategy=strategy)
        coverage = len(gen.visited_states) / gen.max_states
        print(f"{strategy:12}: Length={len(seq):3d}, Coverage={coverage:.4f}")