/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Heap.h
 * Author: LTSACH
 *
 * Created on 22 August 2020, 18:18
 */

#ifndef HEAP_H
#define HEAP_H
#include <memory.h>
#include "heap/IHeap.h"
#include <sstream>
#include <iostream>
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 * 
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 * 
 */
template<class T>
class Heap: public IHeap<T>{
public:
    class Iterator; //forward declaration
    
protected:
    T *elements;    //a dynamic array to contain user's data
    int capacity;   //size of the dynamic array
    int count;      //current count of elements stored in this heap
    int (*comparator)(T& lhs, T& rhs);      //see above
    void (*deleteUserData)(Heap<T>* pHeap); //see above
    
public:
    Heap(   int (*comparator)(T& , T&)=0, 
            void (*deleteUserData)(Heap<T>*)=0 );
    
    Heap(const Heap<T>& heap); //copy constructor 
    Heap<T>& operator=(const Heap<T>& heap); //assignment operator
    
    ~Heap();
    
    //Inherit from IHeap: BEGIN
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T)=0);
    bool contains(T item);
    int size() ;
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T&)=0 );
    //Inherit from IHeap: END
    
    void println(string (*item2str)(T&)=0 ){
        cout << toString(item2str) << endl;
    }
    
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }
    
public:
    /* if T is pointer type:
     *     pass the address of method "free" to Heap<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  Heap<Point*> heap(&Heap<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(Heap<T> *pHeap){
        for(int idx=0; idx < pHeap->count; idx++) delete pHeap->elements[idx];
    }
    
    
private:
    bool aLTb(T& a, T& b){
        return compare(a, b) < 0;
    }
    int compare(T& a, T& b){
        if(comparator != 0) return comparator(a, b);
        else{
            if (a < b) return -1;
            else if(a > b) return 1;
            else return 0;
        }
    }
/* 
    comparator for min-heap
    a < b -> compare(a, b) = -1 -> aLTb: True
    a > b -> compare(a, b) = 1 -> aLTb: False
    => for min-heap comparator, aLTb means "a less than b"

    comparator for max-heap
    a < b -> compare(a, b) = 1 -> aLTb: False
    a > b -> compare(a, b) = -1 -> aLTb: True
    => for max-heap comparator, aLTb means "a larger than b"
 */
    
    void ensureCapacity(int minCapacity); 
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);
    
    void removeInternalData();
    void copyFrom(const Heap<T>& heap);
    
    
