#ifndef TOPOSORT_H
#define TOPOSORT_H
#include "graph/AbstractGraph.h"
#include "graph/IGraph.h"
#include "graph/DGraphAlgorithm.h"
template<class T>
class TopoSorter {
public:
    enum SortMethod { BFS, DFS };

    TopoSorter(DGraphModel<T>* graph) : graph(graph) {}

    DLinkedList<T> sort(SortMethod method) {
        switch(method) {
            case BFS: return bfsTopologicalSort();
            case DFS: return dfsTopologicalSort();
            default: throw runtime_error("Invalid sorting method");
        }
    }

private:
    DGraphModel<T>* graph;

    DLinkedList<T> bfsTopologicalSort() {
        // Kahn's algorithm for topological sorting
        DLinkedList<T> result;
        DLinkedList<T> vertices = graph->vertices();
        unordered_map<T, int> inDegrees;

        // Calculate in-degrees
        for (auto v : vertices) {
            inDegrees[v] = graph->inDegree(v);
        }

        queue<T> zeroInDegree;
        for (auto v : vertices) {
            if (inDegrees[v] == 0) {
                zeroInDegree.push(v);
            }
        }

        while (!zeroInDegree.empty()) {
            T current = zeroInDegree.front();
            zeroInDegree.pop();
            result.add(current);

            for (auto neighbor : graph->getOutwardEdges(current)) {
                inDegrees[neighbor]--;
                if (inDegrees[neighbor] == 0) {
                    zeroInDegree.push(neighbor);
                }
            }
        }

        return result;
    }

    DLinkedList<T> dfsTopologicalSort() {
        // DFS-based topological sorting
        DLinkedList<T> result;
        unordered_set<T> visited;
        unordered_set<T> recursionStack;

        for (auto v : graph->vertices()) {
            if (!visited.count(v)) {
                dfsVisit(v, visited, recursionStack, result);
            }
        }

        // Reverse the list to get correct order using BWDIterator
        DLinkedList<T> reversedResult;
        for (typename DLinkedList<T>::BWDIterator it = result.bbegin(); 
             it != result.bend(); ++it) {
            reversedResult.add(*it);
        }

        return reversedResult;
    }

    void dfsVisit(T vertex, 
                  unordered_set<T>& visited, 
                  unordered_set<T>& recursionStack, 
                  DLinkedList<T>& result) {
        visited.insert(vertex);
        recursionStack.insert(vertex);

        for (auto neighbor : graph->getOutwardEdges(vertex)) {
            if (!visited.count(neighbor)) {
                dfsVisit(neighbor, visited, recursionStack, result);
            }
        }

        recursionStack.erase(vertex);
        result.add(vertex);
    }
};
#endif /*TOPOSORT_H*/