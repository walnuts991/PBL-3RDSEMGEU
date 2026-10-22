#include <stdio.h>

#define V 5
#define E 7

struct Edge
{
    int src;
    int dest;
    int weight;
};

int parent[V];

int findParent(int vertex)
{
    if (parent[vertex] == vertex)
        return vertex;

    parent[vertex] = findParent(parent[vertex]);

    return parent[vertex];
}

void unionSet(int u, int v)
{
    int rootU = findParent(u);
    int rootV = findParent(v);

    if (rootU != rootV)
        parent[rootV] = rootU;
}

void sortEdges(struct Edge edges[])
{
    int i, j;
    struct Edge temp;

    for (i = 0; i < E - 1; i++)
    {
        for (j = 0; j < E - i - 1; j++)
        {
            if (edges[j].weight > edges[j + 1].weight)
            {
                temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
}

void kruskalMST(struct Edge edges[])
{
    int i;
}