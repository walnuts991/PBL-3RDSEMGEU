
#ifndef GRID_COMPONENT_H
#define GRID_COMPONENT_H

#include <string>
using namespace std;

class GridComponent {
protected:
    int id;

public:
    explicit GridComponent(int componentId) : id(componentId) {}

    virtual void display() const = 0;

    virtual ~GridComponent() {}

    int getId() const {
        return id;
    }
};

#endif