//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
    
    //Iterator: BEGIN
    class Iterator{
    private:
        Heap<T>* heap;
        int cursor;
    public:
        Iterator(Heap<T>* heap=0, bool begin=0){
            this->heap = heap;
            if(begin && (heap !=0)) cursor = 0;
            if(!begin && (heap !=0)) cursor = heap->size();
        }
        Iterator& operator=(const Iterator& iterator){
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }
        
        T& operator*(){
            return this->heap->elements[cursor];
        }
        bool operator!=(const Iterator& iterator){
            return this->cursor != iterator.cursor;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            cursor++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
        void remove(void (*removeItemData)(T)=0){
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
    //Iterator: END
};


//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
Heap<T>::Heap(
        int (*comparator)(T&, T&), 
        void (*deleteUserData)(Heap<T>* ) ){
    //YOUR CODE IS HERE
    this->comparator = comparator;
    this->deleteUserData = deleteUserData;

    this->capacity = 10;
    this->count = 0;
    this->elements = new T[this->capacity];

}
template<class T>
Heap<T>::Heap(const Heap<T>& heap){
    //YOUR CODE IS HERE
    copyFrom(heap);
    this->deleteUserData = nullptr;
}

template<class T>
Heap<T>& Heap<T>::operator=(const Heap<T>& heap){
    //YOUR CODE IS HERE
    if (this == &heap){
        return *this;
    }
    removeInternalData();
    copyFrom(heap);
    this->deleteUserData = nullptr;
    return *this;
}


template<class T>
Heap<T>::~Heap(){
    //YOUR CODE IS HERE
    this->removeInternalData();
}

template<class T>
void Heap<T>::push(T item){ //item  = 25
    //YOUR CODE IS HERE
    ensureCapacity(this->count+1);
    elements[count] = item;
    reheapUp(count);
    this->count++;
}
/*
      18
     /  \
    15   13
    /
  25 
 =>
      25
     /  \
    18   13
    /
  15 
=> array: [25, 18, 13, 15, , ]
           0   1    2   3
 
 */
template<class T>
T Heap<T>::pop(){
    //YOUR CODE IS HERE
    if(this->count == 0){
        throw  std::underflow_error("Calling to peek with the empty heap.");
    }
    T delElement = elements[0];
    elements[0] = elements[count-1];
    this->count--; 
    reheapDown(0);

    return delElement;
}

/*
      15
     /  \
    18   13
 => ReheapDown
      18
     /  \
    15   13
=> Array: [18, 15, 13, , , ]
 */

template<class T>
const T Heap<T>::peek(){
    //YOUR CODE IS HERE
    if(this->count == 0){
        throw  std::underflow_error("Calling to peek with the empty heap.");
    }
    return elements[0];
}


template<class T>
void Heap<T>::remove(T item, void (*removeItemData)(T)){
    //YOUR CODE IS HERE
    int index = getItem(item);
    if(index == -1) return;
    if(removeItemData != nullptr){
        removeItemData(elements[index]);
    }
    elements[index] = elements[count-1];
    this->count--;
    reheapDown(index);
}

template<class T>
bool Heap<T>::contains(T item){
    //YOUR CODE IS HERE
    int check = getItem(item);
    if(check == -1){
        return false;
    }
    return true;
}

template<class T>
int Heap<T>::size() {
    return this->count;
}

template<class T>
void Heap<T>::heapify(T array[], int size){
    //YOUR CODE IS HERE
    clear(); // clear the current heap
    for(int i = 0; i < size; i++){
        push(array[i]);
    }
}

template<class T>
void Heap<T>::clear(){
    //YOUR CODE IS HERE
    if(this->deleteUserData != nullptr){
        this->deleteUserData(this);
    }
    if(elements != nullptr){
        delete[] elements;
        elements = nullptr;
    }
    
    this->count = 0;
    this->capacity = 10;
    this->elements = new T[this->capacity];
}

template<class T>
bool Heap<T>::empty(){
    //YOUR CODE IS HERE
    return this->count == 0;
}

template<class T>
string Heap<T>::toString(string (*item2str)(T&)){
    stringstream os;
    if(item2str != 0){
        os << "[";
        for(int idx=0; idx < count -1; idx++)
            os << item2str(elements[idx]) << ",";
        if(count > 0) os << item2str(elements[count - 1]);
        os << "]";
    }
    else{
        os << "[";
        for(int idx = 0; idx < count -1; idx++)
            os << elements[idx] << ",";
        if(count > 0) os << elements[count - 1];
        os << "]";
    }
    return os.str();
}


//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
void Heap<T>::ensureCapacity(int minCapacity){
    if(minCapacity >= capacity){
        //re-allocate 
        int old_capacity = capacity;
        capacity = old_capacity + (old_capacity >> 2);
        try{
            T* new_data = new T[capacity];
            //OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity*sizeof(T));
            delete []elements;
            elements = new_data;
        }
        catch(std::bad_alloc e){
            e.what();
        }
    }
}

template<class T>
void Heap<T>::swap(int a, int b){
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template<class T>
void Heap<T>::reheapUp(int position){
    //YOUR CODE IS HERE
    while(position > 0){
        int parentIdx = (position - 1) / 2;
        if(aLTb(elements[position], elements[parentIdx])){
            swap(position, parentIdx);
            position = parentIdx;
        }
        else break;
    }
}

template<class T>
void Heap<T>::reheapDown(int position){
    //YOUR CODE IS HERE
    int left, right, selected;
    while (position < count){
        left = position * 2 + 1;
        right = position * 2 + 2;
        selected = position;
        // compare with left child
        if(left < count && aLTb(elements[left], elements[selected])){
            selected = left;
        }
        // compare with right child
        if(right < count && aLTb(elements[right], elements[selected])){
            selected = right;
        }
        if(selected == position){
            break;
        }
        swap(selected, position);
        position = selected;
    }
    
}

template<class T>
int Heap<T>::getItem(T item){
    //YOUR CODE IS HERE
    for(int i = 0; i < count; i++){
        if(compare(elements[i], item) == 0){
            return i;
        }
    }
    return -1;
}

template<class T>
void Heap<T>::removeInternalData(){
    if(this->deleteUserData != 0) deleteUserData(this); //clear users's data if they want
    delete []elements;
}

template<class T>
void Heap<T>::copyFrom(const Heap<T>& heap){
    this->capacity = heap.capacity;
    this->count = heap.count;
    this->elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;
    
    //Copy items from heap:
    for(int idx=0; idx < capacity; idx++){
        this->elements[idx] = heap.elements[idx];
    }
}

#endif /* HEAP_H */
