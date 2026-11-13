#include "faulttolerance.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    // =========================================
    // CREATE A SAMPLE POWER GRID
    // =========================================

    Graph graph;

    graph.addSubstation(
        Substation(10, "A", 0, 0, "generator", 100, 0)
    );

    graph.addSubstation(
        Substation(20, "B", 1, 0, "load", 0, 20)
    );

    graph.addSubstation(
        Substation(30, "C", 2, 0, "load", 0, 20)
    );

    graph.addSubstation(
        Substation(40, "D", 3, 0, "load", 0, 20)
    );


    // A-B-C-A forms a cycle.
    // C-D is a bridge.

    graph.addLine(
        TransmissionLine(1, 10, 20, 1, 1, 50)
    );

    graph.addLine(
        TransmissionLine(2, 20, 30, 1, 1, 50)
    );

    graph.addLine(
        TransmissionLine(3, 30, 10, 1, 1, 50)
    );

    graph.addLine(
        TransmissionLine(4, 30, 40, 1, 1, 50)
    );


    FaultTolerance ft(graph);


    // =========================================
    // TEST 1: BRIDGE DETECTION
    // =========================================

    std::vector<int> bridges = ft.findBridges();

    assert(bridges.size() == 1);
    assert(bridges[0] == 4);


    // =========================================
    // TEST 2: SINGLE EDGE FAILURE
    // =========================================

    std::vector<FailureResult> failures =
        ft.testSingleEdgeFailures();

    assert(failures.size() == 4);

    // The first three lines belong to a cycle.
    assert(failures[0].networkRemainsConnected);
    assert(failures[1].networkRemainsConnected);
    assert(failures[2].networkRemainsConnected);

    // Removing line 4 disconnects the network.
    assert(!failures[3].networkRemainsConnected);
    assert(failures[3].componentsAfterRemoval == 2);


    // =========================================
    // TEST 3: BUDGET-BASED AUGMENTATION
    // =========================================

    std::vector<FT_Edge> candidates = {

        // Additional connection from D to A.
        {5, 40, 10, 15.0},

        // Another possible connection.
        {6, 40, 20, 25.0}
    };

    double budget = 20.0;

    AugmentationResult result =
        ft.optimizeBudget(candidates, budget);

    assert(result.initialBridgeCount == 1);

    assert(result.selectedLinks.size() == 1);

    assert(result.selectedLinks[0].id == 5);

    assert(std::abs(result.budgetUsed - 15.0) < 1e-9);

    assert(std::abs(result.budgetRemaining - 5.0) < 1e-9);

    assert(result.finalBridgeCount == 0);


    // =========================================
    // DISPLAY RESULTS
    // =========================================

    std::cout << "Initial bridges: "
              << result.initialBridgeCount << std::endl;

    std::cout << "Selected additional links: "
              << result.selectedLinks.size() << std::endl;

    std::cout << "Budget used: "
              << result.budgetUsed << std::endl;

    std::cout << "Remaining bridges: "
              << result.finalBridgeCount << std::endl;

    std::cout << "\nModule 3 tests passed!"
              << std::endl;

    return 0;
}