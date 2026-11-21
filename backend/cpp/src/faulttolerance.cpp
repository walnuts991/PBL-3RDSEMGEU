#include "faulttolerance.h"

#include <algorithm>
<<<<<<< HEAD
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

=======
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <utility>

FaultTolerance::FaultTolerance(const Graph& inputGraph)
    : graph(inputGraph) {}


// ============================================
// COUNT CONNECTED COMPONENTS
// ============================================

int FaultTolerance::countComponents(
    const std::vector<int>& extraFrom,
    const std::vector<int>& extraTo,
    int removedEdgeIndex
) const {

    int n = graph.vertexCount();

    struct AdjEdge {
        int to;
        int edgeIndex;
    };

    std::vector<std::vector<AdjEdge>> adjacency(n);

    const auto& lines = graph.getLines();

    // Add original transmission lines.
    for (int i = 0; i < static_cast<int>(lines.size()); i++) {

        if (i == removedEdgeIndex) {
            continue;
        }

        int u = -1;
        int v = -1;

        if (!graph.getSubstationIndex(lines[i].getFrom(), u) ||
            !graph.getSubstationIndex(lines[i].getTo(), v)) {
            throw std::invalid_argument(
                "Transmission line has an invalid endpoint"
            );
        }

        adjacency[u].push_back({v, i});
        adjacency[v].push_back({u, i});
    }

    // Add selected extra links.
    for (int i = 0; i < static_cast<int>(extraFrom.size()); i++) {

        int edgeIndex = static_cast<int>(lines.size()) + i;

        adjacency[extraFrom[i]].push_back(
            {extraTo[i], edgeIndex}
        );

        adjacency[extraTo[i]].push_back(
            {extraFrom[i], edgeIndex}
        );
    }

    std::vector<bool> visited(n, false);

    int components = 0;

    // DFS to count connected components.
    for (int start = 0; start < n; start++) {

        if (visited[start]) {
            continue;
        }

        components++;

        std::vector<int> stack;
        stack.push_back(start);

        visited[start] = true;

        while (!stack.empty()) {

            int current = stack.back();
            stack.pop_back();

            for (const auto& edge : adjacency[current]) {

                if (!visited[edge.to]) {

                    visited[edge.to] = true;
                    stack.push_back(edge.to);
                }
            }
        }
    }

    return components;
}


// ============================================
// FIND BRIDGES USING DFS
// ============================================

