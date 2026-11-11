#ifndef SUBSTATION_H
#define SUBSTATION_H

#include "Gridcomponent.h"
#include <string>

class Substation : public GridComponent {
private:
    std::string name;
    double x;
    double y;
    std::string type;       // "generator", "load", or "both"
    double supplyMW;
    double demandMW;

public:
    // Backward-compatible constructor used by the original project.
    Substation(int id, const std::string& name, double loadDemand);

    // Extended constructor for the complete GridGuard data model.
    Substation(int id, const std::string& name,
               double x, double y, const std::string& type,
               double supplyMW, double demandMW);

    const std::string& getName() const;
    double getX() const;
    double getY() const;
    const std::string& getType() const;
    double getSupplyMW() const;
    double getDemandMW() const;

    // Backward-compatible name retained for existing code.
    double getLoadDemand() const;

    void display() const override;
};

#endif
