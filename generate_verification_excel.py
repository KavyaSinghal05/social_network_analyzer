import pandas as pd
from collections import defaultdict
import os

DATA_PATH = "data/facebook_combined.txt"
OUTPUT_PATH = "facebook_verification.xlsx"

# Read edges
edges = []
nodes = set()
with open(DATA_PATH, "r") as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        if len(parts) >= 2:
            u, v = int(parts[0]), int(parts[1])
            edges.append((u, v))
            nodes.add(u)
            nodes.add(v)

edges = sorted(set(tuple(sorted(e)) for e in edges))  # unique undirected edges
num_nodes = len(nodes)
num_edges = len(edges)

# Compute degrees
degree = defaultdict(int)
for u, v in edges:
    degree[u] += 1
    degree[v] += 1

# Build DataFrames
df_edges = pd.DataFrame(edges, columns=["Source", "Target"])
df_nodes = pd.DataFrame(
    [(n, degree.get(n, 0)) for n in sorted(nodes)],
    columns=["NodeID", "Degree"]
).sort_values("NodeID")

# Summary stats
avg_degree = (2.0 * num_edges) / num_nodes if num_nodes > 0 else 0
max_deg_node = max(degree, key=degree.get) if degree else None
min_deg_node = min(degree, key=degree.get) if degree else None

summary = pd.DataFrame([
    ["Total Unique Nodes", num_nodes],
    ["Total Undirected Edges", num_edges],
    ["Average Degree", round(avg_degree, 2)],
    ["Maximum Degree", degree.get(max_deg_node, 0)],
    ["Max Degree Node ID", max_deg_node],
    ["Minimum Degree", degree.get(min_deg_node, 0)],
    ["Min Degree Node ID", min_deg_node],
], columns=["Metric", "Value"])

# Connected components via simple BFS (independent check)
adj = defaultdict(list)
for u, v in edges:
    adj[u].append(v)
    adj[v].append(u)

visited = set()
components = []
for node in nodes:
    if node not in visited:
        comp = []
        stack = [node]
        visited.add(node)
        while stack:
            cur = stack.pop()
            comp.append(cur)
            for nb in adj[cur]:
                if nb not in visited:
                    visited.add(nb)
                    stack.append(nb)
        components.append(comp)

component_summary = pd.DataFrame([
    ["Number of Connected Components", len(components)],
    ["Largest Component Size", max(len(c) for c in components) if components else 0],
    ["Smallest Component Size", min(len(c) for c in components) if components else 0],
], columns=["Metric", "Value"])

# Write to Excel with multiple sheets
with pd.ExcelWriter(OUTPUT_PATH, engine="openpyxl") as writer:
    summary.to_excel(writer, sheet_name="Summary", index=False)
    component_summary.to_excel(writer, sheet_name="Components", index=False)
    df_nodes.to_excel(writer, sheet_name="NodeDegrees", index=False)
    df_edges.to_excel(writer, sheet_name="Edges", index=False)

print(f"Created: {OUTPUT_PATH}")
print(f"Nodes: {num_nodes}, Edges: {num_edges}")
print(f"Avg Degree: {avg_degree:.2f}")
print(f"Max Degree: {degree.get(max_deg_node, 0)} (Node {max_deg_node})")
print(f"Min Degree: {degree.get(min_deg_node, 0)} (Node {min_deg_node})")
print(f"Connected Components: {len(components)}")
