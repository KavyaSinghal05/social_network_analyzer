

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>

class SocialGraph {
private:

    std::vector<int> row_ptr;
    std::vector<int> col_idx;

    std::unordered_map<int, int> rawToInternal;
    std::vector<int> internalToRaw;

    std::vector<int> uf_parent;
    std::vector<int> uf_rank;
    bool components_computed;
    int num_components;

    int uf_find(int x);

    void uf_unite(int x, int y);

    std::vector<double> pagerank_scores;
    bool pagerank_computed;

    std::unordered_map<int, std::string> nodeNames;
    std::unordered_map<std::string, int> nameToRawId;
    bool names_loaded;

    int getInternalId(int rawId) const;
    int getRawId(int internalId) const;

public:
    int numNodes;
    int numEdges;

    SocialGraph();

    bool loadFromFile(const std::string& filepath);

    bool loadNames(const std::string& filepath);

    std::string getNodeName(int rawId) const;

    int findNodeByName(const std::string& name) const;

    std::vector<std::pair<int, std::string>> searchNodesByName(const std::string& query, int maxResults = 10) const;

    int resolveNodeInput(const std::string& input) const;

    void printStats() const;
    double getAverageDegree() const;
    int getMaxDegree() const;
    int getMinDegree() const;

    std::vector<int> bfs(int startRaw, int maxNodes = 50) const;

    std::vector<int> dfs(int startRaw, int maxNodes = 50) const;

    std::vector<int> shortestPath(int srcRaw, int dstRaw) const;

    int computeComponents();

    std::vector<std::pair<int, int>> getComponentSizes() const;

    bool areConnected(int nodeA_raw, int nodeB_raw);

    void computePageRank(int iterations = 20, double damping = 0.85);

    std::vector<std::pair<int, double>> getTopPageRank(int k = 10) const;

    std::vector<std::pair<int, int>> getTwoHopSuggestions(int nodeRaw, int maxSuggestions = 10) const;

    int getDegree(int nodeRaw) const;

    std::vector<std::pair<int, int>> getTopNodesByDegree(int k = 10) const;

    bool nodeExists(int rawId) const;

    void printNodeInfo(int rawId) const;

    std::vector<int> getNeighbors(int nodeRaw) const;

    bool hasEdge(int uRaw, int vRaw) const;
};

#endif

