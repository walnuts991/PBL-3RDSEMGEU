#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "Graph.h"
#include <vector>

struct ShortestPathResult {
    bool pathFound = false;
    double totalDistance = 0.0;
    std::vector<int> path;
};

class ShortestPath {
public:
    // sourceId and destinationId are actual substation IDs.
    static ShortestPathResult dijkstra(
        const Graph& graph,
        int sourceId,
        int destinationId
    );
};

#endif