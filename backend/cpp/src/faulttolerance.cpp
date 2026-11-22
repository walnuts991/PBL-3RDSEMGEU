#include "faulttolerance.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <stdexcept>

FaultTolerance::FaultTolerance(const Graph& inputGraph) : graph(inputGraph) {}

int FaultTolerance::countComponents(const std::vector<int>& extraFrom,
                                    const std::vector<int>& extraTo,
                                    int removedEdgeIndex) const {
    const int n = graph.vertexCount();
    std::vector<std::vector<int>> adj(n);
    const auto& lines = graph.getLines();
    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        if (i == removedEdgeIndex) continue;
        int u=-1, v=-1;
        if (!graph.getSubstationIndex(lines[i].getFrom(),u) ||
            !graph.getSubstationIndex(lines[i].getTo(),v))
            throw std::invalid_argument("Invalid transmission line endpoint");
        adj[u].push_back(v); adj[v].push_back(u);
    }
    for (std::size_t i=0; i<extraFrom.size(); ++i) {
        int u=-1,v=-1;
        if (!graph.getSubstationIndex(extraFrom[i],u) || !graph.getSubstationIndex(extraTo[i],v))
            throw std::invalid_argument("Invalid candidate link endpoint");
        adj[u].push_back(v); adj[v].push_back(u);
    }
    std::vector<bool> seen(n,false);
    int components=0;
    for (int start=0; start<n; ++start) if (!seen[start]) {
        ++components; std::queue<int> q; q.push(start); seen[start]=true;
        while(!q.empty()) { int u=q.front(); q.pop(); for(int v:adj[u]) if(!seen[v]) {seen[v]=true;q.push(v);} }
    }
    return components;
}

std::vector<int> FaultTolerance::findBridgesInternal(
    const std::vector<int>& extraFrom, const std::vector<int>& extraTo,
    const std::vector<int>& extraIds) const {
    const int n=graph.vertexCount();
    struct Edge { int u,v,id; };
    std::vector<Edge> edges;
    for (const auto& line: graph.getLines()) {
        int u=-1,v=-1;
        if(!graph.getSubstationIndex(line.getFrom(),u) || !graph.getSubstationIndex(line.getTo(),v))
            throw std::invalid_argument("Invalid transmission line endpoint");
        edges.push_back({u,v,line.getId()});
    }
    for(std::size_t i=0;i<extraFrom.size();++i) {
        int u=-1,v=-1;
        if(!graph.getSubstationIndex(extraFrom[i],u) || !graph.getSubstationIndex(extraTo[i],v))
            throw std::invalid_argument("Invalid candidate link endpoint");
        edges.push_back({u,v,extraIds[i]});
    }
    std::vector<std::vector<std::pair<int,int>>> adj(n);
    for(int i=0;i<(int)edges.size();++i) { adj[edges[i].u].push_back({edges[i].v,i}); adj[edges[i].v].push_back({edges[i].u,i}); }
    std::vector<int> tin(n,-1), low(n,-1), bridges;
    int timer=0;
    std::function<void(int,int)> dfs=[&](int u,int parentEdge) {
        tin[u]=low[u]=timer++;
      for (const auto& edge : adj[u]) {
    int v = edge.first;
    int eid = edge.second;

    if (eid == parentEdge) continue;

    if (tin[v] != -1) {
        low[u] = std::min(low[u], tin[v]);
    }
    else {
        dfs(v, eid);
        low[u] = std::min(low[u], low[v]);

        if (low[v] > tin[u]) {
            bridges.push_back(edges[eid].id);
        }
    }
}
    };
    for(int i=0;i<n;++i) if(tin[i]==-1) dfs(i,-1);
    std::sort(bridges.begin(),bridges.end());
    return bridges;
}

std::vector<int> FaultTolerance::findBridges() const {
    return findBridgesInternal({}, {}, {});
}

std::vector<FailureResult> FaultTolerance::testSingleEdgeFailures() const {
    std::vector<FailureResult> results;
    const auto& lines=graph.getLines();
    for(int i=0;i<(int)lines.size();++i) {
        int components=countComponents({}, {}, i);
        results.push_back({lines[i].getId(),components,components<=1});
    }
    return results;
}

AugmentationResult FaultTolerance::optimizeBudget(const std::vector<FT_Edge>& candidates,
                                                   double budget) const {
    if(!std::isfinite(budget) || budget<0) throw std::invalid_argument("Budget must be non-negative");
    AugmentationResult result;
    result.initialBridgeCount=static_cast<int>(findBridges().size());
    result.budgetRemaining=budget;
    std::vector<FT_Edge> remaining=candidates;
    std::vector<int> extraFrom,extraTo,extraIds;
    while(true) {
        auto current=findBridgesInternal(extraFrom,extraTo,extraIds);
        int best=-1, bestReduction=0;
        for(int i=0;i<(int)remaining.size();++i) {
            const auto& c=remaining[i];
            if(!std::isfinite(c.cost) || c.cost<=0 || c.cost>result.budgetRemaining) continue;
            if(c.id<0 || !graph.findSubstation(c.fromId) || !graph.findSubstation(c.toId) || c.fromId==c.toId) continue;
            auto f=extraFrom,t=extraTo,ids=extraIds;
            f.push_back(c.fromId); t.push_back(c.toId); ids.push_back(c.id);
            int reduction=static_cast<int>(current.size())-static_cast<int>(findBridgesInternal(f,t,ids).size());
            if(reduction>bestReduction || (reduction==bestReduction && reduction>0 && best>=0 && c.cost<remaining[best].cost)) {
                best=i; bestReduction=reduction;
            }
        }
        if(best<0) break;
        FT_Edge chosen=remaining[best];
        result.selectedLinks.push_back(chosen);
        extraFrom.push_back(chosen.fromId); extraTo.push_back(chosen.toId); extraIds.push_back(chosen.id);
        result.budgetUsed+=chosen.cost; result.budgetRemaining-=chosen.cost;
        remaining.erase(remaining.begin()+best);
    }
    result.remainingBridgeIds=findBridgesInternal(extraFrom,extraTo,extraIds);
    result.finalBridgeCount=static_cast<int>(result.remainingBridgeIds.size());
    return result;
}
