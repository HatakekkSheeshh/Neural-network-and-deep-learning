/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   UGraphModel.h
 * Author: LTSACH
 *
 * Created on 24 August 2020, 15:16
 */

#ifndef UGRAPHMODEL_H
#define UGRAPHMODEL_H

#include "graph/AbstractGraph.h"
#include "graph/UGraphAlgorithm.h"
//#include "stacknqueue/PriorityQueue.h"

//////////////////////////////////////////////////////////////////////
///////////// UGraphModel: Undirected Graph Model ////////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
class UGraphModel : public AbstractGraph<T>
{
private:
public:
    // class UGraphAlgorithm;
    // friend class UGraphAlgorithm;

    UGraphModel(
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &)) : AbstractGraph<T>(vertexEQ, vertex2str){
    }

    void connect(T from, T to, float weight = 0)
    {
        // TODO
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
        if(from == to){
            ver_from->connect(from,weight);
        }
        else{
            ver_from->connect(to,weight);
            ver_to->connect(from,weight);
        }
    }
    void disconnect(T from, T to)
    {
        // TODO
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
        if(from == to){
            ver_from->removeTo(ver_from);
        }
        else{
            ver_from->removeTo(ver_to);
            ver_to->removeTo(ver_from);
        }
    }
    void remove(T vertex)
    {
        // TODO
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
    static UGraphModel<T> *create(
        T *vertices, int nvertices, Edge<T> *edges, int nedges,
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &))
    {
        // TODO
        UGraphModel<T>* graph = new UGraphModel<T>(vertexEQ, vertex2str);   
        // add vertices     
        for(int i = 0; i < nvertices; i++){
            graph->add(vertices[i]);        for(int i = 0; i < nvertices; i++){
            graph->connect(edges[i].from,edges[i].to,edges[i].weight);
        }
        return graph;
        }
        // add edges
        for(int i = 0; i < nedges; i++){
            graph->connect(edges[i].from,edges[i].to,edges[i].weight);
        }
        return graph;
    }
};

#endif /* UGRAPHMODEL_H */
