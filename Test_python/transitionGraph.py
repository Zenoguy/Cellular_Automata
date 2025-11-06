"""
State Transition Graph analysis for 1D Elementary Cellular Automata (ECA)

Features:
- build_state_graph(rule, n): build directed graph of 2^n states (ring)
- compute_graph_metrics(G): returns dictionary of topological/attractor metrics
- plot_state_graph(G, n, figsize): plot whole graph (best for small n)
- plot_attractor_basins(G, n): plot each attractor + basin separately
- compare_rule_groups(success_rules, failed_rules, n): compute metrics for each rule and compare groups (summary & boxplots)

Usage:
- set rules and ring size n
- optionally adjust plotting / export behavior
"""

import itertools
from collections import deque, defaultdict
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

# ----------------------------
# Utility: convert rule number to lookup table
# ----------------------------
def rule_to_lookup(rule_num):
    """
    Convert ECA rule number (0-255) to a dictionary mapping 3-bit neighborhoods (as int 0..7)
    to outputs (0 or 1).
    MSB of the 8-bit binary corresponds to neighborhood 7 (111), LSB to neighborhood 0 (000).
    """
    bits = [(rule_num >> i) & 1 for i in range(8)]  # LSB..MSB
    # bits[i] = output for neighborhood i (0..7) where i corresponds to binary abc -> a*4+b*2+c
    # convert to mapping
    lookup = {i: bits[i] for i in range(8)}
    return lookup

# ----------------------------
# Utility: evolve a configuration once (ring)
# ----------------------------
def evolve_once(config_bits, lookup):
    """
    config_bits: tuple/list of 0/1 of length n
    lookup: mapping neighborhood int (0..7) -> 0/1
    returns next configuration tuple
    """
    n = len(config_bits)
    nxt = [0] * n
    for i in range(n):
        left = config_bits[(i-1) % n]
        center = config_bits[i]
        right = config_bits[(i+1) % n]
        neigh = (left << 2) | (center << 1) | right
        nxt[i] = lookup[neigh]
    return tuple(nxt)

# ----------------------------
# Build full state transition graph for finite ring of size n
# ----------------------------
def build_state_graph(rule_num, n):
    """
    Returns a directed graph G with 2^n nodes (each node: integer 0..2^n - 1) and edge u->v (deterministic)
    Also returns a mapping int->tuple_of_bits for convenience.
    """
    lookup = rule_to_lookup(rule_num)
    N = 1 << n
    G = nx.DiGraph()
    int_to_bits = {}
    for s in range(N):
        bits = tuple(((s >> (n-1-i)) & 1) for i in range(n))  # big-endian: leftmost is highest bit
        int_to_bits[s] = bits
        G.add_node(s)
    # add edges
    for s, bits in int_to_bits.items():
        nxt = evolve_once(bits, lookup)
        v = 0
        for b in nxt:
            v = (v << 1) | b
        G.add_edge(s, v)
    return G, int_to_bits

