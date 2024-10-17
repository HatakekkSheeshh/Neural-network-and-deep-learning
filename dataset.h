/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "ann/xtensor_lib.h"
using namespace std;

template<typename DType, typename LType>
class DataLabel{
private:
    xt::xarray<DType> data;//mảng 
    xt::xarray<LType> label;//mảng
public:
    DataLabel(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    xt::xarray<DType> getData() const{ return data; }
    xt::xarray<LType> getLabel() const{ return label; }
};

template<typename DType, typename LType>
class Batch{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
public:
    Batch(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    virtual ~Batch(){}
    xt::xarray<DType>& getData(){return data; }
    xt::xarray<LType>& getLabel(){return label; }
};


template<typename DType, typename LType>
class Dataset{
private:
public:
    Dataset(){};
    virtual ~Dataset(){};
    
    virtual int len()=0;
    virtual DataLabel<DType, LType> getitem(int index)=0;
    virtual xt::svector<std::size_t> get_data_shape()=0;
    virtual xt::svector<std::size_t> get_label_shape()=0;

    virtual bool has_label() const = 0; 
}; 

//////////////////////////////////////////////////////////////////////
template<typename DType, typename LType>
class TensorDataset: public Dataset<DType, LType>{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<std::size_t> data_shape, label_shape;
    
public:
    /* TensorDataset: 
     * need to initialize:
     * 1. data, label;
     * 2. data_shape, label_shape
    */
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label){
        /* TODO: your code is here for the initialization
        */
       this->data = data;
       this->label = label;
       this->data_shape = data.shape();
       this->label_shape = label.shape();
       //.shape: số lượng dữ liệu
    }
    /* len():
     *  return the size of dimension 0
    */
    int len(){
        /* TODO: your code is here to return the dataset's length
         */
        if(data.shape()[0] > 0){
            return data.shape()[0];//trả về chiều của trục đầu tiên
        }
        else return 0;
    }

    bool has_label() const override{
        if(label.shape().size() > 0){
            return true;
        }
        return false;
    }

    /* getitem:
     * return the data item (of type: DataLabel) that is specified by index
     */
    //
    DataLabel<DType, LType> getitem(int index){
        /* TODO: your code is here
         */
        if(!(index>=0 && index<len())){
            throw std::out_of_range("Index is out of range!");
        }
        xt::xarray<DType> data_row = xt::view(data, index, xt::all()); //xem dữ liệu ở data
        xt::xarray<LType> label_row;
        if (label.shape().size() == 0){
            label_row = label; //trả về rỗng
        } 
        else{
            label_row = xt::view(label, index, xt::all()); //trích xuất nhãn 
        }
        return DataLabel<DType, LType>(data_row, label_row);
    }
    

    xt::svector<std::size_t> get_data_shape(){
        /* TODO: your code is here to return data_shape
         */
        return data_shape;
    }
    xt::svector<std::size_t> get_label_shape(){
        /* TODO: your code is here to return label_shape
         */
        return label_shape;
    }
};



#endif /* DATASET_H */
