
#include "Substation.h"
#include <iostream>
using namespace std;

Substation::Substation(
    int id, const string& name, double loadDemand
) : GridComponent(id), name(name), loadDemand(loadDemand) {}

string Substation::getName() const {
    return name;
}

double Substation::getLoadDemand() const {
    return loadDemand;
}

void Substation::display() const {
    cout << "Substation ID: " << id
         << ", Name: " << name
         << ", Load: " << loadDemand << '\n';
}