# ----------------------------
# Graph metrics & attractor analysis
# ----------------------------
def compute_graph_metrics(G):
    """
    Compute attractors (cycles), transient lengths to attractors, basin sizes, components, degree stats.
    Returns a dictionary of metrics.
    """
    N = G.number_of_nodes()
    # Since deterministic, out-degree of every node is 1.
    # Compute strongly connected components (SCC). Each SCC that has size >1 or a self-loop is an attractor (cycle).
    sccs = list(nx.strongly_connected_components(G))
    attractors = []
    for comp in sccs:
        # check if comp is a cycle/attractor: either size>1 or single node with self-loop
        if len(comp) > 1:
            attractors.append(set(comp))
        else:
            node = next(iter(comp))
            if G.has_edge(node, node):
                attractors.append({node})
    # compute cycle lengths
    cycle_lengths = [len(c) for c in attractors]
    # find for each node its attractor and transient length (distance to first node in cycle)
    node_to_attractor = {}
    node_transient_len = {}
    node_distance = {}
    # Precompute by iterating each node until hitting seen node (tortoise-hare not necessary for small graphs)
    for node in G.nodes():
        visited = {}
        cur = node
        step = 0
        while cur not in visited:
            visited[cur] = step
            next_node = next(G.successors(cur))
            cur = next_node
            step += 1
        # loop detected: start of loop = cur, loop_start_step = visited[cur]
        loop_start_step = visited[cur]
        # find attractor set (cycle nodes)
        cycle_nodes = set(k for k,v in visited.items() if v >= loop_start_step)
        # record metrics for original node
        node_to_attractor[node] = tuple(sorted(cycle_nodes))
        node_transient_len[node] = loop_start_step
        node_distance[node] = visited[cur]  # the number of steps until loop started for the path starting from node
    # basin sizes: how many nodes lead to each attractor (use attractor repr as tuple)
    basin_counts = defaultdict(int)
    for node, attr in node_to_attractor.items():
        basin_counts[attr] += 1
    basins = sorted([(attr, basin_counts[attr], len(attr)) for attr in basin_counts], key=lambda x: (-x[1], -x[2]))
    # degrees
    indegs = [d for n, d in G.in_degree()]
    outdegs = [d for n, d in G.out_degree()]
    metrics = {
        "N": N,
        "num_scc": len(sccs),
        "num_attractors": len(attractors),
        "cycle_lengths": cycle_lengths,
        "basins": basins,
        "indegree_stats": {
            "min": int(np.min(indegs)),
            "max": int(np.max(indegs)),
            "mean": float(np.mean(indegs)),
            "median": float(np.median(indegs))
        },
        "outdegree_stats": {
            "min": int(np.min(outdegs)),
            "max": int(np.max(outdegs)),
            "mean": float(np.mean(outdegs)),
            "median": float(np.median(outdegs))
        },
        "node_to_attractor": node_to_attractor,
        "node_transient_len": node_transient_len
    }
    return metrics

# ----------------------------
# Visualization helpers
# ----------------------------
def plot_state_graph(G, int_to_bits, rule_num, n, figsize=(10,8), node_size=300, with_labels=False):
    """
    Plot the entire state transition graph. Best for small n (e.g., n <= 6).
    Node label = bitstring.
    """
    pos = nx.spring_layout(G, seed=42)  # layout
    plt.figure(figsize=figsize)
    nx.draw_networkx_nodes(G, pos, node_size=node_size)
    nx.draw_networkx_edges(G, pos, arrowsize=12, arrowstyle='-|>')
    if with_labels:
        labels = {s: ''.join(str(b) for b in bits) for s, bits in int_to_bits.items()}
        nx.draw_networkx_labels(G, pos, labels=labels, font_size=8)
    plt.title(f"State transition graph for rule {rule_num} (n={n}, N={G.number_of_nodes()})")
    plt.axis('off')
    plt.show()

def plot_attractor_basins(G, int_to_bits, metrics, rule_num, n):
    """
    For each attractor (basin), plot the subgraph of its basin colored by distance to attractor
    (transient length).
    """
    node_to_attr = metrics['node_to_attractor']
    transient = metrics['node_transient_len']
    # group nodes by attractor tuple
    groups = defaultdict(list)
    for node, attr in node_to_attr.items():
        groups[attr].append(node)
    # Plot each basin
    num = len(groups)
    cols = min(3, num)
    rows = (num + cols - 1) // cols
    plt.figure(figsize=(5*cols, 4*rows))
    idx = 1
    for attr, nodes in groups.items():
        subG = G.subgraph(nodes).copy()
        # color by transient length (distance to cycle)
        max_t = max(transient[n] for n in nodes)
        colors = [transient[n] for n in subG.nodes()]
        pos = nx.spring_layout(subG, seed=42)
        plt.subplot(rows, cols, idx)
        nx.draw_networkx_nodes(subG, pos, node_size=300, cmap='viridis', node_color=colors)
        nx.draw_networkx_edges(subG, pos, arrowsize=12)
        labels = {s: ''.join(str(b) for b in int_to_bits[s]) for s in subG.nodes()}
        nx.draw_networkx_labels(subG, pos, labels=labels, font_size=8)
        plt.title(f"Attractor (size {len(attr)}) - basin {len(nodes)} nodes")
        plt.axis('off')
        idx += 1
    plt.suptitle(f"Attractor Basins for rule {rule_num} (n={n})")
    plt.show()

