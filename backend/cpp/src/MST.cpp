
#include "MST.h"
#include "DSU.h"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <utility>

namespace {

// Stores an edge with both its original substation IDs
// and their internal zero-based indices.
struct IndexedEdge {
    MSTEdge edge;
    int u;
    int v;
};

// Converts Graph edges into indexed edges for algorithms.
std::vector<IndexedEdge> indexedEdges(const Graph& graph) {

    std::vector<IndexedEdge> result;

    for (const auto& line : graph.getLines()) {

        int u = -1;
        int v = -1;

        if (!graph.getSubstationIndex(line.getFrom(), u) ||
            !graph.getSubstationIndex(line.getTo(), v)) {

            throw std::invalid_argument(
                "Line endpoint is missing from Graph"
            );
        }

        result.push_back({
            {
                line.getId(),
                line.getFrom(),
                line.getTo(),
                line.getConstructionCost()
            },
            u,
            v
        });
    }

    return result;
}

// Creates the final result returned by either algorithm.
MSTResult makeResult(
    std::vector<MSTEdge> edges,
    double cost,
    int components,
    int vertices
) {
    MSTResult result;

    result.selectedEdges = std::move(edges);
    result.totalCost = cost;
    result.componentCount = components;
    result.isSpanningTree = (vertices == 0 || components == 1);

    return result;
}

} // namespace


// ==========================================
// KRUSKAL'S ALGORITHM
// ==========================================

MSTResult MST::kruskal(const Graph& graph) {

    int n = graph.vertexCount();

    // Get all transmission lines.
    auto edges = indexedEdges(graph);

    // Sort edges in increasing order of construction cost.
    std::sort(
        edges.begin(),
        edges.end(),
        [](const IndexedEdge& a, const IndexedEdge& b) {

            if (a.edge.cost != b.edge.cost) {
                return a.edge.cost < b.edge.cost;
            }

            return a.edge.lineId < b.edge.lineId;
        }
    );

    DSU dsu(n);

    std::vector<MSTEdge> chosen;
    double totalCost = 0.0;

    // Select edges without forming cycles.
    for (const auto& edge : edges) {

        if (dsu.unite(edge.u, edge.v)) {

            chosen.push_back(edge.edge);
            totalCost += edge.edge.cost;
        }
    }

    return makeResult(
        std::move(chosen),
        totalCost,
        dsu.getComponentCount(),
        n
    );
}


// ==========================================
// PRIM'S ALGORITHM
// ==========================================

MSTResult MST::prim(const Graph& graph) {

    int n = graph.vertexCount();

    auto edges = indexedEdges(graph);

    // Adjacency list representation.
    struct Adj {
        int to;
        std::size_t edgeIndex;
    };

    std::vector<std::vector<Adj>> adjacency(n);

    for (std::size_t i = 0; i < edges.size(); i++) {

        adjacency[edges[i].u].push_back(
            {edges[i].v, i}
        );

        adjacency[edges[i].v].push_back(
            {edges[i].u, i}
        );
    }

    // Priority queue item.
    struct Item {
        double cost;
        int lineId;
        int to;
        std::size_t edgeIndex;
    };

    // Makes the priority queue behave as a min-heap.
    struct Greater {
        bool operator()(const Item& a, const Item& b) const {

            if (a.cost != b.cost) {
                return a.cost > b.cost;
            }

            return a.lineId > b.lineId;
        }
    };

    std::priority_queue<
        Item,
        std::vector<Item>,
        Greater
    > pq;

    std::vector<bool> visited(n, false);

    std::vector<MSTEdge> chosen;

    double totalCost = 0.0;
    int components = 0;

    // Start from every unvisited vertex.
    // This also handles disconnected graphs.
    for (int start = 0; start < n; start++) {

        if (visited[start]) {
            continue;
        }

        components++;
        visited[start] = true;

        // Add neighbouring edges to the priority queue.
        for (const auto& a : adjacency[start]) {

            const auto& edge = edges[a.edgeIndex];

            pq.push({
                edge.edge.cost,
                edge.edge.lineId,
                a.to,
                a.edgeIndex
            });
        }

        while (!pq.empty()) {

            Item current = pq.top();
            pq.pop();

            // Ignore already visited vertices.
            if (visited[current.to]) {
                continue;
            }

            visited[current.to] = true;

            const auto& selected =
                edges[current.edgeIndex].edge;

            chosen.push_back(selected);
            totalCost += selected.cost;

            // Add edges leading to unvisited neighbours.
            for (const auto& a : adjacency[current.to]) {

                if (!visited[a.to]) {

                    const auto& edge = edges[a.edgeIndex];

                    pq.push({
                        edge.edge.cost,
                        edge.edge.lineId,
                        a.to,
                        a.edgeIndex
                    });
                }
            }
        }
    }

    return makeResult(
        std::move(chosen),
        totalCost,
        components,
        n
    );
}
