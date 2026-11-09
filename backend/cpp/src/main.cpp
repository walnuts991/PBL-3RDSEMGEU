
#include "Graph.h"
#include "../include/FaultTolerance.h"
#include <iostream>
using namespace std;

int main() {
    Graph grid;

    cout << "=== GridGuard: Module 1 ===\n";

    grid.addSubstation(Substation(1, "North", 100));
    grid.addSubstation(Substation(2, "Central", 150));
    grid.addSubstation(Substation(3, "South", 120));

    // Test duplicate substation ID.
    cout << "Duplicate substation test: "
         << (grid.addSubstation(
                 Substation(1, "Duplicate", 50))
                 ? "FAIL" : "PASS") << '\n';

    grid.addLine(TransmissionLine(1, 1, 2, 10, 1.2, 100));
    grid.addLine(TransmissionLine(2, 2, 3, 15, 1.0, 120));
    grid.addLine(TransmissionLine(3, 1, 3, 20, 1.5, 80));

    // Test duplicate connection.
    cout << "Duplicate line test: "
         << (grid.addLine(
                 TransmissionLine(4, 2, 1, 12, 1.0, 90))
                 ? "FAIL" : "PASS") << '\n';

    grid.display();

    if (grid.saveForAlgorithms("../../data/grid.txt")) {
        cout << "\nGrid data saved to data/grid.txt\n";
    } else {
        cout << "\nCould not save grid data. Check the data folder.\n";
    }

    // ==============================
    // MODULE 3: FAULT TOLERANCE
    // AND BUDGET OPTIMIZATION
    // ==============================

    cout << "\n=== GridGuard: Module 3 ===\n";

    // Create edges from the existing network
    vector<FT_Edge> edges = {
        {0, 1, 10},
        {1, 2, 15},
        {0, 2, 20}
    };

    FaultTolerance faultNetwork(3, edges);

    // 1. Find bridges / vulnerable connections
    vector<FT_Edge> bridges =
        faultNetwork.findBridges();

    cout << "\n--- Vulnerable / Bridge Connections ---\n";

    if (bridges.empty()) {
        cout << "No bridge connections found.\n";
    } else {
        for (const auto& edge : bridges) {
            cout << "Connection: "
                 << edge.u << " - " << edge.v
                 << " | Cost: " << edge.cost << '\n';
        }
    }

    // 2. Test failure of every single edge
    vector<FailureResult> failures =
        faultNetwork.testSingleEdgeFailures();

    cout << "\n--- Single Edge Failure Test ---\n";

    for (const auto& result : failures) {
        cout << "Edge " << result.edgeIndex << ": ";

        if (result.networkConnected) {
            cout << "Network remains CONNECTED\n";
        } else {
            cout << "Network FAILURE\n";
        }
    }

    // 3. Test additional links within budget
    vector<ExtraLink> extraLinks = {
        {0, 2, 25},
        {1, 3, 30},
        {0, 3, 20}
    };

    int budget = 50;

    vector<ExtraLink> selectedLinks =
        faultNetwork.optimizeBudget(extraLinks, budget);

    cout << "\n--- Budget Optimization ---\n";
    cout << "Available Budget: " << budget << '\n';

    for (const auto& link : selectedLinks) {
        cout << "Additional Link: "
             << link.u << " - " << link.v
             << " | Cost: " << link.cost << '\n';
    }

    cout << "\nModule 3 completed successfully.\n";

    return 0;
}
