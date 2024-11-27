#ifndef DGRAPH_ALGORITHM_H
#define DGRAPH_ALGORITHM_H

#include <limits>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <string>

// Correct forward declaration of DGraphModel as a template class
template <class T>
class DGraphModel;

#include "graph/DGraphModel.h"
#include "list/DLinkedList.h"

template <class T>
class Path {
public:
    T start;
    T end;
    double totalCost;
    DLinkedList<T> vertices;

    Path(T start, T end) : start(start), end(end), totalCost(std::numeric_limits<double>::infinity()) {
        vertices.add(start);
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "(" << start << "->" << end << ": " << totalCost << ") ";
        ss << "Path: ";
        // Use const_cast to allow iteration over const list
        auto& nonConstVertices = const_cast<DLinkedList<T>&>(vertices);
        for (auto it = nonConstVertices.begin(); it != nonConstVertices.end(); ++it) {
            ss << *it << " ";
        }
        return ss.str();
    }
};

template <class T>
class DGraphModel;

template <class T>
class DGraphAlgorithm {
public:
    DLinkedList<Path<T>*> dijkstra(DGraphModel<T>* graph, T startVertex) {
        DLinkedList<Path<T>*> paths;
        std::unordered_map<T, Path<T>*> pathMap;

        // Initialize paths for all vertices
        DLinkedList<T> vertices = graph->vertices();
        for (auto vertex : vertices) {
            Path<T>* path = new Path<T>(startVertex, vertex);
            paths.add(path);
            pathMap[vertex] = path;
        }

        // Set start vertex path cost to 0
        pathMap[startVertex]->totalCost = 0;

        // Priority queue to get minimum cost vertex
        auto comparator = [&pathMap](const T& a, const T& b) {
            return pathMap[a]->totalCost > pathMap[b]->totalCost;
        };
        std::priority_queue<T, std::vector<T>, decltype(comparator)> pq(comparator);
        pq.push(startVertex);

        while (!pq.empty()) {
            T current = pq.top();
            pq.pop();

            // Get outgoing edges from current vertex
            DLinkedList<T> edges = graph->getOutwardEdges(current);
            for (auto neighbor : edges) {
                double edgeCost = graph->weight(current, neighbor);
                double newCost = pathMap[current]->totalCost + edgeCost;

                // Update path if new cost is less
                if (newCost < pathMap[neighbor]->totalCost) {
                    pathMap[neighbor]->totalCost = newCost;
                    pathMap[neighbor]->vertices.clear();
                    
                    // Copy current path and add new vertex
                    for (auto it = pathMap[current]->vertices.begin(); 
                         it != pathMap[current]->vertices.end(); ++it) {
                        pathMap[neighbor]->vertices.add(*it);
                    }
                    pathMap[neighbor]->vertices.add(neighbor);
                    pathMap[neighbor]->end = neighbor;

                    pq.push(neighbor);
                }
            }
        }

        return paths;
    }
};

#endif // DGRAPH_ALGORITHM_H