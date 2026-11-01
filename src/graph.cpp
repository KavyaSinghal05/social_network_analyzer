

#include "graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <functional>
#include <numeric>

SocialGraph::SocialGraph()
    : components_computed(false),
      num_components(0),
      pagerank_computed(false),
      names_loaded(false),
      numNodes(0),
      numEdges(0) {
}

int SocialGraph::getInternalId(int rawId) const {
    auto it = rawToInternal.find(rawId);
    if (it != rawToInternal.end()) {
        return it->second;
    }
    return -1;
}

int SocialGraph::getRawId(int internalId) const {
    if (internalId >= 0 && internalId < static_cast<int>(internalToRaw.size())) {
        return internalToRaw[internalId];
    }
    return -1;
}

bool SocialGraph::loadFromFile(const std::string& filepath) {
    auto start = std::chrono::high_resolution_clock::now();

    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Failed to open graph file: " << filepath << std::endl;
        return false;
    }

    row_ptr.clear();
    col_idx.clear();
    rawToInternal.clear();
    internalToRaw.clear();
    uf_parent.clear();
    uf_rank.clear();
    components_computed = false;
    num_components = 0;
    pagerank_scores.clear();
    pagerank_computed = false;
    nodeNames.clear();
    nameToRawId.clear();
    names_loaded = false;
    numNodes = 0;
    numEdges = 0;

    auto getOrAddNode = [this](int rawId) -> int {
        auto it = rawToInternal.find(rawId);
        if (it == rawToInternal.end()) {
            int newId = static_cast<int>(internalToRaw.size());
            rawToInternal[rawId] = newId;
            internalToRaw.push_back(rawId);
            return newId;
        }
        return it->second;
    };

    std::vector<std::pair<int, int>> edgeList;
    std::string line;

    while (std::getline(inFile, line)) {
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue;
        }
        if (line[first] == '#') {
            continue;
        }

        std::istringstream iss(line.substr(first));
        int rawU, rawV;
        if (iss >> rawU >> rawV) {
            int u = getOrAddNode(rawU);
            int v = getOrAddNode(rawV);
            edgeList.emplace_back(u, v);
        }
    }
    inFile.close();

    numNodes = static_cast<int>(internalToRaw.size());
    numEdges = static_cast<int>(edgeList.size());

    if (numNodes == 0) {
        row_ptr.assign(1, 0);
        return true;
    }

    std::vector<int> degrees(numNodes, 0);
    for (const auto& edge : edgeList) {
        degrees[edge.first]++;
        degrees[edge.second]++;
    }

    row_ptr.assign(numNodes + 1, 0);
    for (int i = 0; i < numNodes; ++i) {
        row_ptr[i + 1] = row_ptr[i] + degrees[i];
    }

    col_idx.resize(row_ptr[numNodes]);
    std::vector<int> offsets = row_ptr;
    for (const auto& edge : edgeList) {
        int u = edge.first;
        int v = edge.second;
        col_idx[offsets[u]++] = v;
        col_idx[offsets[v]++] = u;
    }

    for (int i = 0; i < numNodes; ++i) {
        std::sort(col_idx.begin() + row_ptr[i], col_idx.begin() + row_ptr[i + 1]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "========================================" << std::endl;
    std::cout << "        Graph Loaded Successfully       " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " File Path       : " << filepath << std::endl;
    std::cout << " Unique Nodes    : " << numNodes << std::endl;
    std::cout << " Undirected Edges: " << numEdges << std::endl;
    std::cout << " CSR Directed Arc: " << col_idx.size() << std::endl;
    std::cout << " Loading Time    : " << std::fixed << std::setprecision(2) << elapsed.count() << " ms" << std::endl;
    std::cout << "========================================" << std::endl;

    return true;
}

std::vector<int> SocialGraph::bfs(int startRaw, int maxNodes) const {
    int startInternal = getInternalId(startRaw);
    if (startInternal == -1 || maxNodes <= 0 || numNodes == 0) {
        return {};
    }

    std::vector<int> result;
    result.reserve(std::min(maxNodes, numNodes));

    std::vector<bool> visited(numNodes, false);
    std::queue<int> q;

    visited[startInternal] = true;
    q.push(startInternal);

    while (!q.empty() && static_cast<int>(result.size()) < maxNodes) {
        int curr = q.front();
        q.pop();

        result.push_back(internalToRaw[curr]);

        int startIdx = row_ptr[curr];
        int endIdx = row_ptr[curr + 1];
        for (int idx = startIdx; idx < endIdx; ++idx) {
            int neighbor = col_idx[idx];
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return result;
}

std::vector<int> SocialGraph::dfs(int startRaw, int maxNodes) const {
    int startInternal = getInternalId(startRaw);
    if (startInternal == -1 || maxNodes <= 0 || numNodes == 0) {
        return {};
    }

    std::vector<int> result;
    result.reserve(std::min(maxNodes, numNodes));

    std::vector<bool> visited(numNodes, false);
    std::stack<int> s;

    s.push(startInternal);

    while (!s.empty() && static_cast<int>(result.size()) < maxNodes) {
        int curr = s.top();
        s.pop();

        if (visited[curr]) {
            continue;
        }

        visited[curr] = true;
        result.push_back(internalToRaw[curr]);

        int startIdx = row_ptr[curr];
        int endIdx = row_ptr[curr + 1];
        for (int idx = endIdx - 1; idx >= startIdx; --idx) {
            int neighbor = col_idx[idx];
            if (!visited[neighbor]) {
                s.push(neighbor);
            }
        }
    }

    return result;
}

std::vector<int> SocialGraph::shortestPath(int srcRaw, int dstRaw) const {
    int src = getInternalId(srcRaw);
    int dst = getInternalId(dstRaw);

    if (src == -1 || dst == -1 || numNodes == 0) {
        return {};
    }

    if (src == dst) {
        return {srcRaw};
    }

    std::queue<int> qSrc;
    std::queue<int> qDst;

    std::unordered_map<int, int> parentSrc;
    std::unordered_map<int, int> parentDst;

    parentSrc[src] = -1;
    parentDst[dst] = -1;

    qSrc.push(src);
    qDst.push(dst);

    int meetingNode = -1;

    while (!qSrc.empty() && !qDst.empty()) {

        if (qSrc.size() <= qDst.size()) {
            int levelSize = static_cast<int>(qSrc.size());
            for (int i = 0; i < levelSize; ++i) {
                int curr = qSrc.front();
                qSrc.pop();

                int startIdx = row_ptr[curr];
                int endIdx = row_ptr[curr + 1];
                for (int idx = startIdx; idx < endIdx; ++idx) {
                    int neighbor = col_idx[idx];

                    if (parentDst.find(neighbor) != parentDst.end()) {
                        parentSrc[neighbor] = curr;
                        meetingNode = neighbor;
                        break;
                    }

                    if (parentSrc.find(neighbor) == parentSrc.end()) {
                        parentSrc[neighbor] = curr;
                        qSrc.push(neighbor);
                    }
                }
                if (meetingNode != -1) {
                    break;
                }
            }
        } else {
            int levelSize = static_cast<int>(qDst.size());
            for (int i = 0; i < levelSize; ++i) {
                int curr = qDst.front();
                qDst.pop();

                int startIdx = row_ptr[curr];
                int endIdx = row_ptr[curr + 1];
                for (int idx = startIdx; idx < endIdx; ++idx) {
                    int neighbor = col_idx[idx];

                    if (parentSrc.find(neighbor) != parentSrc.end()) {
                        parentDst[neighbor] = curr;
                        meetingNode = neighbor;
                        break;
                    }

                    if (parentDst.find(neighbor) == parentDst.end()) {
                        parentDst[neighbor] = curr;
                        qDst.push(neighbor);
                    }
                }
                if (meetingNode != -1) {
                    break;
                }
            }
        }

        if (meetingNode != -1) {
            break;
        }
    }

    if (meetingNode == -1) {
        return {};
    }

    std::vector<int> path;
    for (int curr = meetingNode; curr != -1; curr = parentSrc[curr]) {
        path.push_back(internalToRaw[curr]);
    }
    std::reverse(path.begin(), path.end());

    for (int curr = parentDst[meetingNode]; curr != -1; curr = parentDst[curr]) {
        path.push_back(internalToRaw[curr]);
    }

    return path;
}

int SocialGraph::uf_find(int x) {
    if (uf_parent[x] != x) {
        uf_parent[x] = uf_find(uf_parent[x]);
    }
    return uf_parent[x];
}

void SocialGraph::uf_unite(int x, int y) {
    int rootX = uf_find(x);
    int rootY = uf_find(y);

    if (rootX != rootY) {
        if (uf_rank[rootX] < uf_rank[rootY]) {
            uf_parent[rootX] = rootY;
        } else if (uf_rank[rootX] > uf_rank[rootY]) {
            uf_parent[rootY] = rootX;
        } else {
            uf_parent[rootY] = rootX;
            uf_rank[rootX]++;
        }
    }
}

int SocialGraph::computeComponents() {
    if (numNodes == 0) {
        components_computed = true;
        num_components = 0;
        return 0;
    }

    uf_parent.resize(numNodes);
    uf_rank.assign(numNodes, 0);
    for (int i = 0; i < numNodes; ++i) {
        uf_parent[i] = i;
    }

    for (int u = 0; u < numNodes; ++u) {
        int startIdx = row_ptr[u];
        int endIdx = row_ptr[u + 1];
        for (int idx = startIdx; idx < endIdx; ++idx) {
            int v = col_idx[idx];
            if (u < v) {
                uf_unite(u, v);
            }
        }
    }

    int count = 0;
    for (int i = 0; i < numNodes; ++i) {
        if (uf_find(i) == i) {
            count++;
        }
    }

    num_components = count;
    components_computed = true;
    return num_components;
}

std::vector<std::pair<int, int>> SocialGraph::getComponentSizes() const {
    if (numNodes == 0) {
        return {};
    }

    if (!components_computed) {
        const_cast<SocialGraph*>(this)->computeComponents();
    }

    std::unordered_map<int, int> rootCount;
    for (int i = 0; i < numNodes; ++i) {
        int root = uf_parent[i];
        while (root != uf_parent[root]) {
            root = uf_parent[root];
        }
        rootCount[internalToRaw[root]]++;
    }

    std::vector<std::pair<int, int>> sizes;
    sizes.reserve(rootCount.size());
    for (const auto& entry : rootCount) {
        sizes.push_back(entry);
    }

    std::sort(sizes.begin(), sizes.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  if (a.second != b.second) {
                      return a.second > b.second;
                  }
                  return a.first < b.first;
              });

    return sizes;
}

bool SocialGraph::areConnected(int nodeA_raw, int nodeB_raw) {
    int u = getInternalId(nodeA_raw);
    int v = getInternalId(nodeB_raw);

    if (u == -1 || v == -1) {
        return false;
    }
    if (u == v) {
        return true;
    }

    if (!components_computed) {
        computeComponents();
    }

    return uf_find(u) == uf_find(v);
}

void SocialGraph::computePageRank(int iterations, double damping) {
    if (numNodes == 0) {
        pagerank_scores.clear();
        pagerank_computed = true;
        return;
    }

    const double baseScore = (1.0 - damping) / static_cast<double>(numNodes);
    const double initialScore = 1.0 / static_cast<double>(numNodes);

    std::vector<double> currentScores(numNodes, initialScore);
    std::vector<double> nextScores(numNodes, 0.0);

    std::vector<double> invDegrees(numNodes, 0.0);
    for (int i = 0; i < numNodes; ++i) {
        int deg = row_ptr[i + 1] - row_ptr[i];
        if (deg > 0) {
            invDegrees[i] = 1.0 / static_cast<double>(deg);
        }
    }

    for (int it = 0; it < iterations; ++it) {
        for (int v = 0; v < numNodes; ++v) {
            double incomingSum = 0.0;
            int startIdx = row_ptr[v];
            int endIdx = row_ptr[v + 1];
            for (int idx = startIdx; idx < endIdx; ++idx) {
                int u = col_idx[idx];
                incomingSum += currentScores[u] * invDegrees[u];
            }
            nextScores[v] = baseScore + damping * incomingSum;
        }
        currentScores.swap(nextScores);
    }

    pagerank_scores = std::move(currentScores);
    pagerank_computed = true;
}

std::vector<std::pair<int, double>> SocialGraph::getTopPageRank(int k) const {
    if (k <= 0 || numNodes == 0) {
        return {};
    }

    if (!pagerank_computed) {
        const_cast<SocialGraph*>(this)->computePageRank();
    }

    auto comp = [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second;
    };

    std::priority_queue<std::pair<int, double>,
                        std::vector<std::pair<int, double>>,
                        decltype(comp)> pq(comp);

    for (int i = 0; i < numNodes; ++i) {
        int rawId = internalToRaw[i];
        double score = pagerank_scores[i];

        if (static_cast<int>(pq.size()) < k) {
            pq.push({rawId, score});
        } else if (score > pq.top().second) {
            pq.pop();
            pq.push({rawId, score});
        }
    }

    std::vector<std::pair<int, double>> topResults;
    topResults.reserve(pq.size());
    while (!pq.empty()) {
        topResults.push_back(pq.top());
        pq.pop();
    }

    std::reverse(topResults.begin(), topResults.end());
    return topResults;
}

std::vector<std::pair<int, int>> SocialGraph::getTwoHopSuggestions(int nodeRaw, int maxSuggestions) const {
    int u = getInternalId(nodeRaw);
    if (u == -1 || maxSuggestions <= 0 || numNodes == 0) {
        return {};
    }

    std::unordered_set<int> directNeighbors;
    int uStart = row_ptr[u];
    int uEnd = row_ptr[u + 1];
    for (int idx = uStart; idx < uEnd; ++idx) {
        directNeighbors.insert(col_idx[idx]);
    }

    std::unordered_map<int, int> mutualCounts;

    for (int idx = uStart; idx < uEnd; ++idx) {
        int neighbor = col_idx[idx];
        int nStart = row_ptr[neighbor];
        int nEnd = row_ptr[neighbor + 1];
        for (int nIdx = nStart; nIdx < nEnd; ++nIdx) {
            int candidate = col_idx[nIdx];

            if (candidate != u && directNeighbors.find(candidate) == directNeighbors.end()) {
                mutualCounts[candidate]++;
            }
        }
    }

    std::vector<std::pair<int, int>> suggestions;
    suggestions.reserve(mutualCounts.size());
    for (const auto& entry : mutualCounts) {
        suggestions.emplace_back(internalToRaw[entry.first], entry.second);
    }

    std::sort(suggestions.begin(), suggestions.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  if (a.second != b.second) {
                      return a.second > b.second;
                  }
                  return a.first < b.first;
              });

    if (static_cast<int>(suggestions.size()) > maxSuggestions) {
        suggestions.resize(maxSuggestions);
    }

    return suggestions;
}

int SocialGraph::getDegree(int nodeRaw) const {
    int u = getInternalId(nodeRaw);
    if (u == -1) {
        return 0;
    }
    return row_ptr[u + 1] - row_ptr[u];
}

std::vector<std::pair<int, int>> SocialGraph::getTopNodesByDegree(int k) const {
    if (k <= 0 || numNodes == 0) {
        return {};
    }

    auto comp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first > b.first;
    };

    std::priority_queue<std::pair<int, int>,
                        std::vector<std::pair<int, int>>,
                        decltype(comp)> pq(comp);

    for (int i = 0; i < numNodes; ++i) {
        int rawId = internalToRaw[i];
        int deg = row_ptr[i + 1] - row_ptr[i];

        if (static_cast<int>(pq.size()) < k) {
            pq.push({rawId, deg});
        } else if (deg > pq.top().second) {
            pq.pop();
            pq.push({rawId, deg});
        }
    }

    std::vector<std::pair<int, int>> results;
    results.reserve(pq.size());
    while (!pq.empty()) {
        results.push_back(pq.top());
        pq.pop();
    }

    std::reverse(results.begin(), results.end());
    return results;
}

