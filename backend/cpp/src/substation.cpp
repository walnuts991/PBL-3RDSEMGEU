
#include "Substation.h"
#include <iostream>
using namespace std;

Substation::Substation(
    int id, const string& name, double loadDemand
) : Substation(id, name, 0.0, 0.0, "", 0.0, loadDemand) {}

Substation::Substation(
    int id,
    const string& name,
    double x,
    double y,
    const string& type,
    double generationMW,
    double loadDemand
) : GridComponent(id),
    name(name),
    x(x),
    y(y),
    type(type),
    generationMW(generationMW),
    loadDemand(loadDemand) {}

string Substation::getName() const {
    return name;
}

double Substation::getGenerationMW() const {
    return generationMW;
}

double Substation::getLoadDemand() const {
    return loadDemand;
}

void Substation::display() const {
    cout << "Substation ID: " << id
         << ", Name: " << name
         << ", Load: " << loadDemand << '\n';
}
