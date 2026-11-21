#include "ShortestPath.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>

ShortestPathResult ShortestPath::dijkstra(
    const Graph& graph,
    int sourceId,
    int destinationId
) {
    int n = graph.vertexCount();

    int source = -1;
    int destination = -1;

    if (!graph.getSubstationIndex(sourceId, source) ||
        !graph.getSubstationIndex(destinationId, destination)) {
        throw std::invalid_argument(
            "Source or destination substation does not exist"
        );
    }

    struct Adjacent {
        int vertex;
        double distance;
    };

    std::vector<std::vector<Adjacent>> adjacency(n);

    // Build an undirected weighted adjacency list.
    for (const auto& line : graph.getLines()) {
        int u = -1;
        int v = -1;

        if (!graph.getSubstationIndex(line.getFrom(), u) ||
            !graph.getSubstationIndex(line.getTo(), v)) {
            throw std::invalid_argument(
                "Transmission line has an invalid endpoint"
            );
        }

        double weight = line.getDistance();

        if (weight < 0.0) {
            throw std::invalid_argument(
                "Dijkstra requires non-negative distances"
            );
        }

        adjacency[u].push_back({v, weight});
        adjacency[v].push_back({u, weight});
    }

    const double INF = std::numeric_limits<double>::infinity();

    std::vector<double> distance(n, INF);
    std::vector<int> parent(n, -1);

    using QueueItem = std::pair<double, int>;

    // Min-priority queue: smallest distance is processed first.
    std::priority_queue<
        QueueItem,
        std::vector<QueueItem>,
        std::greater<QueueItem>
    > pq;

    distance[source] = 0.0;
    pq.push({0.0, source});

    while (!pq.empty()) {
        double currentDistance = pq.top().first;
        int current = pq.top().second;
        pq.pop();

        // Ignore outdated queue entries.
        if (currentDistance > distance[current]) {
            continue;
        }

        if (current == destination) {
            break;
        }

        for (const auto& edge : adjacency[current]) {
            double newDistance =
                currentDistance + edge.distance;

            if (newDistance < distance[edge.vertex]) {
                distance[edge.vertex] = newDistance;
                parent[edge.vertex] = current;

                pq.push({newDistance, edge.vertex});
            }
        }
    }

    ShortestPathResult result;

    if (distance[destination] == INF) {
        return result;
    }

    result.pathFound = true;
    result.totalDistance = distance[destination];

    // Reconstruct the route using parent pointers.
    std::vector<int> reversedPath;

    for (int current = destination;
         current != -1;
         current = parent[current]) {

        reversedPath.push_back(
            graph.getSubstations()[current].getId()
        );
    }

    std::reverse(reversedPath.begin(), reversedPath.end());

    result.path = std::move(reversedPath);

    return result;
}