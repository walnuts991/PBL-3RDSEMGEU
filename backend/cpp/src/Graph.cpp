#include "Graph.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_set>

bool Graph::hasSubstationId(int targetId) const {
    for (const auto& station : substations) {
        if (station.getId() == targetId) return true;
    }
    return false;
}

bool Graph::addSubstation(const Substation& station) {
    if (station.getId() < 0 || station.getName().empty() ||
        station.getType().empty() ||
        !std::isfinite(station.getX()) || !std::isfinite(station.getY()) ||
        !std::isfinite(station.getSupplyMW()) ||
        !std::isfinite(station.getDemandMW()) ||
        station.getSupplyMW() < 0.0 || station.getDemandMW() < 0.0) {
        return false;
    }

    if (station.getType() != "generator" &&
        station.getType() != "load" &&
        station.getType() != "both") {
        return false;
    }

    if (station.getType() == "generator" && station.getSupplyMW() <= 0.0)
        return false;
    if (station.getType() == "load" && station.getDemandMW() <= 0.0)
        return false;
    if (station.getType() == "both" &&
        station.getSupplyMW() <= 0.0 && station.getDemandMW() <= 0.0)
        return false;

    if (hasSubstationId(station.getId())) return false;

    substations.push_back(station);
    return true;
}

bool Graph::addLine(const TransmissionLine& line) {
    if (line.getId() < 0 || line.getFrom() == line.getTo() ||
        !std::isfinite(line.getDistance()) ||
        !std::isfinite(line.getTerrainFactor()) ||
        !std::isfinite(line.getCapacityMW()) ||
        !std::isfinite(line.getConstructionCost()) ||
        line.getDistance() <= 0.0 ||
        line.getTerrainFactor() <= 0.0 ||
        line.getCapacityMW() <= 0.0 ||
        line.getConstructionCost() <= 0.0) {
        return false;
    }

    if (!hasSubstationId(line.getFrom()) || !hasSubstationId(line.getTo()))
        return false;

    for (const auto& existing : lines) {
        if (existing.getId() == line.getId()) return false;

        // This project currently treats the grid as a simple undirected graph:
        // one line per unordered pair of substations.
        const bool samePair =
            (existing.getFrom() == line.getFrom() &&
             existing.getTo() == line.getTo()) ||
            (existing.getFrom() == line.getTo() &&
             existing.getTo() == line.getFrom());
        if (samePair) return false;
    }

    lines.push_back(line);
    return true;
}

void Graph::display() const {
    std::cout << "\\n--- Substations ---\\n";
    for (const auto& station : substations) station.display();

    std::cout << "\\n--- Transmission Lines ---\\n";
    for (const auto& line : lines) line.display();

    std::cout << "\\nSubstations: " << vertexCount()
              << "\\nLines: " << edgeCount() << '\n';
}

int Graph::vertexCount() const {
    return static_cast<int>(substations.size());
}

int Graph::edgeCount() const {
    return static_cast<int>(lines.size());
}

const std::vector<Substation>& Graph::getSubstations() const {
    return substations;
}

const std::vector<TransmissionLine>& Graph::getLines() const {
    return lines;
}

bool Graph::getSubstationIndex(int substationId, int& index) const {
    for (std::size_t i = 0; i < substations.size(); ++i) {
        if (substations[i].getId() == substationId) {
            index = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

const Substation* Graph::findSubstation(int substationId) const {
    for (const auto& station : substations) {
        if (station.getId() == substationId) return &station;
    }
    return nullptr;
}

// Text format: counts, then quoted station names and line attributes.
// IDs are retained; algorithms should use getSubstationIndex() when they
// need zero-based internal indices.
bool Graph::saveForAlgorithms(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return false;

    file << std::setprecision(17);
    file << substations.size() << '\n';
    for (const auto& station : substations) {
        file << station.getId() << ' '
             << std::quoted(station.getName()) << ' '
             << station.getX() << ' ' << station.getY() << ' '
             << station.getType() << ' '
             << station.getSupplyMW() << ' '
             << station.getDemandMW() << '\n';
    }

    file << lines.size() << '\n';
    for (const auto& line : lines) {
        file << line.getId() << ' '
             << line.getFrom() << ' '
             << line.getTo() << ' '
             << line.getDistance() << ' '
             << line.getTerrainFactor() << ' '
             << line.getCapacityMW() << ' '
             << line.getConstructionCost() << '\n';
    }

    return file.good();
}
