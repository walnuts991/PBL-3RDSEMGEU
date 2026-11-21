#ifndef MAX_FLOW_H
#define MAX_FLOW_H

#include "Graph.h"
#include <vector>

struct MaxFlowResult {
    double maximumFlowMW = 0.0;

    // Transmission lines crossing the minimum cut.
    std::vector<int> bottleneckLineIds;

    // Substations reachable from the virtual source
    // in the final residual graph.
    std::vector<int> sourceSideSubstationIds;
};

class MaxFlow {
public:
    // Calculates maximum transferable power from all
    // generators to all loads.
    static MaxFlowResult edmondsKarp(const Graph& graph);
};

#endif