
#ifndef GRAPH_H
#define GRAPH_H

#include "Substation.h"
#include "TransmissionLine.h"
#include <vector>
using namespace std;

class Graph {
private:
    vector<Substation> substations;
    vector<TransmissionLine> lines;

public:
    bool addSubstation(const Substation& substation);
    bool addLine(const TransmissionLine& line);

    void display() const;
    bool saveForAlgorithms(const string& filename) const;

    int vertexCount() const;
    int edgeCount() const;
};

#endif
