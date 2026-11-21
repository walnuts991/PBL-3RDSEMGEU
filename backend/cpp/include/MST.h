
#ifndef MST_H
#define MST_H

#include "Graph.h"
#include <vector>

// Stores information about one selected transmission line.
struct MSTEdge {
    int lineId;
    int fromId;
    int toId;
    double cost;
};

// Stores the complete result of an MST algorithm.
struct MSTResult {
    std::vector<MSTEdge> selectedEdges;
    double totalCost = 0.0;
    int componentCount = 0;
    bool isSpanningTree = false;
};

class MST {
public:

    // Kruskal's Minimum Spanning Tree algorithm.
    static MSTResult kruskal(const Graph& graph);

    // Prim's Minimum Spanning Tree algorithm.
    static MSTResult prim(const Graph& graph);
};

#endif
