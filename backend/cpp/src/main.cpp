#include "Graph.h"
#include "MST.h"
#include "faulttolerance.h"

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;


// ============================================
// DISPLAY MST RESULTS
// ============================================

void displayMSTResult(const MSTResult& result,
                      const string& algorithmName) {

    cout << "\n--- " << algorithmName << " ---\n";

    for (const auto& edge : result.selectedEdges) {

        cout << "Line " << edge.lineId
             << ": " << edge.fromId
             << " - " << edge.toId
             << " | Cost: " << edge.cost << '\n';
    }

    cout << "Total Construction Cost: "
         << result.totalCost << '\n';

    cout << "Connected Components: "
         << result.componentCount << '\n';

    if (result.isSpanningTree) {
        cout << "Result: Minimum Spanning Tree\n";
    }
    else {
        cout << "Result: Minimum Spanning Forest\n";
    }
}


// ============================================
// MAIN FUNCTION
// ============================================

int main() {

    cout << fixed << setprecision(2);

    // ========================================
    // MODULE 1: GRAPH CREATION
    // ========================================

    cout << "\n========== GRIDGUARD ==========\n";
    cout << "\n=== MODULE 1: GRAPH CREATION ===\n";

    Graph grid;

    // Create substations.
    grid.addSubstation(
        Substation(1, "North", 0, 0, "generator", 200, 0)
    );

    grid.addSubstation(
        Substation(2, "Central", 5, 2, "load", 0, 100)
    );

    grid.addSubstation(
        Substation(3, "South", 8, 5, "load", 0, 80)
    );

    grid.addSubstation(
        Substation(4, "East", 12, 3, "load", 0, 60)
    );


    // Test duplicate substation ID.
    cout << "\nDuplicate Substation Test: ";

    if (!grid.addSubstation(
            Substation(1, "Duplicate", 0, 0, "load", 0, 50))) {

        cout << "PASS\n";
    }
    else {
        cout << "FAIL\n";
    }


    // Add transmission lines.
    grid.addLine(
        TransmissionLine(1, 1, 2, 10, 1.2, 100)
    );

    grid.addLine(
        TransmissionLine(2, 2, 3, 15, 1.0, 120)
    );

    grid.addLine(
        TransmissionLine(3, 1, 3, 20, 1.5, 80)
    );

    grid.addLine(
        TransmissionLine(4, 3, 4, 8, 1.0, 90)
    );


    // Test duplicate connection.
    cout << "Duplicate Transmission Line Test: ";

    if (!grid.addLine(
            TransmissionLine(5, 2, 1, 12, 1.0, 90))) {

        cout << "PASS\n";
    }
    else {
        cout << "FAIL\n";
    }


    // Display graph.
    grid.display();


    // Save graph data.
    if (grid.saveForAlgorithms("../../data/grid.txt")) {
        cout << "\nGrid data saved successfully.\n";
    }
    else {
        cout << "\nCould not save grid data.\n";
    }


    // ========================================
    // MODULE 2: MINIMUM SPANNING TREE
    // ========================================

    cout << "\n=== MODULE 2: MINIMUM COST NETWORK ===\n";

    // Kruskal's Algorithm.
    MSTResult kruskalResult = MST::kruskal(grid);

    displayMSTResult(
        kruskalResult,
        "KRUSKAL'S ALGORITHM"
    );


    // Prim's Algorithm.
    MSTResult primResult = MST::prim(grid);

    displayMSTResult(
        primResult,
        "PRIM'S ALGORITHM"
    );


    // ========================================
    // MODULE 3: FAULT TOLERANCE
    // ========================================

    cout << "\n=== MODULE 3: FAULT TOLERANCE ===\n";

    FaultTolerance faultNetwork(grid);


    // ----------------------------------------
    // 1. BRIDGE DETECTION
    // ----------------------------------------

    cout << "\n--- Critical Transmission Lines ---\n";

    vector<int> bridges = faultNetwork.findBridges();

    if (bridges.empty()) {

        cout << "No bridge connections found.\n";
    }
    else {

        for (int lineId : bridges) {

            cout << "Critical Line ID: "
                 << lineId << '\n';
        }
    }


    // ----------------------------------------
    // 2. SINGLE EDGE FAILURE TEST
    // ----------------------------------------

    cout << "\n--- Single Transmission Line Failure Test ---\n";

    vector<FailureResult> failures =
        faultNetwork.testSingleEdgeFailures();

    for (const auto& result : failures) {

        cout << "Line " << result.lineId << ": ";

        if (result.networkRemainsConnected) {

            cout << "Network remains connected\n";
        }
        else {

            cout << "NETWORK DISCONNECTED\n";
        }

        cout << "Components after failure: "
             << result.componentsAfterRemoval << '\n';
    }


    // ----------------------------------------
    // 3. BUDGET-BASED NETWORK AUGMENTATION
    // ----------------------------------------

    cout << "\n--- Budget-Based Network Augmentation ---\n";

    // Proposed additional transmission lines.
    // These are candidate links, not existing lines.

    vector<FT_Edge> candidates = {

        {5, 4, 1, 50.0},
        {6, 4, 2, 60.0}
    };

    double budget = 100.0;

    AugmentationResult augmentation =
        faultNetwork.optimizeBudget(candidates, budget);


    cout << "Available Budget: "
         << budget << '\n';

    cout << "Initial Bridge Count: "
         << augmentation.initialBridgeCount << '\n';


    cout << "\nSelected Additional Links:\n";

    if (augmentation.selectedLinks.empty()) {

        cout << "No additional links selected.\n";
    }
    else {

        for (const auto& link : augmentation.selectedLinks) {

            cout << "Line " << link.id
                 << ": " << link.fromId
                 << " - " << link.toId
                 << " | Cost: " << link.cost << '\n';
        }
    }


    cout << "\nBudget Used: "
         << augmentation.budgetUsed << '\n';

    cout << "Remaining Budget: "
         << augmentation.budgetRemaining << '\n';

    cout << "Final Bridge Count: "
         << augmentation.finalBridgeCount << '\n';


    // ----------------------------------------
    // FINAL STATUS
    // ----------------------------------------

    cout << "\n========== GRIDGUARD SUMMARY ==========\n";

    cout << "Module 1: Graph Creation Executed\n";
    cout << "Module 2: Kruskal and Prim Executed\n";
    cout << "Module 3: Fault Tolerance Executed\n";

    cout << "\nGridGuard execution completed.\n";

    return 0;
}