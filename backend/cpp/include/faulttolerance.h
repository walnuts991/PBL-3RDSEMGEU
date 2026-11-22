#ifndef FAULTTOLERANCE_H
#define FAULTTOLERANCE_H
#include "Graph.h"
#include <vector>
struct FT_Edge {
    int id;
    int fromId;
    int toId;
    double cost;
};
struct FailureResult {
    int lineId;
    int componentsAfterRemoval;
    bool networkRemainsConnected;
};
struct AugmentationResult {
    std::vector<FT_Edge> selectedLinks;
    std::vector<int> remainingBridgeIds;
    double budgetUsed = 0.0;
    double budgetRemaining = 0.0;
    int initialBridgeCount = 0;
    int finalBridgeCount = 0;
};
class FaultTolerance {
private:
    const Graph& graph;
    int countComponents(const std::vector<int>& extraFrom,
                        const std::vector<int>& extraTo,
                        int removedEdgeIndex = -1) const;
    std::vector<int> findBridgesInternal(const std::vector<int>& extraFrom,
                                         const std::vector<int>& extraTo,
                                         const std::vector<int>& extraIds) const;
public:
    explicit FaultTolerance(const Graph& inputGraph);
    std::vector<int> findBridges() const;
    std::vector<FailureResult> testSingleEdgeFailures() const;
    AugmentationResult optimizeBudget(const std::vector<FT_Edge>& candidates,
                                      double budget) const;
};
#endif
