#include "TransmissionLine.h"
#include <iostream>

TransmissionLine::TransmissionLine(int componentId, int source, int destination,
                                   double lineDistance, double terrain,
                                   double capacity)
    : GridComponent(componentId),
      from(source), to(destination), distance(lineDistance),
      terrainFactor(terrain), capacityMW(capacity),
      constructionCost(lineDistance * terrain * 10.0) {}

int TransmissionLine::getFrom() const { return from; }
int TransmissionLine::getTo() const { return to; }
double TransmissionLine::getDistance() const { return distance; }
double TransmissionLine::getTerrainFactor() const { return terrainFactor; }
double TransmissionLine::getCapacity() const { return capacityMW; }
double TransmissionLine::getCapacityMW() const { return capacityMW; }
double TransmissionLine::getConstructionCost() const { return constructionCost; }

void TransmissionLine::display() const {
    std::cout << "Line ID: " << id
              << ", " << from << " - " << to
              << ", Distance: " << distance << " km"
              << ", Terrain factor: " << terrainFactor
              << ", Capacity: " << capacityMW << " MW"
              << ", Construction cost: " << constructionCost << '\n';
}