bool SocialGraph::nodeExists(int rawId) const {
    return rawToInternal.find(rawId) != rawToInternal.end();
}

void SocialGraph::printNodeInfo(int rawId) const {
    int u = getInternalId(rawId);
    if (u == -1) {
        std::cout << "[Info] Node " << rawId << " does not exist in the graph." << std::endl;
        return;
    }

    int deg = row_ptr[u + 1] - row_ptr[u];

    std::cout << "----------------------------------------" << std::endl;
    std::cout << " Node Information: " << rawId << std::endl;
    std::cout << " Name             : " << getNodeName(rawId) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << " Raw Node ID     : " << rawId << std::endl;
    std::cout << " Internal CSR ID : " << u << std::endl;
    std::cout << " Degree          : " << deg << std::endl;

    if (pagerank_computed && u < static_cast<int>(pagerank_scores.size())) {
        std::cout << " PageRank Score  : " << std::fixed << std::setprecision(6) << pagerank_scores[u] << std::endl;
    }

    int displayLimit = std::min(deg, 20);
    std::cout << " Neighbors (" << displayLimit << " of " << deg << " shown): ";
    int startIdx = row_ptr[u];
    for (int i = 0; i < displayLimit; ++i) {
        std::cout << internalToRaw[col_idx[startIdx + i]];
        if (i + 1 < displayLimit) {
            std::cout << ", ";
        }
    }
    if (deg > displayLimit) {
        std::cout << ", ... (" << (deg - displayLimit) << " more)";
    }
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

std::vector<int> SocialGraph::getNeighbors(int nodeRaw) const {
    int u = getInternalId(nodeRaw);
    if (u == -1) {
        return {};
    }

    int startIdx = row_ptr[u];
    int endIdx = row_ptr[u + 1];

    std::vector<int> neighbors;
    neighbors.reserve(endIdx - startIdx);
    for (int idx = startIdx; idx < endIdx; ++idx) {
        neighbors.push_back(internalToRaw[col_idx[idx]]);
    }
    return neighbors;
}

bool SocialGraph::hasEdge(int uRaw, int vRaw) const {
    int u = getInternalId(uRaw);
    int v = getInternalId(vRaw);

    if (u == -1 || v == -1) {
        return false;
    }

    auto startIt = col_idx.begin() + row_ptr[u];
    auto endIt = col_idx.begin() + row_ptr[u + 1];

    return std::binary_search(startIt, endIt, v);
}

bool SocialGraph::loadNames(const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        std::cerr << "[Warning] Could not open names file: " << filepath << std::endl;
        return false;
    }

    nodeNames.clear();
    nameToRawId.clear();

    std::string line;
    while (std::getline(inFile, line)) {
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos || line[first] == '#') {
            continue;
        }

        std::istringstream iss(line.substr(first));
        int rawId;
        std::string firstName, lastName;
        if (iss >> rawId >> firstName >> lastName) {
            std::string fullName = firstName + " " + lastName;
            nodeNames[rawId] = fullName;

            std::string lowerName = fullName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            nameToRawId[lowerName] = rawId;
        }
    }
    inFile.close();

    names_loaded = !nodeNames.empty();
    return names_loaded;
}

