

#include "graph.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <limits>
#include <cstdlib>
#include <algorithm>

const std::string SEPARATOR = "------------------------------------------------------------";
const std::string DOUBLE_SEPARATOR = "============================================================";

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pausePrompt() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int readNodeInput(const SocialGraph& graph, const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, input);

    size_t start = input.find_first_not_of(" \t");
    size_t end = input.find_last_not_of(" \t");
    if (start == std::string::npos) {
        return -1;
    }
    input = input.substr(start, end - start + 1);

    int nodeId = graph.resolveNodeInput(input);
    if (nodeId == -1) {

        auto matches = graph.searchNodesByName(input, 5);
        if (!matches.empty()) {
            std::cout << "\n  Did you mean one of these?\n";
            for (const auto& m : matches) {
                std::cout << "    - " << m.second << " (ID: " << m.first << ")\n";
            }
        }
        std::cout << "  Error: Could not find node '" << input << "'.\n";
    }
    return nodeId;
}

void printBanner(const SocialGraph& graph) {
    std::cout << DOUBLE_SEPARATOR << "\n";
    std::cout << "     SOCIAL NETWORK GRAPH ANALYZER\n";
    std::cout << "     DSA Project | B.Tech V Semester | JIIT Noida\n";
    std::cout << DOUBLE_SEPARATOR << "\n";
    std::cout << "  Dataset:     SNAP Facebook (data/facebook_combined.txt)\n";
    std::cout << "  Nodes:       " << std::setw(6) << graph.numNodes
              << "  | Edges:       " << std::setw(6) << graph.numEdges << "\n";
    std::cout << "  Avg Degree:  " << std::setw(6) << std::fixed << std::setprecision(2) << graph.getAverageDegree()
              << "  | Storage:     Compressed Sparse Row (CSR)\n";
    std::cout << "  Max Degree:  " << std::setw(6) << graph.getMaxDegree()
              << "  | Min Degree:  " << std::setw(6) << graph.getMinDegree() << "\n";
    std::cout << DOUBLE_SEPARATOR << "\n";
}

void printMenu() {
    std::cout << "\n" << SEPARATOR << "\n";
    std::cout << "                        MAIN MENU\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "   1.  Graph Statistics\n";
    std::cout << "   2.  BFS Traversal\n";
    std::cout << "   3.  DFS Traversal\n";
    std::cout << "   4.  Shortest Path (Bidirectional BFS)\n";
    std::cout << "   5.  Connected Components (Union-Find)\n";
    std::cout << "   6.  PageRank Analysis\n";
    std::cout << "   7.  Top Nodes by Degree\n";
    std::cout << "   8.  Friend Suggestions (2-Hop)\n";
    std::cout << "   9.  Search Node\n";
    std::cout << "   10. Check Connection\n";
    std::cout << "   11. Find Neighbors\n";
    std::cout << "   12. Search by Name\n";
    std::cout << "   0.  Exit\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  Enter your choice (0-12): ";
}

