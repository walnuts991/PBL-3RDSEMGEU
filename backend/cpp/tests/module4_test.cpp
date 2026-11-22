#include "ShortestPath.h"
#include "MaxFlow.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main() {
    Graph g;
    assert(g.addSubstation(Substation(10,"Generator",0,0,"generator",100,0)));
    assert(g.addSubstation(Substation(25,"Relay",1,0,"generator",1,0)));
    assert(g.addSubstation(Substation(40,"Load",2,0,"load",0,80)));
    assert(g.addLine(TransmissionLine(1,10,25,1.0,1.0,60.0)));
    assert(g.addLine(TransmissionLine(2,25,40,1.0,1.0,50.0)));
    auto sp=ShortestPath::dijkstra(g,10,40);
    assert(sp.pathFound);
    assert(std::abs(sp.totalDistance-2.0)<1e-9);
    assert((sp.path==std::vector<int>{10,25,40}));
    auto flow = MaxFlow::edmondsKarp(g);
    assert(std::abs(flow.maximumFlowMW - 50.0) < 1e-9);
    std::cout << "Module 4 tests passed.\n";
}
