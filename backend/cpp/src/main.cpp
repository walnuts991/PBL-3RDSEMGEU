
#include "Graph.h"
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

    if (grid.saveForAlgorithms("data/grid.txt")) {
        cout << "\nGrid data saved to data/grid.txt\n";
    } else {
        cout << "\nCould not save grid data. Check the data folder.\n";
    }

    return 0;
}
