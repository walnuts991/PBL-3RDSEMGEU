#include <iostream>
#include "Graph.h"

using namespace std;

int main() {
    Graph grid;

    cout << "=== GridGuard: Module 1 ===\n\n";

    // 1. Add substations
    cout << "--- Adding Substations ---\n";

    cout << "North: "
         << (grid.addSubstation({1, "North", 100})
                 ? "Added" : "Rejected") << '\n';

    cout << "Central: "
         << (grid.addSubstation({2, "Central", 150})
                 ? "Added" : "Rejected") << '\n';

    cout << "South: "
         << (grid.addSubstation({3, "South", 120})
                 ? "Added" : "Rejected") << '\n';

    cout << "Duplicate ID: "
         << (grid.addSubstation({1, "Duplicate", 50})
                 ? "Unexpectedly added" : "Correctly rejected")
         << "\n\n";

    // 2. Add transmission lines
    cout << "--- Adding Transmission Lines ---\n";

    cout << "Line 1 (1-2): "
         << (grid.addLine({1, 1, 2, 10.0, 1.2, 100.0, 0.0})
                 ? "Added" : "Rejected") << '\n';

    cout << "Line 2 (2-3): "
         << (grid.addLine({2, 2, 3, 15.0, 1.0, 120.0, 0.0})
                 ? "Added" : "Rejected") << '\n';

    cout << "Line 3 (1-3): "
         << (grid.addLine({3, 1, 3, 20.0, 1.5, 80.0, 0.0})
                 ? "Added" : "Rejected") << "\n\n";

    // 3. Display graph statistics
    cout << "--- Graph Statistics ---\n";
    cout << "Substations: " << grid.vertexCount() << '\n';
    cout << "Transmission lines: " << grid.edgeCount() << "\n\n";

    // 4. Display construction costs
    cout << "--- Construction Cost Report ---\n";

    for (const auto& line : grid.getLines()) {
        cout << "Line " << line.id << ": "
             << line.from << " - " << line.to
             << " | Distance: " << line.distance
             << " | Terrain Factor: " << line.terrainFactor
             << " | Cost: " << line.constructionCost << '\n';
    }

    // 5. Display adjacency list
    cout << "\n--- Adjacency List ---\n";

    for (const auto& entry : grid.getAdjacencyList()) {
        cout << "Substation " << entry.first << ": ";

        for (const auto& neighbor : entry.second) {
            cout << neighbor.first
                 << " (line " << neighbor.second << ") ";
        }

        cout << '\n';
    }

    // 6. Test duplicate connection rejection
    cout << "\n--- Duplicate Line Test ---\n";

    bool duplicateLine = grid.addLine(
        {4, 2, 1, 12.0, 1.0, 90.0, 0.0}
    );

    cout << (duplicateLine
                 ? "FAIL: Duplicate line accepted"
                 : "PASS: Duplicate line rejected")
         << '\n';

    // 7. Final counts
    cout << "\n--- Final Graph Statistics ---\n";
    cout << "Substations: " << grid.vertexCount() << '\n';
    cout << "Transmission lines: " << grid.edgeCount() << '\n';

    return 0;
}