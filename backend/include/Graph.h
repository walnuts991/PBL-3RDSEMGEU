
#ifndef GRIDGUARD_GRAPH_H
#define GRIDGUARD_GRAPH_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;

// Represents a power-grid substation.
struct Substation {
    int id;
    string name;
    double loadDemand;
};

// Represents a transmission line between two substations.
struct TransmissionLine {
    int id;
    int from;
    int to;
    double distance;
    double terrainFactor;
    double capacity;
    double constructionCost;
};

class Graph {
private:
    vector<Substation> substations;
    vector<TransmissionLine> lines;

    // Maps substation ID to its index in the vector.
    unordered_map<int, int> indexById;

    // Adjacency list:
    // substation ID -> (neighbor ID, transmission-line ID).
    unordered_map<int, vector<pair<int, int>>> adjacency;

public:
    // Add a substation after validating its input.
    bool addSubstation(const Substation& substation);

    // Add a transmission line after validating its endpoints.
    bool addLine(const TransmissionLine& line);

    // Estimate construction cost using distance and terrain.
    double calculateConstructionCost(
        double distance,
        double terrainFactor
    ) const;

    // Graph statistics.
    int vertexCount() const;
    int edgeCount() const;

    // Read-only access to graph data for other modules.
    const vector<Substation>& getSubstations() const;
    const vector<TransmissionLine>& getLines() const;

    const unordered_map<int, vector<pair<int, int>>>&
    getAdjacencyList() const;
};

#endif
