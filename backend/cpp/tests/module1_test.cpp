#include "Graph.h"
#include <cassert>
#include <cstdio>
#include <iostream>

int main() {
    Graph graph;

    // Non-consecutive IDs demonstrate that algorithms must map IDs to indices.
    assert(graph.addSubstation(Substation(10, "North Hub", 0, 0,
                                          "generator", 100, 0)));
    assert(graph.addSubstation(Substation(25, "Central Load", 5, 2,
                                          "load", 0, 60)));
    assert(graph.addSubstation(Substation(40, "South Hub", 8, 7,
                                          "both", 30, 10)));

    assert(!graph.addSubstation(Substation(10, "Duplicate", 0, 0,
                                           "load", 0, 5)));
    assert(!graph.addSubstation(Substation(50, "Bad type", 0, 0,
                                           "storage", 5, 5)));

    assert(graph.addLine(TransmissionLine(1, 10, 25, 5.0, 1.2, 80.0)));
    assert(graph.addLine(TransmissionLine(2, 25, 40, 6.0, 1.1, 70.0)));
    assert(!graph.addLine(TransmissionLine(3, 25, 99, 2.0, 1.0, 20.0)));
    assert(!graph.addLine(TransmissionLine(4, 10, 10, 2.0, 1.0, 20.0)));
    assert(!graph.addLine(TransmissionLine(5, 25, 10, 5.0, 1.2, 80.0)));

    assert(graph.vertexCount() == 3);
    assert(graph.edgeCount() == 2);

    int index = -1;
    assert(graph.getSubstationIndex(25, index));
    assert(index == 1);
    assert(!graph.getSubstationIndex(999, index));

    const Substation* station = graph.findSubstation(40);
    assert(station != nullptr);
    assert(station->getSupplyMW() == 30.0);
    assert(station->getDemandMW() == 10.0);

    assert(graph.saveForAlgorithms("module1_test_output.txt"));
    std::remove("module1_test_output.txt");

    std::cout << "Module 1 tests passed.\\n";
    return 0;
}
