from flask import Flask, render_template, request, jsonify
import pandas as pd
from collections import defaultdict, deque
import heapq
import os

app = Flask(__name__)

DATA_PATH = os.path.join(os.path.dirname(__file__), '..', 'data', 'facebook_combined.txt')
NAMES_PATH = os.path.join(os.path.dirname(__file__), '..', 'data', 'names.txt')

# Global graph data
adj = defaultdict(list)
nodes = set()
edges = []
node_names = {}
name_to_id = {}


def load_data():
    global adj, nodes, edges, node_names, name_to_id
    adj = defaultdict(list)
    nodes = set()
    edges = []

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
                adj[u].append(v)
                adj[v].append(u)

    # Load names
    node_names = {}
    name_to_id = {}
    if os.path.exists(NAMES_PATH):
        with open(NAMES_PATH, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                parts = line.split()
                if len(parts) >= 3:
                    nid = int(parts[0])
                    name = " ".join(parts[1:])
                    node_names[nid] = name
                    name_to_id[name.lower()] = nid

    for n in nodes:
        if n not in node_names:
            node_names[n] = f"Node {n}"


def get_name(nid):
    return node_names.get(nid, f"Node {nid}")


def resolve_input(text):
    text = text.strip()
    # Try numeric ID
    try:
        nid = int(text)
        if nid in nodes:
            return nid
    except ValueError:
        pass
    # Try name
    lower = text.lower()
    if lower in name_to_id:
        return name_to_id[lower]
    # Partial match
    matches = [(nid, name) for nid, name in node_names.items() if lower in name.lower()]
    if len(matches) == 1:
        return matches[0][0]
    return None


load_data()


@app.route("/")
def index():
    total_nodes = len(nodes)
    total_edges = len(edges)
    degrees = {n: len(adj[n]) for n in nodes}
    avg_deg = round(sum(degrees.values()) / total_nodes, 2) if total_nodes else 0
    max_deg = max(degrees.values()) if degrees else 0
    min_deg = min(degrees.values()) if degrees else 0
    max_node = max(degrees, key=degrees.get) if degrees else None
    min_node = min(degrees, key=degrees.get) if degrees else None

    return render_template("index.html",
                           total_nodes=total_nodes,
                           total_edges=total_edges,
                           avg_degree=avg_deg,
                           max_degree=max_deg,
                           min_degree=min_deg,
                           max_node=get_name(max_node) if max_node else "-",
                           min_node=get_name(min_node) if min_node else "-")


@app.route("/api/stats")
def api_stats():
    degrees = {n: len(adj[n]) for n in nodes}
    return jsonify({
        "total_nodes": len(nodes),
        "total_edges": len(edges),
        "avg_degree": round(sum(degrees.values()) / len(nodes), 2) if nodes else 0,
        "max_degree": max(degrees.values()) if degrees else 0,
        "min_degree": min(degrees.values()) if degrees else 0,
        "density": round(len(edges) / (len(nodes) * (len(nodes) - 1) / 2), 6) if len(nodes) > 1 else 0
    })


@app.route("/api/bfs", methods=["POST"])
def api_bfs():
    data = request.get_json()
    start = resolve_input(data.get("start", ""))
    if start is None:
        return jsonify({"error": "Node not found"}), 404

    visited = set([start])
    queue = deque([start])
    order = []

    while queue:
        curr = queue.popleft()
        order.append(curr)
        for nb in adj[curr]:
            if nb not in visited:
                visited.add(nb)
                queue.append(nb)

    return jsonify({
        "traversal": [{"id": n, "name": get_name(n)} for n in order],
        "total_visited": len(order)
    })


@app.route("/api/dfs", methods=["POST"])
def api_dfs():
    data = request.get_json()
    start = resolve_input(data.get("start", ""))
    if start is None:
        return jsonify({"error": "Node not found"}), 404

    visited = set()
    stack = [start]
    order = []

    while stack:
        curr = stack.pop()
        if curr in visited:
            continue
        visited.add(curr)
        order.append(curr)
        for nb in reversed(sorted(adj[curr])):
            if nb not in visited:
                stack.append(nb)

    return jsonify({
        "traversal": [{"id": n, "name": get_name(n)} for n in order],
        "total_visited": len(order)
    })


@app.route("/api/shortest_path", methods=["POST"])
def api_shortest_path():
    data = request.get_json()
    src = resolve_input(data.get("src", ""))
    dst = resolve_input(data.get("dst", ""))
    if src is None or dst is None:
        return jsonify({"error": "One or both nodes not found"}), 404
    if src == dst:
        return jsonify({"path": [{"id": src, "name": get_name(src)}], "distance": 0})

    # Bidirectional BFS
    q_src = deque([src])
    q_dst = deque([dst])
    parent_src = {src: None}
    parent_dst = {dst: None}
    meeting = None

    while q_src and q_dst and meeting is None:
        if len(q_src) <= len(q_dst):
            for _ in range(len(q_src)):
                curr = q_src.popleft()
                for nb in adj[curr]:
                    if nb in parent_dst:
                        parent_src[nb] = curr
                        meeting = nb
                        break
                    if nb not in parent_src:
                        parent_src[nb] = curr
                        q_src.append(nb)
                if meeting:
                    break
        else:
            for _ in range(len(q_dst)):
                curr = q_dst.popleft()
                for nb in adj[curr]:
                    if nb in parent_src:
                        parent_dst[nb] = curr
                        meeting = nb
                        break
                    if nb not in parent_dst:
                        parent_dst[nb] = curr
                        q_dst.append(nb)
                if meeting:
                    break

    if meeting is None:
        return jsonify({"path": [], "distance": -1, "message": "No path found"})

    path = []
    curr = meeting
    while curr is not None:
        path.append(curr)
        curr = parent_src[curr]
    path.reverse()

    curr = parent_dst[meeting]
    while curr is not None:
        path.append(curr)
        curr = parent_dst[curr]

    return jsonify({
        "path": [{"id": n, "name": get_name(n)} for n in path],
        "distance": len(path) - 1
    })


@app.route("/api/components")
def api_components():
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

    components.sort(key=len, reverse=True)
    return jsonify({
        "count": len(components),
        "largest_size": len(components[0]) if components else 0,
        "components": [[{"id": n, "name": get_name(n)} for n in comp[:5]] for comp in components[:5]]
    })


@app.route("/api/pagerank")
def api_pagerank():
    n = len(nodes)
    if n == 0:
        return jsonify({"top": []})

    damping = 0.85
    iterations = 20
    scores = {node: 1.0 / n for node in nodes}
    inv_deg = {node: (1.0 / len(adj[node])) if len(adj[node]) > 0 else 0 for node in nodes}

    for _ in range(iterations):
        new_scores = {}
        base = (1.0 - damping) / n
        for v in nodes:
            incoming = sum(scores[u] * inv_deg[u] for u in adj[v])
            new_scores[v] = base + damping * incoming
        scores = new_scores

    top = sorted(scores.items(), key=lambda x: x[1], reverse=True)[:10]
    return jsonify({"top": [{"id": nid, "name": get_name(nid), "score": round(score, 6)} for nid, score in top]})


@app.route("/api/top_degree", methods=["POST"])
def api_top_degree():
    data = request.get_json()
    k = int(data.get("k", 10))
    degrees = [(n, len(adj[n])) for n in nodes]
    degrees.sort(key=lambda x: (-x[1], x[0]))
    top = degrees[:k]
    return jsonify({"top": [{"id": nid, "name": get_name(nid), "degree": deg} for nid, deg in top]})


@app.route("/api/suggestions", methods=["POST"])
def api_suggestions():
    data = request.get_json()
    node = resolve_input(data.get("node", ""))
    if node is None:
        return jsonify({"error": "Node not found"}), 404

    direct = set(adj[node])
    mutual = defaultdict(int)

    for nb in adj[node]:
        for cand in adj[nb]:
            if cand != node and cand not in direct:
                mutual[cand] += 1

    suggestions = sorted(mutual.items(), key=lambda x: (-x[1], x[0]))[:10]
    return jsonify({
        "suggestions": [{"id": nid, "name": get_name(nid), "mutual": count} for nid, count in suggestions]
    })


@app.route("/api/search_name", methods=["POST"])
def api_search_name():
    data = request.get_json()
    query = data.get("query", "").strip().lower()
    if not query:
        return jsonify({"results": []})

    results = [(nid, name) for nid, name in node_names.items() if query in name.lower()]
    results.sort(key=lambda x: x[0])
    return jsonify({"results": [{"id": nid, "name": name, "degree": len(adj[nid])} for nid, name in results[:20]]})


@app.route("/api/node_info", methods=["POST"])
def api_node_info():
    data = request.get_json()
    node = resolve_input(data.get("node", ""))
    if node is None:
        return jsonify({"error": "Node not found"}), 404

    neighbors = [{"id": nb, "name": get_name(nb)} for nb in adj[node]]
    return jsonify({
        "id": node,
        "name": get_name(node),
        "degree": len(adj[node]),
        "neighbors": neighbors[:20],
        "total_neighbors": len(neighbors)
    })


@app.route("/api/check_connection", methods=["POST"])
def api_check_connection():
    data = request.get_json()
    u = resolve_input(data.get("u", ""))
    v = resolve_input(data.get("v", ""))
    if u is None or v is None:
        return jsonify({"error": "One or both nodes not found"}), 404

    direct = v in adj[u]

    # Component check via BFS from u
    visited = set([u])
    queue = deque([u])
    connected = False
    while queue:
        curr = queue.popleft()
        if curr == v:
            connected = True
            break
        for nb in adj[curr]:
            if nb not in visited:
                visited.add(nb)
                queue.append(nb)

    return jsonify({
        "u": {"id": u, "name": get_name(u)},
        "v": {"id": v, "name": get_name(v)},
        "direct": direct,
        "connected": connected
    })


if __name__ == "__main__":
    app.run(debug=True, host="127.0.0.1", port=5000)
