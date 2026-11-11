#include "Substation.h"
#include <iostream>

Substation::Substation(int componentId, const std::string& stationName,
                       double loadDemand)
    : GridComponent(componentId),
      name(stationName), x(0.0), y(0.0),
      type("load"), supplyMW(0.0), demandMW(loadDemand) {}

Substation::Substation(int componentId, const std::string& stationName,
                       double posX, double posY, const std::string& stationType,
                       double supply, double demand)
    : GridComponent(componentId),
      name(stationName), x(posX), y(posY), type(stationType),
      supplyMW(supply), demandMW(demand) {}

const std::string& Substation::getName() const { return name; }
double Substation::getX() const { return x; }
double Substation::getY() const { return y; }
const std::string& Substation::getType() const { return type; }
double Substation::getSupplyMW() const { return supplyMW; }
double Substation::getDemandMW() const { return demandMW; }
double Substation::getLoadDemand() const { return demandMW; }

void Substation::display() const {
    std::cout << "Substation ID: " << id
              << ", Name: " << name
              << ", Type: " << type
              << ", Position: (" << x << ", " << y << ")"
              << ", Supply: " << supplyMW << " MW"
              << ", Demand: " << demandMW << " MW\\n";
}
