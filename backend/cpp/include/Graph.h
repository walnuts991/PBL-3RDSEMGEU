#ifndef GRAPH_H
#define GRAPH_H

#include "Substation.h"
#include "TransmissionLine.h"
#include <string>
#include <vector>

class Graph {
private:
    std::vector<Substation> substations;
    std::vector<TransmissionLine> lines;

    bool hasSubstationId(int id) const;

public:
    bool addSubstation(const Substation& substation);
    bool addLine(const TransmissionLine& line);

    void display() const;
    bool saveForAlgorithms(const std::string& filename) const;

    int vertexCount() const;
    int edgeCount() const;

    // Read-only access for algorithms; callers cannot modify Graph storage.
    const std::vector<Substation>& getSubstations() const;
    const std::vector<TransmissionLine>& getLines() const;

    // Convert an external substation ID to its zero-based vector index.
    bool getSubstationIndex(int substationId, int& index) const;
    const Substation* findSubstation(int substationId) const;
};

#endif
