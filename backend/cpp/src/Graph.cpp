#include "Graph.h"
#include <fstream>
#include <iostream>

using namespace std;

bool Graph::hasSubstationId(int id) const {
    for (const auto& s : substations) {
        if (s.getId() == id) {
            return true;
        }
    }
    return false;
}

bool Graph::addSubstation(const Substation& s) {
    if (s.getId() < 0 || s.getName().empty() ||
        s.getLoadDemand() < 0) {
        return false;
    }

    if (hasSubstationId(s.getId())) {
        return false;
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

    if (!hasSubstationId(line.getFrom()) ||
        !hasSubstationId(line.getTo())) {
        return false;
    }

    for (const auto& existing : lines) {
        if (existing.getId() == line.getId()) {
            return false;
        }

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

const vector<Substation>& Graph::getSubstations() const {
    return substations;
}

const vector<TransmissionLine>& Graph::getLines() const {
    return lines;
}

bool Graph::getSubstationIndex(int substationId, int& index) const {
    for (int i = 0; i < static_cast<int>(substations.size()); i++) {
        if (substations[i].getId() == substationId) {
            index = i;
            return true;
        }
    }

    return false;
}

const Substation* Graph::findSubstation(int substationId) const {
    for (const auto& substation : substations) {
        if (substation.getId() == substationId) {
            return &substation;
        }
    }

    return nullptr;
}

bool Graph::saveForAlgorithms(const string& filename) const {
    ofstream file(filename);

    if (!file) {
        return false;
    }

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