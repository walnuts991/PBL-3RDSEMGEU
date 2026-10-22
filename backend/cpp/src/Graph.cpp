
#include "Graph.h"
#include <fstream>
#include <iostream>
#include <unordered_set>
using namespace std;

bool Graph::addSubstation(const Substation& s) {
    if (s.getId() < 0 || s.getName().empty() ||
        s.getLoadDemand() < 0) {
        return false;
    }

    for (const auto& existing : substations) {
        if (existing.getId() == s.getId()) {
            return false;
        }
    }

    substations.push_back(s);
    return true;
}

bool Graph::addLine(const TransmissionLine& line) {
    if (line.getId() < 0 ||
        line.getFrom() == line.getTo() ||
        line.getDistance() <= 0 ||
        line.getCapacity() <= 0 ||
        line.getConstructionCost() <= 0) {
        return false;
    }

    bool fromExists = false;
    bool toExists = false;

    for (const auto& s : substations) {
        if (s.getId() == line.getFrom()) fromExists = true;
        if (s.getId() == line.getTo()) toExists = true;
    }

    if (!fromExists || !toExists) return false;

    for (const auto& existing : lines) {
        if (existing.getId() == line.getId()) return false;

        if ((existing.getFrom() == line.getFrom() &&
             existing.getTo() == line.getTo()) ||
            (existing.getFrom() == line.getTo() &&
             existing.getTo() == line.getFrom())) {
            return false;
        }
    }

    lines.push_back(line);
    return true;
}

void Graph::display() const {
    cout << "\n--- Substations ---\n";
    for (const auto& s : substations) {
        s.display();
    }

    cout << "\n--- Transmission Lines ---\n";
    for (const auto& line : lines) {
        line.display();
    }

    cout << "\nSubstations: " << vertexCount()
         << "\nLines: " << edgeCount() << '\n';
}

int Graph::vertexCount() const {
    return static_cast<int>(substations.size());
}

int Graph::edgeCount() const {
    return static_cast<int>(lines.size());
}

// File format: vertex count, vertices, edge count, edges.
bool Graph::saveForAlgorithms(const string& filename) const {
    ofstream file(filename);

    if (!file) return false;

    file << substations.size() << '\n';

    for (const auto& s : substations) {
        file << s.getId() << ' '
             << s.getName() << ' '
             << s.getLoadDemand() << '\n';
    }

    file << lines.size() << '\n';

    for (const auto& line : lines) {
        file << line.getId() << ' '
             << line.getFrom() << ' '
             << line.getTo() << ' '
             << line.getDistance() << ' '
             << line.getConstructionCost() << ' '
             << line.getCapacity() << '\n';
    }

    return static_cast<bool>(file);
}
