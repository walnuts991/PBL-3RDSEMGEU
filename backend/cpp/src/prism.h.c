#include <stdio.h>
#include <limits.h>

#define V 5

void primMST(int graph[V][V])
{
    int parent[V];
    int key[V];
    int visited[V];
    int i, j;

    for (i = 0; i < V; i++)
    {
        key[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }

    key[0] = 0;

    for (i = 0; i < V - 1; i++)
    {
        int min = INT_MAX;
        int u = -1;

        for (j = 0; j < V; j++)
        {
            if (visited[j] == 0 && key[j] < min)
            {
                min = key[j];
                u = j;
            }
        }

        if (u == -1)
            return;

        visited[u] = 1;

        for (j = 0; j < V; j++)
        {
            if (graph[u][j] != 0 &&
                visited[j] == 0 &&
                graph[u][j] < key[j])
            {
                parent[j] = u;
                key[j] = graph[u][j];
            }
        }
    }

    int totalCost = 0;

    printf("Prim's Minimum Spanning Tree:\n");
    printf("Edge\tWeight\n");

    for (i = 1; i < V; i++)
    {
        if (parent[i] == -1)
        {
            printf("Graph is disconnected. MST does not exist.\n");
            return;
        }

        printf("%d - %d\t%d\n",
               parent[i], i, graph[i][parent[i]]);

        totalCost += graph[i][parent[i]];
    }

    printf("Total MST Cost: %d\n", totalCost);
}

int main()
{
    int graph[V][V];
}    