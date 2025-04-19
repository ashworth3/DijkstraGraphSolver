# DijkstraGraphSolver

A C++ implementation of Dijkstra’s algorithm featuring a custom min-heap and dynamic adjacency list. Built as an interactive CLI-based pathfinding tool that reads graphs from file and supports both directed and undirected modes. Allows users to run shortest path queries, print paths and distances, and visualize the adjacency list. Emphasizes efficient heap operations and memory-safe graph traversal through modular design.

---

## Features

- Supports directed and undirected graphs (D, UD)
- Dynamic adjacency list construction
- Custom min-heap for efficient pathfinding
- Prints adjacency lists, path, and path lengths
- Interactive CLI menu

---

## Files

- `main.cpp` – Main source file with full implementation
- `network01.txt` – Sample graph (8 nodes, 14 edges)
- `network02.txt` – Sample graph (8 nodes, 11 edges)

---

## How to Compile

```bash
g++ main.cpp -o dijkstra
```
---

## How to Run
```bash
./dijkstra
```
---

## Example Menu
1. Read graph from file (enter filename, D or UD, + flag)
Enter: network01.txt UD 2
Enter number of vertices: 8

## Author
Created by [@ashworth3](https://github.com/ashworth3)
