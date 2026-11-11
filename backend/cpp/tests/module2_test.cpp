
#include "DSU.h"
#include "MST.h"

#include <cassert>
#include <cmath>
#include <iostream>

bool closeEnough(double a, double b) {
    return std::abs(a - b) < 1e-9;
}

int main() {

    // =====================================
    // TEST 1: DISJOINT SET UNION
    // =====================================

    DSU dsu(5);

    assert(dsu.getComponentCount() == 5);

    assert(dsu.unite(0, 1));
    assert(dsu.unite(1, 2));

    assert(!dsu.unite(0, 2));

    assert(dsu.connected(0, 2));
    assert(!dsu.connected(0, 4));

    assert(dsu.getComponentCount() == 3);


    // =====================================
    // TEST 2: CONNECTED GRAPH
    // =====================================

    Graph graph;

    graph.addSubstation(
        Substation(10, "A", 0, 0, "generator", 50, 0)
    );

    graph.addSubstation(
        Substation(25, "B", 1, 0, "load", 0, 20)
    );

    graph.addSubstation(
        Substation(40, "C", 2, 0, "load", 0, 15)
    );

    graph.addSubstation(
        Substation(70, "D", 3, 0, "load", 0, 10)
    );


    // Construction cost = distance * terrain factor * 10.

    graph.addLine(
        TransmissionLine(1, 10, 25, 1, 1, 30)
    );

    graph.addLine(
        TransmissionLine(2, 25, 40, 1, 1, 30)
    );

    graph.addLine(
        TransmissionLine(3, 40, 70, 1, 1, 30)
    );

    graph.addLine(
        TransmissionLine(4, 10, 40, 2, 1, 30)
    );

    graph.addLine(
        TransmissionLine(5, 25, 70, 3, 1, 30)
    );


    MSTResult kruskalResult = MST::kruskal(graph);
    MSTResult primResult = MST::prim(graph);

    assert(kruskalResult.isSpanningTree);
    assert(primResult.isSpanningTree);

    assert(kruskalResult.componentCount == 1);
    assert(primResult.componentCount == 1);

    assert(kruskalResult.selectedEdges.size() == 3);
    assert(primResult.selectedEdges.size() == 3);

    assert(closeEnough(kruskalResult.totalCost, 30));
    assert(closeEnough(primResult.totalCost, 30));


    // =====================================
    // TEST 3: DISCONNECTED GRAPH
    // =====================================

    Graph disconnectedGraph;

    disconnectedGraph.addSubstation(
        Substation(1, "P", 0, 0, "generator", 5, 0)
    );

    disconnectedGraph.addSubstation(
        Substation(2, "Q", 1, 0, "load", 0, 2)
    );

    disconnectedGraph.addSubstation(
        Substation(3, "R", 2, 0, "load", 0, 2)
    );

    disconnectedGraph.addSubstation(
        Substation(4, "S", 3, 0, "load", 0, 2)
    );

    disconnectedGraph.addLine(
        TransmissionLine(11, 1, 2, 1, 1, 10)
    );

    disconnectedGraph.addLine(
        TransmissionLine(12, 3, 4, 2, 1, 10)
    );


    MSTResult dk = MST::kruskal(disconnectedGraph);
    MSTResult dp = MST::prim(disconnectedGraph);

    assert(!dk.isSpanningTree);
    assert(!dp.isSpanningTree);

    assert(dk.componentCount == 2);
    assert(dp.componentCount == 2);

    assert(dk.selectedEdges.size() == 2);
    assert(dp.selectedEdges.size() == 2);

    assert(closeEnough(dk.totalCost, 30));
    assert(closeEnough(dp.totalCost, 30));


    std::cout << "Module 2 tests passed." << std::endl;

    return 0;
}