# ----------------------------
# Compare groups of rules
# ----------------------------
def compare_rule_groups(success_rules, failed_rules, n, verbose=True):
    """
    For every rule in each group, build state graph and compute metrics.
    Then summarize distributions of key metrics and plot comparisons.
    Returns a dict with detailed metrics per rule.
    """
    groups = {"success": success_rules, "failed": failed_rules}
    results = {}
    summary = {}
    for gname, rlist in groups.items():
        results[gname] = {}
        summary[gname] = {
            "num_attractors": [],
            "avg_cycle_length": [],
            "max_cycle_length": [],
            "num_scc": [],
            "mean_indegree": [],
            "mean_transient": [],
            "largest_basin": []
        }
        for r in rlist:
            G, int_to_bits = build_state_graph(r, n)
            metrics = compute_graph_metrics(G)
            # derived
            clens = metrics['cycle_lengths'] if metrics['cycle_lengths'] else [0]
            mean_cycle = float(np.mean(clens))
            max_cycle = int(np.max(clens)) if clens else 0
            mean_transient = float(np.mean(list(metrics['node_transient_len'].values())))
            largest_basin = metrics['basins'][0][1] if metrics['basins'] else 0
            # record
            summary[gname]["num_attractors"].append(metrics['num_attractors'])
            summary[gname]["avg_cycle_length"].append(mean_cycle)
            summary[gname]["max_cycle_length"].append(max_cycle)
            summary[gname]["num_scc"].append(metrics['num_scc'])
            summary[gname]["mean_indegree"].append(metrics['indegree_stats']['mean'])
            summary[gname]["mean_transient"].append(mean_transient)
            summary[gname]["largest_basin"].append(largest_basin)
            results[gname][r] = {
                "metrics": metrics,
                "int_to_bits": int_to_bits
            }
            if verbose:
                print(f"Rule {r} (n={n}): N={metrics['N']}, #attractors={metrics['num_attractors']}, cycles={metrics['cycle_lengths']}, largest_basin={largest_basin}")
    # plotting comparison boxplots for a few key stats
    plt.figure(figsize=(12,8))
    keys = ["num_attractors", "avg_cycle_length", "max_cycle_length", "largest_basin", "mean_transient"]
    for i, key in enumerate(keys, start=1):
        plt.subplot(2, 3, i)
        data = [summary["success"][key], summary["failed"][key]]
        plt.boxplot(data, labels=["success", "failed"])
        plt.title(key)
    plt.tight_layout()
    plt.suptitle(f"Comparison of rule groups (n={n})", y=1.02)
    plt.show()

    return results, summary

# ----------------------------
# Example usage
# ----------------------------
if __name__ == "__main__":
    # RULE SET — replace or edit as you like
    rules = [30, 45, 75, 120, 135, 160, 210, 225]

    # Example: label rules as successful vs failed (you control this).
    # For the demo I put an arbitrary split: you can change that.
    successful_rules = [30, 45, 75, 120, 135]   # example set (edit as needed)
    failed_rules     = [160, 210, 225]          # example set (edit as needed)

    # Choose ring size for state graph analysis. n=6 -> 64 nodes; n=7 -> 128 nodes; n=8 -> 256 nodes
    n = 6

    # Compare groups (build graphs and compute metrics). This prints summaries & shows boxplots.
    results, summary = compare_rule_groups(successful_rules, failed_rules, n)

    # If you want to examine a single rule in detail & visualize:
    example_rule = 30
    G, int_to_bits = build_state_graph(example_rule, n)
    metrics = compute_graph_metrics(G)
    print("\nDetailed metrics for rule", example_rule)
    for k,v in metrics.items():
        if k in ('node_to_attractor', 'node_transient_len'):
            continue
        print(f"{k}: {v}")
    # Visualize whole graph (small n recommended)
    plot_state_graph(G, int_to_bits, example_rule, n, with_labels=False)
    # Visualize basins and attractors
    plot_attractor_basins(G, int_to_bits, metrics, example_rule, n)
