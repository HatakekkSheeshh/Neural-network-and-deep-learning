#ifndef UGRAPHALGORITHM_H
#define UGRAPHALGORITHM_H

#include <vector>
#include <algorithm>
#include "graph/UGraphModel.h"
#include "list/DLinkedList.h"

// Correct forward declaration of DGraphModel as a template class
template <class T>
class UGraphModel;

template <typename T>
class UGraphAlgorithm {
private:
    // Disjoint Set data structure for Kruskal's algorithm
    class DisjointSet {
    private:
        std::unordered_map<T, T> parent;
        std::unordered_map<T, int> rank;

    public:
        void makeSet(const T& vertex) {
            if (parent.find(vertex) == parent.end()) {
                parent[vertex] = vertex;
                rank[vertex] = 0;
            }
        }

        T find(const T& vertex) {
            if (parent[vertex] != vertex) {
                parent[vertex] = find(parent[vertex]);
            }
            return parent[vertex];
        }

        void unionSets(const T& x, const T& y) {
            T rootX = find(x);
            T rootY = find(y);

            if (rootX == rootY) return;

            if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            }
            else if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            }
            else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    };

    // Modify Edge to be a template class
    template <typename U>
    struct Edge {
        U src;
        U dest;
        double weight;

        Edge(const U& s, const U& d, double w) : src(s), dest(d), weight(w) {}

        bool operator<(const Edge& other) const {
            return weight < other.weight;
        }
    };

public:
    static std::vector<T> getVertices(UGraphModel<T>* graph) {
        std::vector<T> vertexList;
        
        DLinkedList<T> vertices = graph->vertices();
        
        for (auto it = vertices.begin(); it != vertices.end(); ++it) {
            vertexList.push_back(*it);
        }
        
        return vertexList;
    }

    static std::vector<T> getNeighbors(UGraphModel<T>* graph, T vertex) {
        std::vector<T> neighbors;
        
        DLinkedList<T> neighborList = graph->getOutwardEdges(vertex);
        
        for (auto it = neighborList.begin(); it != neighborList.end(); ++it) {
            neighbors.push_back(*it);
        }
        
        return neighbors;
    }
    static double getWeight(UGraphModel<T>* graph, T from, T to) {
        return graph->weight(from, to);
    }

    static UGraphModel<T> minSpanningTree(UGraphModel<T>* graph) {
        // Create a new graph for the Minimum Spanning Tree
        UGraphModel<T> mst(graph->getVertexEQ(), graph->getVertex2Str());

        // Add all vertices to the MST
        std::vector<T> vertices = getVertices(graph);
        for (const T& vertex : vertices) {
            mst.add(vertex);
        }

        // Collect all edges from the original graph
        std::vector<Edge<T>> edges;
        for (const T& src : vertices) {
            std::vector<T> neighbors = getNeighbors(graph, src);
            for (const T& dest : neighbors) {
                // Get the weight of the edge
                double weight = getWeight(graph, src, dest);
                if (src < dest) {
                    edges.emplace_back(src, dest, weight);
                }
            }
        }

        // Sort edges by weight
        std::sort(edges.begin(), edges.end());

        // Implement Kruskal's algorithm
        DisjointSet ds;
        for (const T& vertex : vertices) {
            ds.makeSet(vertex);
        }

        for (const auto& edge : edges) {
            T rootSrc = ds.find(edge.src);
            T rootDest = ds.find(edge.dest);

            if (rootSrc != rootDest) {
                mst.connect(edge.src, edge.dest, edge.weight);
                ds.unionSets(rootSrc, rootDest);
            }
        }

        return mst;
    }
};

#endif // UGRAPHALGORITHM_H