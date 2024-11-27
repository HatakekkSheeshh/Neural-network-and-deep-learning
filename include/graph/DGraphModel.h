/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DGraphModel.h
 * Author: LTSACH
 *
 * Created on 23 August 2020, 19:36
 */

#ifndef DGRAPHMODEL_H
#define DGRAPHMODEL_H
#include "graph/AbstractGraph.h"
#include "graph/IGraph.h"
#include "graph/DGraphAlgorithm.h"
//#include "stacknqueue/Queue.h"
//#include "stacknqueue/Stack.h"
#include "hash/XMap.h"
//#include "stacknqueue/PriorityQueue.h"
//#include "sorting/DLinkedListSE.h"


//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Directed Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////
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

template<class T>
class DGraphModel: public AbstractGraph<T>{
private:
public:
    DGraphModel(
            bool (*vertexEQ)(T&, T&), 
            string (*vertex2str)(T&) ): 
        AbstractGraph<T>(vertexEQ, vertex2str){
    }
    
    void connect(T from, T to, float weight=0){
        //TODO
        typename AbstractGraph<T>::VertexNode* ver_from = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode* ver_to = this->getVertexNode(to);
        if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException("Not found");
        ver_from->connect(to,weight);
    }
    void disconnect(T from, T to){
        //TODO
        typename AbstractGraph<T>::VertexNode* ver_from = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode* ver_to = this->getVertexNode(to);
        if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException("Not found");
        ver_from->removeTo(ver_to);
    }
    void remove(T vertex){
        //TODO
        typename AbstractGraph<T>::VertexNode* node = this->getVertexNode(vertex);
        if(node == nullptr) throw VertexNotFoundException("Not found");
        typename DLinkedList<typename AbstractGraph<T>::VertexNode*>::Iterator it = this->nodeList.begin();
        while (it != this->nodeList.end()) {
            (*it)->removeTo(node);
            ++it;
        }        
        
        // Create a custom free function for VertexNode
        auto vertexNodeFree = [](typename AbstractGraph<T>::VertexNode* nodePtr) {
            delete nodePtr;
        };
        
        // Remove the node from the list
        this->nodeList.removeItem(node, vertexNodeFree);
    }
    
    static DGraphModel<T>* create(
            T* vertices, int nvertices, Edge<T>* edges, int nedges,
            bool (*vertexEQ)(T&, T&),
            string (*vertex2str)(T&)){
        //TODO
        DGraphModel<T>* graph = new DGraphModel<T>(vertexEQ, vertex2str);
        // add vertex
        for(int i = 0; i < nvertices; i++){
            graph->add(vertices[i]);
        }
        // add edge
        for(int i = 0; i < nvertices; i++){
            graph->connect(edges[i].from,edges[i].to,edges[i].weight);
        }
        return graph;
    }
};

#endif /* DGRAPHMODEL_H */

