/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AbstractGraph.h
 * Author: LTSACH
 *
 * Created on 23 August 2020, 17:48
 */

#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H
#include "graph/IGraph.h"
#include <string>
#include <sstream>
// Delete before handling in
#include<unordered_map>
#include<queue>
#include<unordered_set>

using namespace std;

template<class T>
class DGraphModel;

template<class T>
class AbstractGraph: public IGraph<T>{
public:
    class Edge; //forward declaration
    class VertexNode; //forward declaration
    class Iterator; //forward declaration
private:
protected:
    //Using the adjacent list technique, so need to store list of nodes (nodeList)
    DLinkedList<VertexNode*> nodeList; 
    
    //Function pointers:
    bool (*vertexEQ)(T&, T&); //to compare two vertices
    string (*vertex2str)(T&); //to obtain string representation of vertices
    
    
    VertexNode* getVertexNode(T& vertex){
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* node = *it;
            if(vertexEQ(node->vertex, vertex) ) return node;
            it++;
        }
        return 0;
    }
    string vertex2Str(VertexNode& node){
        return vertex2str(node.vertex);
    }
    string edge2Str(Edge& edge){
        stringstream os;
        os << "E("
                << vertex2str(edge.from->vertex)
                << ","
                << vertex2str(edge.to->vertex)
                << ")";
        return os.str();
    }
    
public:
    AbstractGraph(
            bool (*vertexEQ)(T&, T&)=0, 
            string (*vertex2str)(T&)=0){
        
        this->vertexEQ = vertexEQ;
        this->vertex2str = vertex2str;
    }
    virtual ~AbstractGraph(){}
    
    typedef bool (*vertexEQFunc)(T&, T&);
    typedef string (*vertex2strFunc)(T&);
    vertexEQFunc getVertexEQ(){
        return this->vertexEQ;
    }
    vertex2strFunc getVertex2Str(){
        return this->vertex2str;
    }

    
