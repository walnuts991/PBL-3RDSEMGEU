#include <stdio.h>

#define V 5

int parent[V];
int rankArr[V];

void makeSet()
{
    int i;

    for (i = 0; i < V; i++)
    {
        parent[i] = i;
        rankArr[i] = 0;
    }
}

int find(int x)
{
    if (parent[x] != x)
    {
        parent[x] = find(parent[x]);
    }

    return parent[x];
}

void unionSet(int x, int y)
{
    int rootX = find(x);
    int rootY = find(y);

    if (rootX == rootY)
    {
        return;
    }

    if (rankArr[rootX] < rankArr[rootY])
    {
        parent[rootX] = rootY;
    }
    else if (rankArr[rootX] > rankArr[rootY])
    {
        parent[rootY] = rootX;
    }
    else
    {
        parent[rootY] = rootX;
        rankArr[rootX]++;
    }
}

int connected(int x, int y)
{
    return find(x) == find(y);
}

int main()
{
    makeSet();

    unionSet(0, 1);
    unionSet(1, 2);
    unionSet(3, 4);

    printf("Are 0 and 2 connected? ");

    if (connected(0, 2))
        printf("Yes\n");
    else
        printf("No\n");

    printf("Are 0 and 4 connected? ");

    if (connected(0, 4))
        printf("Yes\n");
    else
        printf("No\n");

    unionSet(2, 4);

    printf("After joining the sets:\n");

    printf("Are 0 and 4 connected? ");

    if (connected(0, 4))
        printf("Yes\n");
    else
        printf("No\n");

    return 0;
}

