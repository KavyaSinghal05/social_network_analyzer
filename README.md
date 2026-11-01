# Social Network Graph Analyzer

**DSA Project | B.Tech V Semester | JIIT Noida**

A terminal-based menu application that analyzes the SNAP Facebook social network
dataset using a high-performance **Compressed Sparse Row (CSR)** graph
representation. Implements classic graph algorithms: BFS/DFS traversal,
bidirectional BFS shortest path, Union-Find connected components, iterative
PageRank, degree centrality, and 2-hop friend suggestions.

## Features (Main Menu)

| # | Feature | Description |
|---|---------|-------------|
| 1 | Graph Statistics | Nodes, edges, avg/max/min degree, density, CSR memory usage |
| 2 | BFS Traversal | Breadth-first traversal from any node (name or ID) |
| 3 | DFS Traversal | Iterative depth-first traversal (stack-based, no recursion limit) |
| 4 | Shortest Path | Bidirectional BFS between two nodes, prints hop-by-hop path |
| 5 | Connected Components | Union-Find with path compression & union by rank; top components |
| 6 | PageRank Analysis | Iterative PageRank (20 iterations, damping 0.85), top-10 ranking |
| 7 | Top Nodes by Degree | Top-K degree centrality via min-heap priority queue |
| 8 | Friend Suggestions | 2-hop "friend of a friend" triadic-closure recommendations |
| 9 | Search Node | Full node metadata (degree, PageRank, neighbor sample) |
| 10 | Check Connection | Direct edge, same-component, reachability between two nodes |
| 11 | Find Neighbors | Lists all direct friends of a node |
| 12 | Search by Name | Partial case-insensitive name search across the network |
| 0 | Exit | Quit the application |

Users may identify nodes either by raw dataset ID or by person's name
(e.g., `"Divya Choudhary"`). Partial name matches show suggestions.

## Dataset

- `data/facebook_combined.txt` — SNAP Facebook ego-network (4,039 nodes,
  88,234 undirected edges)
- `data/names.txt` — generated name mapping (`raw_id FirstName LastName`)
- `generate_names.py` — script used to generate the name mappings

## Build

### Using the Makefile (Linux / MinGW / MSYS2 / Git Bash)

```bash
make
./social_network
```

### Manual compile (any C++11 compiler)

```bash
g++ -std=c++11 -O2 -Wall src/graph.cpp src/main.cpp -o social_network
./social_network
```

### Windows (Command Prompt / PowerShell)

```powershell
g++ -std=c++11 -O2 -Wall src\graph.cpp src\main.cpp -o social_network.exe
.\social_network.exe
```

## Project Structure

```
socialnetwork/
├── Makefile                  # Build automation
├── README.md                 # This file
├── generate_names.py         # Name-mapping generator script
├── synopsis_final.pdf        # Project synopsis / report
├── social_network.exe        # Built executable (Windows)
├── src/
│   ├── graph.h               # SocialGraph class declaration (CSR + algorithms)
│   ├── graph.cpp             # Algorithm implementations
│   └── main.cpp              # Terminal menu UI and entry point
└── data/
    ├── facebook_combined.txt # SNAP edge-list dataset
    ├── facebook_combined.txt.gz
    └── names.txt             # Node ID -> name mappings
```

## Complexity Highlights

- **CSR storage**: O(V + E) memory, cache-friendly contiguous layout
- **Degree lookup**: O(1) via `row_ptr` differences
- **Edge existence**: O(log degree) via binary search on sorted neighbor lists
- **BFS/DFS**: O(V + E) traversal
- **Bidirectional BFS**: ~O(b^(d/2)) instead of O(b^d)
- **Union-Find components**: near-linear O(E · α(V))
- **PageRank**: O(iterations · (V + E)) with ping-pong buffers
- **Top-K degree/PageRank**: O(N log K) via min-heap

## Notes

- The program loads `data/facebook_combined.txt` and `data/names.txt` using
  paths relative to the working directory, so run the executable from the
  project root.
- If `names.txt` is missing, the app falls back to displaying numeric IDs.
- Pre-built executables `social_network.exe` and `social_network_v2.exe` are
  included for convenience; rebuild from source with `make` to match the
  current code.