//////////////////////////////////////////////////////////////////////
//////////////////// IMPLEMENTATION of IGraph API ////////////////////
//////////////////////////////////////////////////////////////////////
    /* connect, disconnect, and remove: will be overridden in:
     *  >> UGraphModel
     *  >> DGraphModel
     */
    virtual void connect(T from, T to, float weight=0) =0;
    virtual void disconnect(T from, T to)=0;
    virtual void remove(T vertex)=0;
    
    
    
    /* The following are common methods for UGraphModel and DGraphModel
     */
    virtual void add(T vertex) {
        //TODO
        if(contains(vertex)) return;
        VertexNode* newNode = new VertexNode(vertex, this, vertexEQ, vertex2str);
        this->nodeList.add(newNode);
    }
    virtual bool contains(T vertex){
        //TODO
        VertexNode* contains = getVertexNode(vertex);
        if(contains != nullptr) return true;
        return false;
    }
    virtual float weight(T from, T to){
        //TODO
        VertexNode* ver_from = getVertexNode(from);
        VertexNode* ver_to = getVertexNode(to);
        if(ver_from == nullptr || ver_to == nullptr) throw VertexNotFoundException("Vertex not found");
        Edge* edge = ver_from->getEdge(ver_to);
        if(edge == nullptr) throw EdgeNotFoundException("Edge not found");
        return edge->weight;
    }
    virtual DLinkedList<T> getOutwardEdges(T from){
        //TODO
        VertexNode *vertex_from = getVertexNode(from);
        if(vertex_from == nullptr) throw VertexNotFoundException("Vertex not found");
        return vertex_from->getOutwardEdges();
    }
    
    virtual DLinkedList<T> getInwardEdges(T to){
        //TODO
        VertexNode *vertex_in = getVertexNode(to);
        if(vertex_in == nullptr) throw VertexNotFoundException("Vertex not found");
        DLinkedList<T> adj_edges;
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode *adj = *it;
            if(adj->getEdge(vertex_in)){
                adj_edges.add(adj->vertex);
            }
            it++;
        }
        return adj_edges;
    }
    
    virtual int size() {
        //TODO
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        int cnt = 0;
        while(it != nodeList.end()){
            cnt++;
            it++;
        }
        return cnt;
    }
    virtual bool empty(){
        //TODO
        if(nodeList.empty()) return true;
        return false;
    };
    virtual void clear(){
        //TODO
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            delete *it;
            it++;
        }
        nodeList.clear();
    }
    virtual int inDegree(T vertex){
        //TODO
        VertexNode *ver_node = getVertexNode(vertex);
        if(ver_node == nullptr) throw VertexNotFoundException("Vertex not found");
        return ver_node->inDegree();
    }
    virtual int outDegree(T vertex){
        //TODO
        VertexNode *ver_node = getVertexNode(vertex);
        if(ver_node == nullptr) throw VertexNotFoundException("Vertex not found");
        return ver_node->outDegree();
    }
    
    virtual DLinkedList<T> vertices(){
        //TODO
        DLinkedList<T> vertexList;
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* v_keep = *it;
            vertexList.add(v_keep->getVertex());
            it++;
        }
        return vertexList;
    }
    virtual bool connected(T from, T to){
        //TODO
        VertexNode *vertex_from = getVertexNode(from);
        VertexNode *vertex_to = getVertexNode(to);
        if(vertex_from == nullptr ||  vertex_to == nullptr) throw VertexNotFoundException("Vertex not found");
        if(vertex_from->getEdge(vertex_to)) return true;
        return false;
    }
    void println(){
        cout << this->toString() << endl;
    }
    virtual string toString(){
        string mark(50, '=');
        stringstream os;
        os << mark << endl;
        os << left << setw(12) << "Vertices:" << endl;
        typename DLinkedList<VertexNode*>::Iterator nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            os << node->toString() << endl;
            nodeIt++;
        }
        string mark2(30, '-');
        os << mark2 << endl;
        
        os << left << setw(12) << "Edges:" << endl;
        
        nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            
            typename DLinkedList<Edge*>::Iterator edgeIt = node->adList.begin();
            while(edgeIt != node->adList.end()){
                Edge* edge = *edgeIt;
                os << edge->toString() << endl;
                
                edgeIt++;
            }
            
            nodeIt++;
        }
        os << mark << endl;
        return os.str();
    }
    
    /* begin, end: will be used to traverse on graph's vertices
     * example: assume that "graph" is a graph.
     * Code:
     *      AbstractGraph<T>::Iterator it;
     *      for(it = graph.begin(); it != graph.end(); it++){
     *          T vertex = *it; 
     *          //continue to process vertex here!
     *      }
     */
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }

