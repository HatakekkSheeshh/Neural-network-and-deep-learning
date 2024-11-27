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
#include "graph/Toposort.h"
#include "hash/XMap.h"
//#include "stacknqueue/Queue.h"
//#include "stacknqueue/Stack.h"
//#include "stacknqueue/PriorityQueue.h"
//#include "sorting/DLinkedListSE.h"


//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Directed Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////

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
        if(ver_from == nullptr){
            //ver_from = new AbstractGraph<T>::VertexNode(from,this->vertexEQ, this->vertex2str);
            this->add(from);
            ver_from = this->getVertexNode(from);
            if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException(ver_from->toString());
        }
        else if(ver_to == nullptr){
            this->add(to);
            ver_from = this->getVertexNode(to);
            if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException(ver_from->toString());            
        }    
        ver_from->connect(to,weight);
    }
    void disconnect(T from, T to){
        //TODO
        typename AbstractGraph<T>::VertexNode* ver_from = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode* ver_to = this->getVertexNode(to);  
        if(ver_from == nullptr){
            //ver_from = new AbstractGraph<T>::VertexNode(from,this->vertexEQ, this->vertex2str);
            this->add(from);
            ver_from = this->getVertexNode(from);
            if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException(ver_from->toString());
        }
        else if(ver_to == nullptr){
            this->add(to);
            ver_from = this->getVertexNode(to);
            if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException(ver_from->toString());            
        }     
        typename AbstractGraph<T>::Edge* edge = ver_from->getEdge(ver_to); 
        if(edge == nullptr){
            edge = new typename AbstractGraph<T>::Edge(ver_from,ver_to,0);
            throw EdgeNotFoundException(edge->toString());        
        }
        ver_from->removeTo(ver_to);
    }
    void remove(T vertex){
        //TODO
        typename AbstractGraph<T>::VertexNode* node = this->getVertexNode(vertex);
        if(node == nullptr) throw VertexNotFoundException("Vertex not found");
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
        // add vertices
        for(int i = 0; i < nvertices; i++){
            graph->add(vertices[i]);
        }
        // add edges
        for(int i = 0; i < nedges; i++){
            graph->connect(edges[i].from,edges[i].to,edges[i].weight);
        }
        return graph;
    }
};

#endif /* DGRAPHMODEL_H */

