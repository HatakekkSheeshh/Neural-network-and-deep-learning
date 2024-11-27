#ifndef UGRAPHALGORITHM_H
#define UGRAPHALGORITHM_H

#include <vector>
#include <algorithm>
#include "graph/UGraphModel.h"

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

    // Edge structure to help with sorting
    struct Edge {
        T src;
        T dest;
        double weight;

        Edge(const T& s, const T& d, double w) : src(s), dest(d), weight(w) {}

        bool operator<(const Edge& other) const {
            return weight < other.weight;
        }
    };

public:
    UGraphModel<T> minSpanningTree(UGraphModel<T>* graph) {
        // Create a new graph for the MST
        UGraphModel<T> mst(graph->getEqualFunc(), graph->getVertex2StrFunc());

        // Get all vertices and add them to the MST
        std::vector<T> vertices = graph->getVertices();
        for (const T& vertex : vertices) {
            mst.add(vertex);
        }

        // Collect all edges
        std::vector<Edge> edges;
        for (const T& src : vertices) {
            std::vector<T> neighbors = graph->getNeighbors(src);
            for (const T& dest : neighbors) {
                double weight = graph->getWeight(src, dest);
                // Only add each edge once to avoid duplicates
                if (src < dest) {
                    edges.emplace_back(src, dest, weight);
                }
            }
        }

        // Sort edges by weight
        std::sort(edges.begin(), edges.end());

        // Disjoint set for cycle detection
        DisjointSet ds;
        for (const T& vertex : vertices) {
            ds.makeSet(vertex);
        }

        // Kruskal's algorithm
        for (const Edge& edge : edges) {
            T root1 = ds.find(edge.src);
            T root2 = ds.find(edge.dest);

            // If including this edge doesn't create a cycle
            if (root1 != root2) {
                mst.connect(edge.src, edge.dest, edge.weight);
                ds.unionSets(edge.src, edge.dest);
            }
        }

        return mst;
    }
};

#endif // UGRAPHALGORITHM_H