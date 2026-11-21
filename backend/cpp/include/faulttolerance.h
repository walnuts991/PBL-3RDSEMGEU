<<<<<<< HEAD
#ifndef FAULT_TOLERANCE_H
#define FAULT_TOLERANCE_H

#include <vector>

struct FT_Edge {
    int u;
    int v;
    int cost;
};

struct FailureResult {
    int edgeIndex;
    bool networkConnected;
};

struct ExtraLink {
    int u;
    int v;
    int cost;
=======
#ifndef FAULTTOLERANCE_H
#define FAULTTOLERANCE_H

#include "Graph.h"

#include <vector>

// Represents a proposed additional transmission line.
// fromId and toId are actual substation IDs, not vector indices.
struct FT_Edge {
    int id;
    int fromId;
    int toId;
    double cost;
};

// Result of simulating the failure of one existing line.
struct FailureResult {
    int lineId;
    int componentsAfterRemoval;
    bool networkRemainsConnected;
};

// Result of budget-based network augmentation.
struct AugmentationResult {
    std::vector<FT_Edge> selectedLinks;
    std::vector<int> remainingBridgeIds;

    double budgetUsed = 0.0;
    double budgetRemaining = 0.0;

    int initialBridgeCount = 0;
    int finalBridgeCount = 0;
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
};

class FaultTolerance {
private:
<<<<<<< HEAD
    int vertexCount;
    std::vector<FT_Edge> edges;

public:
    FaultTolerance(int vertexCount, const std::vector<FT_Edge>& edges);

    std::vector<FT_Edge> findBridges() const;
    std::vector<FailureResult> testSingleEdgeFailures() const;
    std::vector<ExtraLink> optimizeBudget(
        const std::vector<ExtraLink>& links,
        int budget
=======
    const Graph& graph;

    // Returns the number of connected components.
    int countComponents(
        const std::vector<int>& extraFrom,
        const std::vector<int>& extraTo,
        int removedEdgeIndex = -1
    ) const;

    // Finds bridge edge IDs, including selected extra links.
    std::vector<int> findBridgesInternal(
        const std::vector<int>& extraFrom,
        const std::vector<int>& extraTo,
        const std::vector<int>& extraIds
    ) const;

public:
    explicit FaultTolerance(const Graph& inputGraph);

    // Finds bridges in the original graph.
    std::vector<int> findBridges() const;

    // Simulates the failure of every existing transmission line.
    std::vector<FailureResult> testSingleEdgeFailures() const;

    // Greedily selects candidate links within the given budget.
    AugmentationResult optimizeBudget(
        const std::vector<FT_Edge>& candidates,
        double budget
>>>>>>> a287298b80dbb29ca2923eead7a29575f3b61534
    ) const;
};

#endif