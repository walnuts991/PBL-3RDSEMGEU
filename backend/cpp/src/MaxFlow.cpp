#include "MaxFlow.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

namespace {

struct ResidualEdge {
    int to;
    int reverseIndex;
    double capacity;
};

// Adds a directed edge and its residual reverse edge.
void addDirectedEdge(
    std::vector<std::vector<ResidualEdge>>& network,
    int from,
    int to,
    double capacity
) {
    int forwardIndex = static_cast<int>(network[from].size());
    int reverseIndex = static_cast<int>(network[to].size());

    network[from].push_back({
        to, reverseIndex, capacity
    });

    network[to].push_back({
        from, forwardIndex, 0.0
    });
}

// An undirected transmission line is represented by
// two independently capacitated directed connections.
void addUndirectedLine(
    std::vector<std::vector<ResidualEdge>>& network,
    int u,
    int v,
    double capacity
) {
    addDirectedEdge(network, u, v, capacity);
    addDirectedEdge(network, v, u, capacity);
}

} // namespace


MaxFlowResult MaxFlow::edmondsKarp(const Graph& graph) {

    const int n = graph.vertexCount();

    // Virtual source = n
    // Virtual sink   = n + 1
    const int source = n;
    const int sink = n + 1;

    std::vector<std::vector<ResidualEdge>> network(n + 2);

    // Add transmission-line capacities.
    for (const auto& line : graph.getLines()) {

        int u = -1;
        int v = -1;

        if (!graph.getSubstationIndex(line.getFrom(), u) ||
            !graph.getSubstationIndex(line.getTo(), v)) {
            throw std::invalid_argument(
                "Transmission line has an invalid endpoint"
            );
        }

        double capacity = line.getCapacity();

        if (!std::isfinite(capacity) || capacity < 0.0) {
            throw std::invalid_argument(
                "Transmission capacity must be non-negative"
            );
        }

        addUndirectedLine(network, u, v, capacity);
    }

    // Connect the virtual source to generators.
    // Connect loads to the virtual sink.
    for (int i = 0; i < n; i++) {

        const auto& station = graph.getSubstations()[i];

        double supply = station.getGenerationMW();
        double demand = station.getLoadDemand();

        if (!std::isfinite(supply) ||
            !std::isfinite(demand) ||
            supply < 0.0 || demand < 0.0) {
            throw std::invalid_argument(
                "Supply and demand must be non-negative"
            );
        }

        if (supply > 0.0) {
            addDirectedEdge(network, source, i, supply);
        }

        if (demand > 0.0) {
            addDirectedEdge(network, i, sink, demand);
        }
    }

    MaxFlowResult result;

    const double EPSILON = 1e-9;

    // ==========================================
    // EDMONDS-KARP: BFS AUGMENTING PATH SEARCH
    // ==========================================

    while (true) {

        std::vector<int> parentVertex(n + 2, -1);
        std::vector<int> parentEdge(n + 2, -1);

        std::queue<int> bfsQueue;

        bfsQueue.push(source);
        parentVertex[source] = source;

        // Find an augmenting path using BFS.
        while (!bfsQueue.empty() &&
               parentVertex[sink] == -1) {

            int current = bfsQueue.front();
            bfsQueue.pop();

            for (int i = 0;
                 i < static_cast<int>(network[current].size());
                 i++) {

                const auto& edge = network[current][i];

                if (edge.capacity > EPSILON &&
                    parentVertex[edge.to] == -1) {

                    parentVertex[edge.to] = current;
                    parentEdge[edge.to] = i;

                    bfsQueue.push(edge.to);

                    if (edge.to == sink) {
                        break;
                    }
                }
            }
        }

        // No more augmenting path exists.
        if (parentVertex[sink] == -1) {
            break;
        }

        // Find the bottleneck capacity of this path.
        double pathFlow =
            std::numeric_limits<double>::infinity();

        for (int v = sink; v != source;
             v = parentVertex[v]) {

            int u = parentVertex[v];
            int edgeIndex = parentEdge[v];

            pathFlow = std::min(
                pathFlow,
                network[u][edgeIndex].capacity
            );
        }

        // Update forward and reverse residual capacities.
        for (int v = sink; v != source;
             v = parentVertex[v]) {

            int u = parentVertex[v];
            int edgeIndex = parentEdge[v];

            int reverseIndex =
                network[u][edgeIndex].reverseIndex;

            network[u][edgeIndex].capacity -= pathFlow;

            network[v][reverseIndex].capacity += pathFlow;
        }

        result.maximumFlowMW += pathFlow;
    }

    // ==========================================
    // FIND SOURCE SIDE OF THE MINIMUM CUT
    // ==========================================

    std::vector<bool> reachable(n + 2, false);

    std::queue<int> q;
    q.push(source);
    reachable[source] = true;

    while (!q.empty()) {

        int current = q.front();
        q.pop();

        for (const auto& edge : network[current]) {

            if (edge.capacity > EPSILON &&
                !reachable[edge.to]) {

                reachable[edge.to] = true;
                q.push(edge.to);
            }
        }
    }

    for (int i = 0; i < n; i++) {

        if (reachable[i]) {
            result.sourceSideSubstationIds.push_back(
                graph.getSubstations()[i].getId()
            );
        }
    }

    // Identify original transmission lines crossing the cut.
    for (const auto& line : graph.getLines()) {

        int u = -1;
        int v = -1;

        graph.getSubstationIndex(line.getFrom(), u);
        graph.getSubstationIndex(line.getTo(), v);

        if (reachable[u] != reachable[v]) {
            result.bottleneckLineIds.push_back(line.getId());
        }
    }

    return result;
}