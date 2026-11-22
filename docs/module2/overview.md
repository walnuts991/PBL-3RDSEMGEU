\# Module 2: Minimum Spanning Tree (MST)



\## 1. Introduction



A graph is a collection of vertices and edges. Vertices represent different locations, while edges represent connections between them.



In our GridGuard project, vertices represent power stations and edges represent transmission lines connecting those stations.



Each transmission line has a weight or cost. Our goal is to connect all power stations with the minimum possible total cost.



\## 2. What is a Minimum Spanning Tree?



A Minimum Spanning Tree (MST) is a tree that connects all vertices of a connected, undirected, weighted graph with the minimum total edge weight.



\### Important Properties



1\. MST connects all vertices of a graph.

2\. It contains no cycles.

3\. It has exactly V - 1 edges, where V is the number of vertices.

4\. It has the minimum possible total edge weight.

5\. A graph can have more than one MST if multiple edges have equal weights.



\## 3. Why is MST Used in GridGuard?



In a power grid, connecting power stations requires transmission lines.



Building every possible connection can increase the overall cost.



MST helps us:

\- Connect all power stations.

\- Reduce the total connection cost.

\- Avoid unnecessary connections.

\- Design an economical network.



\## 4. Algorithms Used



We use three important concepts in Module 2:



1\. \*\*Disjoint Set Union (DSU):\*\* Helps manage connected components and detect cycles.

2\. \*\*Kruskal's Algorithm:\*\* Selects edges in increasing order of weight.

3\. \*\*Prim's Algorithm:\*\* Builds the MST by expanding from a starting vertex.



\## 5. Conclusion



MST plays an important role in designing a cost-efficient power grid. Kruskal's and Prim's algorithms help find the minimum spanning tree, while DSU supports efficient cycle detection in Kruskal's algorithm.

