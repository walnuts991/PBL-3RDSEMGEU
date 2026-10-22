
#ifndef TRANSMISSION_LINE_H
#define TRANSMISSION_LINE_H

#include "GridComponent.h"

class TransmissionLine : public GridComponent {
private:
    int from;
    int to;
    double distance;
    double terrainFactor;
    double capacity;
    double constructionCost;

public:
    TransmissionLine(
        int id, int from, int to,
        double distance, double terrainFactor, double capacity
    );

    int getFrom() const;
    int getTo() const;
    double getDistance() const;
    double getCapacity() const;
    double getConstructionCost() const;

    void display() const override;
};

#endif
