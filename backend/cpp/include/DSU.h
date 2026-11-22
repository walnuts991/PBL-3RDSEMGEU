
#ifndef DSU_H
#define DSU_H

#include <vector>
// Manages disjoint sets of vertices.
class DSU {
private:
    std::vector<int> parent;
    std::vector<int> rankValue;
    int componentCount;

public:
    explicit DSU(int n);

    int find(int vertex);
    bool unite(int a, int b);
    bool connected(int a, int b);
    int getComponentCount() const;
};

#endif