std::string SocialGraph::getNodeName(int rawId) const {
    if (names_loaded) {
        auto it = nodeNames.find(rawId);
        if (it != nodeNames.end()) {
            return it->second;
        }
    }
    return "Node " + std::to_string(rawId);
}

int SocialGraph::findNodeByName(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    auto it = nameToRawId.find(lowerName);
    if (it != nameToRawId.end()) {
        return it->second;
    }
    return -1;
}

std::vector<std::pair<int, std::string>> SocialGraph::searchNodesByName(const std::string& query, int maxResults) const {
    std::vector<std::pair<int, std::string>> results;
    if (!names_loaded || query.empty()) {
        return results;
    }

    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& entry : nodeNames) {
        std::string lowerName = entry.second;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.emplace_back(entry.first, entry.second);
            if (static_cast<int>(results.size()) >= maxResults) {
                break;
            }
        }
    }

    std::sort(results.begin(), results.end());
    return results;
}

int SocialGraph::resolveNodeInput(const std::string& input) const {

    try {
        size_t pos;
        int id = std::stoi(input, &pos);

        if (pos == input.size() && nodeExists(id)) {
            return id;
        }
    } catch (...) {

    }

    int id = findNodeByName(input);
    if (id != -1) {
        return id;
    }

    auto matches = searchNodesByName(input, 2);
    if (matches.size() == 1) {
        return matches[0].first;
    }

    return -1;
}

