# Web Frontend — Social Network Graph Analyzer

This is a separate Flask-based web dashboard for presenting the Social Network Graph Analyzer project. It does **not** modify any existing C++ files.

## Features
- Graph Statistics dashboard
- BFS / DFS Traversal (visual node sequence)
- Shortest Path using Bidirectional BFS
- PageRank centrality (top 10)
- 2-hop Friend Suggestions
- Name search across all nodes
- Connected Components (Union-Find)
- Top-K Degree Centrality
- Degree Distribution Chart

## How to Run

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Start the server:
```bash
python app.py
```

3. Open your browser to:
```
http://127.0.0.1:5000
```

## Notes
- Reads the same `data/facebook_combined.txt` and `data/names.txt` files as the C++ version.
- All C++ source files (`src/`) remain unchanged.
- You can run both the terminal version (`social_network.exe`) and this web version at the same time.