void displayTraversal(const std::vector<int>& traversal, const SocialGraph& graph) {
    if (traversal.empty()) {
        std::cout << "  (No nodes visited)\n";
        return;
    }

    for (size_t i = 0; i < traversal.size(); ++i) {
        std::cout << "  Step " << std::setw(3) << (i + 1) << ": "
                  << graph.getNodeName(traversal[i])
                  << " (ID: " << traversal[i] << ")";
        if (i + 1 < traversal.size()) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void handleGraphStats(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  GRAPH STATISTICS\n";
    std::cout << SEPARATOR << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    graph.printStats();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleBFS(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  BFS TRAVERSAL (Breadth-First Search)\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int startNode = readNodeInput(graph, "  Enter start node: ");
    if (startNode == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    std::cout << "  Starting BFS from: " << graph.getNodeName(startNode) << " (ID: " << startNode << ")\n";

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> traversal = graph.bfs(startNode, 50);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  BFS Traversal Order (showing first " << traversal.size() << " nodes):\n";
    displayTraversal(traversal, graph);

    std::cout << "\n  Total Visited: " << traversal.size() << " nodes\n";
    std::cout << "  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleDFS(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  DFS TRAVERSAL (Depth-First Search)\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int startNode = readNodeInput(graph, "  Enter start node: ");
    if (startNode == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    std::cout << "  Starting DFS from: " << graph.getNodeName(startNode) << " (ID: " << startNode << ")\n";

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> traversal = graph.dfs(startNode, 50);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  DFS Traversal Order (showing first " << traversal.size() << " nodes):\n";
    displayTraversal(traversal, graph);

    std::cout << "\n  Total Visited: " << traversal.size() << " nodes\n";
    std::cout << "  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleShortestPath(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  SHORTEST PATH (Bidirectional BFS)\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int src = readNodeInput(graph, "  Enter source node: ");
    if (src == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    int dst = readNodeInput(graph, "  Enter destination node: ");
    if (dst == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    std::cout << "  Finding path: " << graph.getNodeName(src) << " -> " << graph.getNodeName(dst) << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> path = graph.shortestPath(src, dst);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n";
    if (path.empty()) {
        std::cout << "  No path found! " << graph.getNodeName(src) << " and " << graph.getNodeName(dst)
                  << " are in disconnected components.\n";
    } else {
        int distance = static_cast<int>(path.size()) - 1;
        std::cout << "  Path found! Distance: " << distance << " hops\n\n";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << graph.getNodeName(path[i]) << " (ID: " << path[i] << ")";
            if (i + 1 < path.size()) {
                std::cout << "\n       |\n       v\n";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleConnectedComponents(SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  CONNECTED COMPONENTS (Union-Find / Disjoint Set Union)\n";
    std::cout << SEPARATOR << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    int numComponents = graph.computeComponents();
    std::vector<std::pair<int, int>> sizes = graph.getComponentSizes();

    std::sort(sizes.begin(), sizes.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.second > b.second;
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "  Algorithm: Disjoint Set Union with Path Compression & Union by Rank\n";
    std::cout << "  Total Connected Components: " << numComponents << "\n\n";

    size_t topK = std::min<size_t>(5, sizes.size());
    std::cout << "  Top " << topK << " Components by Size:\n";
    std::cout << "  " << std::setw(6) << "Rank" << " | "
              << std::setw(15) << "Component Root" << " | "
              << std::setw(12) << "Size (Nodes)" << " | "
              << std::setw(11) << "% of Total" << "\n";
    std::cout << "  -------+-----------------+--------------+------------\n";

    for (size_t i = 0; i < topK; ++i) {
        double pct = (graph.numNodes > 0) ? (100.0 * sizes[i].second / graph.numNodes) : 0.0;
        std::cout << "  " << std::setw(6) << (i + 1) << " | "
                  << std::setw(15) << sizes[i].first << " | "
                  << std::setw(12) << sizes[i].second << " | "
                  << std::setw(10) << std::fixed << std::setprecision(2) << pct << "%\n";
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handlePageRank(SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  PAGERANK ANALYSIS (Random Surfer Centrality)\n";
    std::cout << SEPARATOR << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    graph.computePageRank(20, 0.85);
    std::vector<std::pair<int, double>> topPR = graph.getTopPageRank(10);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "  Convergence Parameters: Iterations = 20, Damping Factor = 0.85\n\n";
    std::cout << "  " << std::setw(6) << "Rank" << " | "
              << std::setw(9) << "Node ID" << " | "
              << std::setw(16) << "PageRank Score" << " | Name\n";
    std::cout << "  -------+-----------+-----------------+----------------------\n";

    for (size_t i = 0; i < topPR.size(); ++i) {
        std::cout << "  " << std::setw(6) << (i + 1) << " | "
                  << std::setw(9) << topPR[i].first << " | "
                  << std::setw(16) << std::fixed << std::setprecision(6) << topPR[i].second << " | "
                  << graph.getNodeName(topPR[i].first) << "\n";
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleTopNodesByDegree(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  TOP NODES BY DEGREE (Degree Centrality)\n";
    std::cout << SEPARATOR << "\n";

    std::cout << "  Enter K (number of top nodes to display, 1-" << graph.numNodes << "): ";
    int k;
    if (!(std::cin >> k)) {
        std::cin.clear();
        std::cout << "  Error: Invalid input! K must be a positive integer.\n";
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    if (k <= 0) {
        std::cout << "  Error: K must be strictly positive (greater than 0).\n";
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    if (k > graph.numNodes) {
        std::cout << "  Note: Requested K exceeds total nodes. Clamping to " << graph.numNodes << ".\n";
        k = graph.numNodes;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> topDegrees = graph.getTopNodesByDegree(k);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  Top " << topDegrees.size() << " Nodes by Degree Centrality:\n\n";
    std::cout << "  " << std::setw(6) << "Rank" << " | "
              << std::setw(9) << "Node ID" << " | "
              << std::setw(10) << "Degree" << " | Name\n";
    std::cout << "  -------+-----------+-----------+----------------------\n";

    for (size_t i = 0; i < topDegrees.size(); ++i) {
        std::cout << "  " << std::setw(6) << (i + 1) << " | "
                  << std::setw(9) << topDegrees[i].first << " | "
                  << std::setw(10) << topDegrees[i].second << " | "
                  << graph.getNodeName(topDegrees[i].first) << "\n";
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleFriendSuggestions(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  FRIEND SUGGESTIONS (2-Hop Neighbor Analysis)\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int nodeId = readNodeInput(graph, "  Enter node: ");
    if (nodeId == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    std::cout << "  Finding suggestions for: " << graph.getNodeName(nodeId) << " (ID: " << nodeId << ")\n";

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<int, int>> suggestions = graph.getTwoHopSuggestions(nodeId, 10);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n";
    if (suggestions.empty()) {
        std::cout << "  No 2-hop friend suggestions found for " << graph.getNodeName(nodeId) << ".\n";
    } else {
        std::cout << "  Friend Suggestions for " << graph.getNodeName(nodeId) << ":\n\n";
        std::cout << "  " << std::setw(4) << "#" << " | "
                  << std::setw(22) << "Suggested Friend" << " | "
                  << std::setw(9) << "Node ID" << " | "
                  << std::setw(14) << "Mutual Friends" << "\n";
        std::cout << "  -----+------------------------+-----------+---------------\n";

        for (size_t i = 0; i < suggestions.size(); ++i) {
            std::cout << "  " << std::setw(4) << (i + 1) << " | "
                      << std::setw(22) << graph.getNodeName(suggestions[i].first) << " | "
                      << std::setw(9) << suggestions[i].first << " | "
                      << std::setw(14) << suggestions[i].second << "\n";
        }
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleSearchNode(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  SEARCH NODE\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int nodeId = readNodeInput(graph, "  Enter node: ");
    if (nodeId == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    std::cout << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    graph.printNodeInfo(nodeId);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  Lookup Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleCheckConnection(SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  CHECK CONNECTION\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int u = readNodeInput(graph, "  Enter first node: ");
    if (u == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    int v = readNodeInput(graph, "  Enter second node: ");
    if (v == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool directEdge = graph.hasEdge(u, v);
    bool inSameComponent = graph.areConnected(u, v);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  Connection Analysis:\n";
    std::cout << "    " << graph.getNodeName(u) << " (ID: " << u << ")\n";
    std::cout << "    " << graph.getNodeName(v) << " (ID: " << v << ")\n";
    std::cout << "  " << SEPARATOR << "\n";
    std::cout << "  Direct Friendship / Edge:       " << (directEdge ? "[YES] - Direct friends" : "[NO] - Not direct friends") << "\n";
    std::cout << "  Same Connected Component (DSU): " << (inSameComponent ? "[YES] - Connected" : "[NO] - Disconnected") << "\n";
    std::cout << "  Reachable / Path Exists:        " << (inSameComponent ? "[YES] - Path exists" : "[NO] - Unreachable") << "\n";

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleFindNeighbors(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  FIND NEIGHBORS / FRIENDS\n";
    std::cout << SEPARATOR << "\n";
    std::cout << "  (Enter node ID or person name)\n";

    int nodeId = readNodeInput(graph, "  Enter node: ");
    if (nodeId == -1) {
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> neighbors = graph.getNeighbors(nodeId);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "\n  " << graph.getNodeName(nodeId) << " has " << neighbors.size() << " friend(s):\n";
    if (neighbors.empty()) {
        std::cout << "  (Isolated node with no friends)\n";
    } else {
        std::cout << "  " << SEPARATOR << "\n";
        for (size_t i = 0; i < neighbors.size(); ++i) {
            std::cout << "  " << std::setw(4) << (i + 1) << ". "
                      << graph.getNodeName(neighbors[i])
                      << " (ID: " << neighbors[i] << ")\n";
        }
        std::cout << "  " << SEPARATOR << "\n";
    }

    std::cout << "\n  Time: " << std::fixed << std::setprecision(2) << duration.count() << " ms\n";
    std::cout << SEPARATOR << "\n";
    pausePrompt();
}

void handleSearchByName(const SocialGraph& graph) {
    std::cout << SEPARATOR << "\n";
    std::cout << "  SEARCH BY NAME\n";
    std::cout << SEPARATOR << "\n";

    std::cout << "  Enter name to search: ";
    std::string query;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, query);

    size_t start = query.find_first_not_of(" \t");
    size_t end = query.find_last_not_of(" \t");
    if (start == std::string::npos) {
        std::cout << "  Error: Empty search query.\n";
        std::cout << SEPARATOR << "\n";
        pausePrompt();
        return;
    }
    query = query.substr(start, end - start + 1);

    auto results = graph.searchNodesByName(query, 20);

    std::cout << "\n";
    if (results.empty()) {
        std::cout << "  No matches found for '" << query << "'.\n";
    } else {
        std::cout << "  Found " << results.size() << " match(es) for '" << query << "':\n\n";
        std::cout << "  " << std::setw(4) << "#" << " | "
                  << std::setw(9) << "Node ID" << " | "
                  << std::setw(8) << "Degree" << " | Name\n";
        std::cout << "  -----+-----------+----------+----------------------\n";

        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "  " << std::setw(4) << (i + 1) << " | "
                      << std::setw(9) << results[i].first << " | "
                      << std::setw(8) << graph.getDegree(results[i].first) << " | "
                      << results[i].second << "\n";
        }
    }

    std::cout << "\n" << SEPARATOR << "\n";
    pausePrompt();
}

int main() {

    std::cout << DOUBLE_SEPARATOR << "\n";
    std::cout << "     SOCIAL NETWORK GRAPH ANALYZER\n";
    std::cout << "     DSA Project | B.Tech V Semester | JIIT Noida\n";
    std::cout << DOUBLE_SEPARATOR << "\n";
    std::cout << "Initializing graph data structures...\n\n";

    SocialGraph graph;

    const std::string datasetPath = "data/facebook_combined.txt";
    std::cout << "[1/3] Loading graph from '" << datasetPath << "'... \n";

    auto loadStart = std::chrono::high_resolution_clock::now();
    if (!graph.loadFromFile(datasetPath)) {
        std::cerr << "\n[FATAL ERROR] Failed to load graph dataset from '" << datasetPath << "'!\n";
        std::cerr << "Please ensure that '" << datasetPath << "' exists in the project directory.\n";
        std::cerr << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }
    auto loadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> loadTime = loadEnd - loadStart;

    std::cout << "      -> Success! Loaded " << graph.numNodes << " nodes and "
              << graph.numEdges << " edges in "
              << std::fixed << std::setprecision(2) << loadTime.count() << " ms\n\n";

    const std::string namesPath = "data/names.txt";
    if (graph.loadNames(namesPath)) {
        std::cout << "      -> Names loaded successfully!\n\n";
    } else {
        std::cout << "      -> Warning: Names file not found. Using numeric IDs only.\n\n";
    }

    std::cout << "[2/3] Pre-computing connected components (Union-Find)... \n";
    auto compStart = std::chrono::high_resolution_clock::now();
    int numComponents = graph.computeComponents();
    auto compEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> compTime = compEnd - compStart;

    std::cout << "      -> Success! Discovered " << numComponents << " connected component(s) in "
              << std::fixed << std::setprecision(2) << compTime.count() << " ms\n\n";

    std::cout << "[3/3] Pre-computing PageRank scores (20 iterations, damping=0.85)... \n";
    auto prStart = std::chrono::high_resolution_clock::now();
    graph.computePageRank(20, 0.85);
    auto prEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> prTime = prEnd - prStart;

    std::cout << "      -> Success! PageRank converged in "
              << std::fixed << std::setprecision(2) << prTime.count() << " ms\n\n";

    std::cout << "All structures initialized. Press Enter to enter the main menu...";
    std::cin.get();

    int choice = -1;
    do {
        clearScreen();
        printBanner(graph);
        printMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cout << "\n  Error: Invalid input! Please enter a number between 0 and 12.\n";
            std::cout << SEPARATOR << "\n";
            pausePrompt();
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                handleGraphStats(graph);
                break;
            case 2:
                handleBFS(graph);
                break;
            case 3:
                handleDFS(graph);
                break;
            case 4:
                handleShortestPath(graph);
                break;
            case 5:
                handleConnectedComponents(graph);
                break;
            case 6:
                handlePageRank(graph);
                break;
            case 7:
                handleTopNodesByDegree(graph);
                break;
            case 8:
                handleFriendSuggestions(graph);
                break;
            case 9:
                handleSearchNode(graph);
                break;
            case 10:
                handleCheckConnection(graph);
                break;
            case 11:
                handleFindNeighbors(graph);
                break;
            case 12:
                handleSearchByName(graph);
                break;
            case 0:
                std::cout << "\n" << DOUBLE_SEPARATOR << "\n";
                std::cout << "  Thank you for using Social Network Graph Analyzer!\n";
                std::cout << "  Exiting application. Goodbye!\n";
                std::cout << DOUBLE_SEPARATOR << "\n";
                break;
            default:
                std::cout << "\n  Error: Invalid choice (" << choice << ")! Please choose from 0 to 12.\n";
                std::cout << SEPARATOR << "\n";
                pausePrompt();
                break;
        }
    } while (choice != 0);

    return 0;
}