double SocialGraph::getAverageDegree() const {
    if (numNodes == 0) {
        return 0.0;
    }
    return (2.0 * static_cast<double>(numEdges)) / static_cast<double>(numNodes);
}

int SocialGraph::getMaxDegree() const {
    if (numNodes == 0) {
        return 0;
    }
    int maxDeg = 0;
    for (int i = 0; i < numNodes; ++i) {
        int deg = row_ptr[i + 1] - row_ptr[i];
        if (deg > maxDeg) {
            maxDeg = deg;
        }
    }
    return maxDeg;
}

int SocialGraph::getMinDegree() const {
    if (numNodes == 0) {
        return 0;
    }
    int minDeg = row_ptr[1] - row_ptr[0];
    for (int i = 1; i < numNodes; ++i) {
        int deg = row_ptr[i + 1] - row_ptr[i];
        if (deg < minDeg) {
            minDeg = deg;
        }
    }
    return minDeg;
}

void SocialGraph::printStats() const {
    std::cout << "========================================" << std::endl;
    std::cout << "         Graph Statistics Summary       " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " Total Unique Nodes: " << numNodes << std::endl;
    std::cout << " Total Undir Edges : " << numEdges << std::endl;
    std::cout << " Average Degree    : " << std::fixed << std::setprecision(2) << getAverageDegree() << std::endl;
    std::cout << " Maximum Degree    : " << getMaxDegree() << std::endl;
    std::cout << " Minimum Degree    : " << getMinDegree() << std::endl;

    if (numNodes > 1) {
        double maxPossibleEdges = (static_cast<double>(numNodes) * (numNodes - 1)) / 2.0;
        double density = static_cast<double>(numEdges) / maxPossibleEdges;
        std::cout << " Graph Density     : " << std::fixed << std::setprecision(6) << density << std::endl;
    }

    double memoryKB = (row_ptr.size() * sizeof(int) + col_idx.size() * sizeof(int)) / 1024.0;
    std::cout << " CSR Array Memory  : " << std::fixed << std::setprecision(2) << memoryKB << " KB" << std::endl;
    std::cout << " Components Status : " << (components_computed ? (std::to_string(num_components) + " components") : "Not computed") << std::endl;
    std::cout << " PageRank Status   : " << (pagerank_computed ? "Computed" : "Not computed") << std::endl;
    std::cout << " Names Loaded    : " << (names_loaded ? "Yes" : "No") << std::endl;
    std::cout << "========================================" << std::endl;
}

