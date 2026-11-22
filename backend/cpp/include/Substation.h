#ifndef SUBSTATION_H
#define SUBSTATION_H
#include "Gridcomponent.h"
#include <string>
class Substation : public GridComponent {
private:
    std::string name;
    double x, y;
    std::string type;
    double supplyMW, demandMW;
public:
    Substation(int id, const std::string& name, double loadDemand);
    Substation(int id, const std::string& name, double x, double y,
               const std::string& type, double supplyMW, double demandMW);
    const std::string& getName() const;
    double getX() const;
    double getY() const;
    const std::string& getType() const;
    double getSupplyMW() const;
    double getGenerationMW() const;
    double getDemandMW() const;
    double getLoadDemand() const;
    void display() const override;
};
#endif
