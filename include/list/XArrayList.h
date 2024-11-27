/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H

#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
#include "list/IList.h"

using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // khai báo trước

protected:
    T *data;                                 // mảng động để lưu trữ các phần tử của danh sách
    int capacity;                            // kích thước của mảng động
    int count;                               // số lượng phần tử được lưu trữ trong mảng
    bool (*itemEqual)(T &lhs, T &rhs);       // con trỏ hàm: kiểm tra hai phần tử (kiểu: T&) có bằng nhau hay không
    void (*deleteUserData)(XArrayList<T> *); // con trỏ hàm: được gọi để xóa các phần tử (nếu chúng là kiểu con trỏ)

public: 
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = nullptr,
        bool (*itemEqual)(T &, T &) = nullptr,
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

    // Kế thừa từ IList: BẮT ĐẦU
    void add(T e) override;
    void add(int index, T e) override;
    T removeAt(int index) override;
    bool removeItem(T item, void (*removeItemData)(T) = nullptr) override;
    bool empty() override;
    int size() override;
    void clear() override;
    T &get(int index) override;
    int indexOf(T item) override;
    bool contains(T item) override;
    string toString(string (*item2str)(T &) = nullptr) override;
    // Kế thừa từ IList: KẾT THÚC

    void println(string (*item2str)(T &) = nullptr)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = nullptr)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    /** free:
     * nếu T là kiểu con trỏ:
     *     truyền ĐỊA CHỈ của phương thức "free" vào constructor của XArrayList<T>:
     *     để: xóa dữ liệu của người dùng (nếu cần)
     * Ví dụ:
     *  XArrayList<Point*> list(&XArrayList<Point*>::free);
     *  => Destructor sẽ gọi free thông qua con trỏ hàm "deleteUserData"
     */
    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            ++it;
        }
    }

protected:
    void checkIndex(int index);     // kiểm tra tính hợp lệ của chỉ số để truy cập
    void ensureCapacity(int capacity); // tự động cấp phát nếu cần

    /** equals:
     * nếu T: kiểu nguyên thủy:
     *      indexOf, contains: sẽ sử dụng toán tử == gốc
     *      để: so sánh hai phần tử kiểu T
     * nếu T: kiểu đối tượng:
     *      indexOf, contains: sẽ sử dụng toán tử == gốc
     *      để: so sánh hai phần tử kiểu T
     *      Do đó, lớp kiểu T PHẢI ghi đè toán tử ==
     * nếu T: kiểu con trỏ:
     *      indexOf, contains: sẽ sử dụng con trỏ hàm "itemEqual"
     *      để: so sánh hai phần tử kiểu T
     *      Do đó:
     *      (1): phải truyền itemEqual vào constructor của XArrayList
     *      (2): phải định nghĩa một phương thức để so sánh
     *           nội dung được trỏ bởi hai con trỏ kiểu T
     *          Xem: định nghĩa của "equals" của lớp Point để biết thêm chi tiết
     */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == nullptr)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);
    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  ĐỊNH NGHĨA LỚP TRONG ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BẮT ĐẦU
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = nullptr, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = nullptr)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != nullptr)
                removeItemData(item);
            cursor -= 1; // PHẢI giữ chỉ số của phần tử trước đó, để ++ sau này
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Nạp chồng toán tử ++ tiền tố
        Iterator &operator++()
        {
            ++this->cursor;
            return *this;
        }
        // Nạp chồng toán tử ++ hậu tố
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
    // Iterator: KẾT THÚC
};

//////////////////////////////////////////////////////////////////////
////////////////////////     ĐỊNH NGHĨA PHƯƠNG THỨC      ///////////////////
//////////////////////////////////////////////////////////////////////

//constructor
template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),    
    int capacity)
{
    // Khởi tạo các thành viên
    this->capacity = capacity;
    this->count = 0;
    this->data = new T[capacity]; // Cấp phát động bộ nhớ cho mảng
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
    capacity = list.capacity;
    count = list.count;
    data = new T[capacity]; 
    for(int i = 0; i < list.count; i++){
        data[i] = list.data[i];
    }
    deleteUserData = list.deleteUserData;
    itemEqual = list.itemEqual;
}

template <class T>
void XArrayList<T>::removeInternalData()
{

    if (deleteUserData)
    {
        deleteUserData(this);
    }
    delete[] data;
    data = nullptr;
    count = 0;
    capacity = 0;
}

//Constructor
template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    copyFrom(list);
}

//Gán
template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{

    copyFrom(list);
    return *this;
}

//Destructor
template <class T>
XArrayList<T>::~XArrayList()
{
    removeInternalData();
}

template <class T>
void XArrayList<T>::add(T e)
{
    ensureCapacity(count + 1);
    data[count++] = e;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    checkIndex(index);
    ensureCapacity(count + 1);
    for(int i = count; i > index; --i){
        data[i] = data[i-1];
    }
    data[index] = e;
    ++count;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
    if (index < 0 || index > this->count - 1) {
        throw std::out_of_range("Index is out of range!");
    }
    T elementToremove = data[index];
    for (int i = index; i < count - 1; ++i) {
        data[i] = data[i + 1]; 
    }
    --this->count;
    return elementToremove;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T)){
    for (int i = 0; i < count; ++i) {

        if (equals(this->data[i], item, this->itemEqual)) {

            T removedItem = removeAt(i);
            if (removeItemData) {
                removeItemData(removedItem);
            }

            return true;
        }
    }
    
    return false; 
}

template <class T>
bool XArrayList<T>::empty()
{
    return this->count == 0;
}

template <class T>
int XArrayList<T>::size()
{
    return this->count;
}

template <class T>
void XArrayList<T>::clear() {
    if (deleteUserData) {
        deleteUserData(this);
    }
    this->count = 0; 

}

template <class T>
T &XArrayList<T>::get(int index)
{
    if (index < 0 || index > this->count - 1) {
        throw std::out_of_range("Index is out of range!");
    }

    return this->data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    for(int i = 0; i < count; ++i){
        if(equals(item, data[i], itemEqual)){
            return i;
        }
    }
    
    // Nếu không tìm thấy phần tử, trả về -1
    return -1;
}

template <class T>
bool XArrayList<T>::contains(T item)
{
    return indexOf(item) != -1;
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
    stringstream os;
    os << "[";
    for(int i = 0; i < count; ++i){
        if(item2str){
            os << item2str(data[i]);
        }
        else{
            os << data[i];
        }
        if(i != count - 1){
            os << ", ";
        }
    }
    os << "]";
    return os.str();    
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) ĐỊNH NGHĨA PHƯƠNG THỨC //////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
void XArrayList<T>::checkIndex(int index)
{
    if(index < 0 || index > this->count){
        throw std::out_of_range("Index is out of range!");
    }
}


template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
    if(index >= this->capacity){
        int add = index - this->capacity +1;
        int newCapacity = this->capacity + add; 
        T *newData = new T[newCapacity];
        for (int i = 0; i < this->count; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        this->capacity = newCapacity;
    }    
}

#endif /* XARRAYLIST_H */