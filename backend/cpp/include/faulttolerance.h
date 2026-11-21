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
};

class FaultTolerance {
private:
    int vertexCount;
    std::vector<FT_Edge> edges;

public:
    FaultTolerance(int vertexCount, const std::vector<FT_Edge>& edges);

    std::vector<FT_Edge> findBridges() const;
    std::vector<FailureResult> testSingleEdgeFailures() const;
    std::vector<ExtraLink> optimizeBudget(
        const std::vector<ExtraLink>& links,
        int budget
    ) const;
};

#endif