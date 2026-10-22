
#include "TransmissionLine.h"
#include <iostream>
using namespace std;

TransmissionLine::TransmissionLine(
    int id, int from, int to,
    double distance, double terrainFactor, double capacity
) : GridComponent(id),
    from(from), to(to),
    distance(distance), terrainFactor(terrainFactor),
    capacity(capacity),
    constructionCost(distance * terrainFactor * 10.0) {}

int TransmissionLine::getFrom() const {
    return from;
}

int TransmissionLine::getTo() const {
    return to;
}

double TransmissionLine::getDistance() const {
    return distance;
}

double TransmissionLine::getCapacity() const {
    return capacity;
}

double TransmissionLine::getConstructionCost() const {
    return constructionCost;
}

void TransmissionLine::display() const {
    cout << "Line ID: " << id
         << ", " << from << " - " << to
         << ", Distance: " << distance
         << ", Cost: " << constructionCost << '\n';
}
