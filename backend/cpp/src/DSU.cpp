
#include "DSU.h"
#include <numeric>
#include <stdexcept>

DSU::DSU(int n)
    : parent(n < 0 ? 0 : n),
      rankValue(n < 0 ? 0 : n, 0),
      componentCount(n < 0 ? 0 : n) {

    if (n < 0) {
        throw std::invalid_argument("DSU size cannot be negative");
    }

    // Initially, every vertex is its own parent.
    std::iota(parent.begin(), parent.end(), 0);
}

int DSU::find(int vertex) {

    if (vertex < 0 || vertex >= static_cast<int>(parent.size())) {
        throw std::out_of_range("DSU vertex index out of range");
    }

    // Path compression.
    if (parent[vertex] != vertex) {
        parent[vertex] = find(parent[vertex]);
    }

    return parent[vertex];
}

bool DSU::unite(int a, int b) {

    int rootA = find(a);
    int rootB = find(b);

    // Already in the same set: joining them creates a cycle.
    if (rootA == rootB) {
        return false;
    }

    // Union by rank.
    if (rankValue[rootA] < rankValue[rootB]) {
        parent[rootA] = rootB;
    }
    else if (rankValue[rootA] > rankValue[rootB]) {
        parent[rootB] = rootA;
    }
    else {
        parent[rootB] = rootA;
        rankValue[rootA]++;
    }

    componentCount--;
    return true;
}

bool DSU::connected(int a, int b) {
    return find(a) == find(b);
}

int DSU::getComponentCount() const {
    return componentCount;
}
