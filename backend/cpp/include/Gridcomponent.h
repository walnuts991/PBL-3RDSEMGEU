#ifndef GRID_COMPONENT_H
#define GRID_COMPONENT_H

#include <string>

class GridComponent {
protected:
    int id;

public:
    explicit GridComponent(int componentId) : id(componentId) {}
    virtual void display() const = 0;
    virtual ~GridComponent() = default;

    int getId() const { return id; }
};

#endif
