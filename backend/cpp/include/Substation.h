
#ifndef SUBSTATION_H
#define SUBSTATION_H

#include "GridComponent.h"

class Substation : public GridComponent {
private:
    string name;
    double loadDemand;

public:
    Substation(int id, const string& name, double loadDemand);

    string getName() const;
    double getLoadDemand() const;

    void display() const override;
};

#endif

#ifndef SUBSTATION_H
#define SUBSTATION_H

#include "GridComponent.h"

class Substation : public GridComponent {
private:
    string name;
    double loadDemand;

public:
    Substation(int id, const string& name, double loadDemand);

    string getName() const;
    double getLoadDemand() const;

    void display() const override;
};

#endif
