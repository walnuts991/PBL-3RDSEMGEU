#ifndef TRANSMISSION_LINE_H
#define TRANSMISSION_LINE_H

#include "Gridcomponent.h"

class TransmissionLine : public GridComponent {
private:
    int from;
    int to;
    double distance;
    double terrainFactor;
    double capacityMW;
    double constructionCost;

public:
    // Retains the original constructor and cost formula.
    TransmissionLine(int id, int from, int to,
                     double distance, double terrainFactor, double capacity);

    int getFrom() const;
    int getTo() const;
    double getDistance() const;
    double getTerrainFactor() const;
    double getCapacity() const;
    double getCapacityMW() const;
    double getConstructionCost() const;

    void display() const override;
};

#endif
