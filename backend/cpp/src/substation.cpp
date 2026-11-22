#include "Substation.h"
#include <iostream>
Substation::Substation(int id, const std::string& name, double loadDemand)
    : Substation(id, name, 0.0, 0.0, "load", 0.0, loadDemand) {}
Substation::Substation(int id, const std::string& name, double x, double y,
                       const std::string& type, double supplyMW, double demandMW)
    : GridComponent(id), name(name), x(x), y(y), type(type),
      supplyMW(supplyMW), demandMW(demandMW) {}
const std::string& Substation::getName() const { return name; }
double Substation::getX() const { return x; }
double Substation::getY() const { return y; }
const std::string& Substation::getType() const { return type; }
double Substation::getSupplyMW() const { return supplyMW; }
double Substation::getGenerationMW() const { return supplyMW; }
double Substation::getDemandMW() const { return demandMW; }
double Substation::getLoadDemand() const { return demandMW; }
void Substation::display() const {
    std::cout << "Substation ID: " << id << ", Name: " << name
              << ", Type: " << type << ", Position: (" << x << ", " << y
              << "), Supply: " << supplyMW << " MW, Demand: "
              << demandMW << " MW\n";
}