//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
//BEGIN of VertexNode    
    class VertexNode{
    private:
        template<class U>
        friend class DGraphModel;
        template<class U>
        friend class UGraphModel;
        
        T vertex;
        int inDegree_, outDegree_;
        DLinkedList<Edge*> adList; 
        AbstractGraph<T>* graph; 
        friend class Edge;
        friend class AbstractGraph;
        
        bool (*vertexEQ)(T&, T&);
        string (*vertex2str)(T&);
        
    public:
        friend class AbstractGraph<T>;
        VertexNode() : adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ) {}
        VertexNode(T vertex, AbstractGraph<T>* graph, 
                   bool (*vertexEQ)(T&, T&), 
                   string (*vertex2str)(T&))
            : adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ) {
            this->vertex = vertex;
            this->graph = graph;
            this->vertexEQ = vertexEQ;
            this->vertex2str = vertex2str;
            this->outDegree_ = this->inDegree_ = 0;
        }
        T& getVertex(){
            return vertex;
        }
        void connect(T to, float weight=0){
            // Find vertex corresponding to 'to'
            VertexNode* toNode = nullptr;
            
            // Use the graph's nodeList to find the matching vertex
            typename DLinkedList<VertexNode*>::Iterator it = graph->nodeList.begin();
            while(it != graph->nodeList.end()){
                if(graph->vertexEQ((*it)->vertex, to)){
                    toNode = *it;
                    break;
                }
                it++;
            }
            
            // If matching vertex not found, return
            if(toNode == nullptr) return;

            // Check if edge already exists
            typename DLinkedList<Edge*>::Iterator edgeIt = adList.begin();
            while(edgeIt != adList.end()){
                if(graph->vertexEQ((*edgeIt)->to->vertex, to)){
                    (*edgeIt)->weight = weight;
                    return; 
                }
                edgeIt++;
            }
            
            // Create new edge if it doesn't exist
            Edge* edge = new Edge(this, toNode, weight);
            adList.add(edge);
            
            // Update degree
            
            this->outDegree_++;
            toNode->inDegree_++;
        }
        DLinkedList<T> getOutwardEdges(){
            //TODO
            // adList:
            DLinkedList<T> adj_edge;
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                Edge* edge = *it;
                adj_edge.add(edge->to->getVertex());
                it++;
            }
            return adj_edge;
        }

        Edge* getEdge(VertexNode* to){
            //TODO
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if(vertexEQ((*it)->to->vertex, to->vertex)){
                    return *it;
                }
                it++;
            }
            return nullptr;
        }
        bool equals(VertexNode* node){
            //TODO
            if(vertexEQ(this->getVertex() == node)){
                return true;
            }
            return false;
        }
        
        void removeTo(VertexNode* to){
            //TODO
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if((*it)->to == to){
                    Edge* del = *it;
                    adList.removeItem(del, Edge::free);
                    this->outDegree_--;
                    to->inDegree_--;
                    return;
                }
                it++;
            }
        }
        int inDegree(){
            //TODO
            return inDegree_;
        }
        int outDegree(){
            //TODO
            return outDegree_;
        }
        string toString(){
            stringstream os;
            os << "V("
                    << this->vertex << ", "
                    << "in: " << this->inDegree_ << ", "
                    << "out: " << this->outDegree_ << ")";
            return os.str();
        }
    };
//END of VertexNode    
    
//BEGIN of Edge
    class Edge{
    private:
        VertexNode* from;
        VertexNode* to;
        float weight;
        friend class VertexNode;
        friend class AbstractGraph;
        
    public:
        Edge(){}
        Edge(VertexNode* from, VertexNode* to, float weight=0){
            this->from = from;
            this->to = to;
            this->weight = weight;
        }
        
        static void free(Edge* edge) {
            delete edge; // Free the memory allocated for Edge
        }
        
        bool equals(Edge* edge){
            //TODO
            // Compare the vertices and weight of the edges
            if (this->from->vertex == edge->from->vertex && this->to->vertex == edge->to->vertex && this->weight == edge->weight) {
                return true;
            }
            return false;
        }

        static bool edgeEQ(Edge*& edge1, Edge*& edge2){
            return edge1->equals(edge2);
        }
        string toString(){
            stringstream os;
            os << "E("
                    << this->from->vertex
                    << ","
                    << this->to->vertex
                    << ","
                    << this->weight
                    << ")";
            return os.str();
        }
    };
 //END of Edge
    

//BEGIN of Iterator
public:
    class Iterator{
    private:
        typename DLinkedList<VertexNode*>::Iterator nodeIt;
        
    public:
        Iterator(AbstractGraph<T>* pGraph=0, bool begin=true){
            if(begin) {
                if(pGraph != 0) nodeIt = pGraph->nodeList.begin();
            }
            else{
                if(pGraph != 0) nodeIt = pGraph->nodeList.end();
            }
        }
        Iterator& operator=(const Iterator& iterator){
            this->nodeIt = iterator.nodeIt;
            return *this;
        }
        
        T& operator*(){
            return (*nodeIt)->vertex;
        }
        
        bool operator!=(const Iterator& iterator){
            return nodeIt != iterator.nodeIt;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            nodeIt++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
    };
//END of Iterator
};

#endif /* ABSTRACTGRAPH_H */

