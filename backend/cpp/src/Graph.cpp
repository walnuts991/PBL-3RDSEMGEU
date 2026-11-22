#include "Graph.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
bool Graph::hasSubstationId(int id) const {
    for (const auto& s : substations) if (s.getId() == id) return true;
    return false;
}
bool Graph::addSubstation(const Substation& s) {
    if (s.getId() < 0 || s.getName().empty() ||
        !std::isfinite(s.getX()) || !std::isfinite(s.getY()) ||
        !std::isfinite(s.getSupplyMW()) || !std::isfinite(s.getDemandMW()) ||
        s.getSupplyMW() < 0 || s.getDemandMW() < 0) return false;
    const auto& t = s.getType();
    if (t != "generator" && t != "load" && t != "both") return false;
    if ((t == "generator" && s.getSupplyMW() <= 0) ||
        (t == "load" && s.getDemandMW() <= 0) ||
        (t == "both" && s.getSupplyMW() <= 0 && s.getDemandMW() <= 0)) return false;
    if (hasSubstationId(s.getId())) return false;
    substations.push_back(s); return true;
}
bool Graph::addLine(const TransmissionLine& l) {
    if (l.getId() < 0 || l.getFrom() == l.getTo() ||
        !hasSubstationId(l.getFrom()) || !hasSubstationId(l.getTo()) ||
        !std::isfinite(l.getDistance()) || !std::isfinite(l.getTerrainFactor()) ||
        !std::isfinite(l.getCapacityMW()) || l.getDistance() <= 0 ||
        l.getTerrainFactor() <= 0 || l.getCapacityMW() <= 0) return false;
    for (const auto& e : lines) {
        if (e.getId() == l.getId()) return false;
        if ((e.getFrom() == l.getFrom() && e.getTo() == l.getTo()) ||
            (e.getFrom() == l.getTo() && e.getTo() == l.getFrom())) return false;
    }
    lines.push_back(l); return true;
}
void Graph::display() const {
    std::cout << "\n--- Substations ---\n";
    for (const auto& s : substations) s.display();
    std::cout << "\n--- Transmission Lines ---\n";
    for (const auto& l : lines) l.display();
    std::cout << "Substations: " << vertexCount() << "\nLines: " << edgeCount() << '\n';
}
int Graph::vertexCount() const { return static_cast<int>(substations.size()); }
int Graph::edgeCount() const { return static_cast<int>(lines.size()); }
const std::vector<Substation>& Graph::getSubstations() const { return substations; }
const std::vector<TransmissionLine>& Graph::getLines() const { return lines; }
bool Graph::getSubstationIndex(int id, int& index) const {
    for (int i=0; i<vertexCount(); ++i) if (substations[i].getId()==id) { index=i; return true; }
    return false;
}
const Substation* Graph::findSubstation(int id) const {
    for (const auto& s : substations) if (s.getId()==id) return &s;
    return nullptr;
}
bool Graph::saveForAlgorithms(const std::string& filename) const {
    std::ofstream f(filename); if (!f) return false;
    f << std::setprecision(17) << substations.size() << '\n';
    for (const auto& s : substations)
        f << s.getId() << ' ' << std::quoted(s.getName()) << ' ' << s.getX() << ' '
          << s.getY() << ' ' << s.getType() << ' ' << s.getSupplyMW() << ' ' << s.getDemandMW() << '\n';
    f << lines.size() << '\n';
    for (const auto& l : lines)
        f << l.getId() << ' ' << l.getFrom() << ' ' << l.getTo() << ' '
          << l.getDistance() << ' ' << l.getTerrainFactor() << ' '
          << l.getCapacityMW() << ' ' << l.getConstructionCost() << '\n';
    return f.good();
}
