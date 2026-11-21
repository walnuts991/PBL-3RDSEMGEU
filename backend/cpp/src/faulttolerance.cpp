#include "faulttolerance.h"

#include <algorithm>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>

FaultTolerance::FaultTolerance(
    int vertexCount,
    const std::vector<FT_Edge>& edges
) : vertexCount(vertexCount), edges(edges) {
    if (vertexCount < 0) {
        throw std::invalid_argument("Vertex count must be non-negative");
    }

    for (const auto& edge : edges) {
        if (edge.u < 0 || edge.u >= vertexCount ||
            edge.v < 0 || edge.v >= vertexCount) {
            throw std::invalid_argument("Edge endpoint is out of range");
        }
    }
}

std::vector<FT_Edge> FaultTolerance::findBridges() const {
    std::vector<std::vector<std::pair<int, int>>> adjacency(vertexCount);
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        adjacency[edges[i].u].push_back({edges[i].v, i});
        adjacency[edges[i].v].push_back({edges[i].u, i});
    }

    std::vector<int> discovery(vertexCount, -1);
    std::vector<int> low(vertexCount, -1);
    std::vector<bool> isBridge(edges.size(), false);
    int time = 0;

    std::function<void(int, int)> visit = [&](int vertex, int parentEdge) {
        discovery[vertex] = low[vertex] = time++;

        for (const auto& adjacent : adjacency[vertex]) {
            const int next = adjacent.first;
            const int edgeIndex = adjacent.second;

            if (edgeIndex == parentEdge) {
                continue;
            }

            if (discovery[next] == -1) {
                visit(next, edgeIndex);
                low[vertex] = std::min(low[vertex], low[next]);
                if (low[next] > discovery[vertex]) {
                    isBridge[edgeIndex] = true;
                }
            } else {
                low[vertex] = std::min(low[vertex], discovery[next]);
            }
        }
    };

    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        if (discovery[vertex] == -1) {
            visit(vertex, -1);
        }
    }

    std::vector<FT_Edge> bridges;
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        if (isBridge[i]) {
            bridges.push_back(edges[i]);
        }
    }
    return bridges;
}

std::vector<FailureResult> FaultTolerance::testSingleEdgeFailures() const {
    std::vector<FailureResult> results;

    for (int failedEdge = 0;
         failedEdge < static_cast<int>(edges.size());
         ++failedEdge) {
        std::vector<bool> visited(vertexCount, false);
        std::queue<int> pending;

        if (vertexCount > 0) {
            pending.push(0);
            visited[0] = true;
        }

        while (!pending.empty()) {
            const int current = pending.front();
            pending.pop();

            for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
                if (i == failedEdge) {
                    continue;
                }

                int next = -1;
                if (edges[i].u == current) {
                    next = edges[i].v;
                } else if (edges[i].v == current) {
                    next = edges[i].u;
                }

                if (next != -1 && !visited[next]) {
                    visited[next] = true;
                    pending.push(next);
                }
            }
        }

        bool connected = true;
        for (bool reachable : visited) {
            if (!reachable) {
                connected = false;
                break;
            }
        }
        results.push_back({failedEdge, connected});
    }

    return results;
}

std::vector<ExtraLink> FaultTolerance::optimizeBudget(
    const std::vector<ExtraLink>& links,
    int budget
) const {
    if (budget < 0) {
        return {};
    }

    std::vector<ExtraLink> candidates = links;
    for (const auto& link : candidates) {
        if (link.cost < 0) {
            throw std::invalid_argument("Link cost must be non-negative");
        }
    }

    std::stable_sort(
        candidates.begin(),
        candidates.end(),
        [](const ExtraLink& left, const ExtraLink& right) {
            return left.cost < right.cost;
        }
    );

    std::vector<ExtraLink> selected;
    int remainingBudget = budget;
    for (const auto& link : candidates) {
        if (link.cost <= remainingBudget) {
            selected.push_back(link);
            remainingBudget -= link.cost;
        }
    }
    return selected;
}