std::vector<int> FaultTolerance::findBridgesInternal(
    const std::vector<int>& extraFrom,
    const std::vector<int>& extraTo,
    const std::vector<int>& extraIds
) const {

    int n = graph.vertexCount();

    struct AdjEdge {
        int to;
        int edgeIndex;
    };

    std::vector<std::vector<AdjEdge>> adjacency(n);

    const auto& lines = graph.getLines();

    // Add original edges.
    for (int i = 0; i < static_cast<int>(lines.size()); i++) {

        int u = -1;
        int v = -1;

        if (!graph.getSubstationIndex(lines[i].getFrom(), u) ||
            !graph.getSubstationIndex(lines[i].getTo(), v)) {
            throw std::invalid_argument(
                "Transmission line has an invalid endpoint"
            );
        }

        adjacency[u].push_back({v, i});
        adjacency[v].push_back({u, i});
    }

    // Add extra edges.
    for (int i = 0; i < static_cast<int>(extraFrom.size()); i++) {

        int edgeIndex = static_cast<int>(lines.size()) + i;

        adjacency[extraFrom[i]].push_back(
            {extraTo[i], edgeIndex}
        );

        adjacency[extraTo[i]].push_back(
            {extraFrom[i], edgeIndex}
        );
    }

    std::vector<int> tin(n, -1);
    std::vector<int> low(n, -1);

    std::vector<int> bridges;

    int timer = 0;

    std::function<void(int, int)> dfs =
        [&](int vertex, int parentEdge) {

        tin[vertex] = low[vertex] = timer++;

        for (const auto& edge : adjacency[vertex]) {

            int next = edge.to;
            int edgeIndex = edge.edgeIndex;

            // Skip only the exact parent edge.
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
            if (edgeIndex == parentEdge) {
                continue;
            }

<<<<<<< HEAD
            if (discovery[next] == -1) {
                visit(next, edgeIndex);
                low[vertex] = std::min(low[vertex], low[next]);
                if (low[next] > discovery[vertex]) {
                    isBridge[edgeIndex] = true;
                }
            } else {
                low[vertex] = std::min(low[vertex], discovery[next]);
=======
            if (tin[next] != -1) {

                // Back edge.
                low[vertex] = std::min(
                    low[vertex],
                    tin[next]
                );
            }
            else {

                dfs(next, edgeIndex);

                low[vertex] = std::min(
                    low[vertex],
                    low[next]
                );

                // Bridge condition.
                if (low[next] > tin[vertex]) {

                    if (edgeIndex < static_cast<int>(lines.size())) {

                        bridges.push_back(
                            lines[edgeIndex].getId()
                        );
                    }
                    else {

                        int extraIndex =
                            edgeIndex - static_cast<int>(lines.size());

                        bridges.push_back(
                            extraIds[extraIndex]
                        );
                    }
                }
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
            }
        }
    };

<<<<<<< HEAD
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
=======
    // Handle disconnected graphs too.
    for (int i = 0; i < n; i++) {

        if (tin[i] == -1) {
            dfs(i, -1);
        }
    }

    std::sort(bridges.begin(), bridges.end());

    return bridges;
}


// ============================================
// PUBLIC BRIDGE DETECTION
// ============================================

std::vector<int> FaultTolerance::findBridges() const {

    return findBridgesInternal({}, {}, {});
}


// ============================================
// SINGLE EDGE FAILURE TESTING
// ============================================

std::vector<FailureResult>
FaultTolerance::testSingleEdgeFailures() const {

    std::vector<FailureResult> results;

    const auto& lines = graph.getLines();

    int originalComponents = countComponents({}, {});

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {

        int componentsAfterRemoval =
            countComponents({}, {}, i);

        FailureResult result;

        result.lineId = lines[i].getId();

        result.componentsAfterRemoval =
            componentsAfterRemoval;

        result.networkRemainsConnected =
            (componentsAfterRemoval <= originalComponents);

        results.push_back(result);
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
    }

    return results;
}

<<<<<<< HEAD
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
=======

// ============================================
// BUDGET-BASED NETWORK AUGMENTATION
// ============================================

AugmentationResult FaultTolerance::optimizeBudget(
    const std::vector<FT_Edge>& candidates,
    double budget
) const {

    if (!std::isfinite(budget) || budget < 0.0) {
        throw std::invalid_argument("Budget must be non-negative");
    }

    AugmentationResult result;

    result.budgetRemaining = budget;

    std::vector<int> extraFrom;
    std::vector<int> extraTo;
    std::vector<int> extraIds;

    std::unordered_set<int> usedIds;

    // Existing line IDs cannot be reused.
    for (const auto& line : graph.getLines()) {
        usedIds.insert(line.getId());
    }

    // Validate and convert candidate links.
    std::vector<FT_Edge> validCandidates;

    for (const auto& candidate : candidates) {

        int u = -1;
        int v = -1;

        if (candidate.id < 0 ||
            !std::isfinite(candidate.cost) ||
            candidate.cost <= 0.0 ||
            candidate.fromId == candidate.toId ||
            !graph.getSubstationIndex(candidate.fromId, u) ||
            !graph.getSubstationIndex(candidate.toId, v) ||
            usedIds.count(candidate.id)) {

            continue;
        }

        bool duplicate = false;

        for (const auto& existing : validCandidates) {

            bool samePair =
                (existing.fromId == candidate.fromId &&
                 existing.toId == candidate.toId) ||
                (existing.fromId == candidate.toId &&
                 existing.toId == candidate.fromId);

            if (samePair) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {

            validCandidates.push_back(candidate);
            usedIds.insert(candidate.id);
        }
    }

    // Count bridges before adding anything.
    auto initialBridges =
        findBridgesInternal({}, {}, {});

    result.initialBridgeCount =
        static_cast<int>(initialBridges.size());

    // Greedy selection:
    // Choose the affordable candidate with the best
    // immediate bridge reduction per unit cost.

    while (true) {

        auto currentBridges =
            findBridgesInternal(extraFrom, extraTo, extraIds);

        if (currentBridges.empty()) {
            break;
        }

        int bestIndex = -1;
        double bestScore = 0.0;

        for (int i = 0;
             i < static_cast<int>(validCandidates.size());
             i++) {

            const auto& candidate = validCandidates[i];

            bool alreadySelected = false;

            for (const auto& selected : result.selectedLinks) {

                if (selected.id == candidate.id) {
                    alreadySelected = true;
                    break;
                }
            }

            if (alreadySelected ||
                candidate.cost > result.budgetRemaining) {
                continue;
            }

            int u = -1;
            int v = -1;

            graph.getSubstationIndex(candidate.fromId, u);
            graph.getSubstationIndex(candidate.toId, v);

            auto testFrom = extraFrom;
            auto testTo = extraTo;
            auto testIds = extraIds;

            testFrom.push_back(u);
            testTo.push_back(v);
            testIds.push_back(candidate.id);

            auto newBridges =
                findBridgesInternal(testFrom, testTo, testIds);

            int reduction =
                static_cast<int>(currentBridges.size()) -
                static_cast<int>(newBridges.size());

            if (reduction <= 0) {
                continue;
            }

            double score = reduction / candidate.cost;

            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
            }
        }

        // No affordable candidate improves the bridge count.
        if (bestIndex == -1) {
            break;
        }

        const auto& selected = validCandidates[bestIndex];

        int u = -1;
        int v = -1;

        graph.getSubstationIndex(selected.fromId, u);
        graph.getSubstationIndex(selected.toId, v);

        extraFrom.push_back(u);
        extraTo.push_back(v);
        extraIds.push_back(selected.id);

        result.selectedLinks.push_back(selected);

        result.budgetUsed += selected.cost;
        result.budgetRemaining -= selected.cost;
    }

    auto finalBridges =
        findBridgesInternal(extraFrom, extraTo, extraIds);

    result.finalBridgeCount =
        static_cast<int>(finalBridges.size());

    result.remainingBridgeIds = finalBridges;

    return result;
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
}