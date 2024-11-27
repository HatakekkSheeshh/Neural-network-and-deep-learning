/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Kiểm tra xem có thể tăng sức chứa của batch cuối nếu vẫn còn dư dữ liệu không
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "loader/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    class Iterator; //forward declaration for class Iterator
    
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int nbatch;
    ulong_tensor item_indices;
    int m_seed;
    
public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, 
            int batch_size, bool shuffle=true, 
            bool drop_last=false, int seed=-1)
                : ptr_dataset(ptr_dataset), 
                batch_size(batch_size), 
                shuffle(shuffle), drop_last(drop_last),
                m_seed(seed){
            //reset_indices();
            nbatch = ptr_dataset->len()/batch_size; //total_batches
            item_indices = xt::arange(0, ptr_dataset->len());
    }
    virtual ~DataLoader(){}
    void reset_indices(){
        if(this->shuffle){
            if(m_seed >= 0){
                xt::random::seed(this->m_seed);
                xt::random::default_engine_type engine(m_seed);
                xt::random::shuffle(item_indices, engine);
            }
            else if(m_seed < 0){
                xt::random::shuffle(item_indices);
            }
        }
    }
    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }
    
    //New method: from V2: end
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
public:
    Iterator begin(){
        //YOUR CODE IS HERE
        reset_indices();
        return Iterator(this, 0);
    }
    Iterator end(){
        //YOUR CODE IS HERE
        return Iterator(this, nbatch);
    }
    
    //BEGIN of Iterator
    class Iterator{
    private:
        DataLoader* loader;
        int current_batch;
    public:
        //YOUR CODE IS HERE: to define iterator
        Iterator( DataLoader* loader, int current_batch){
            this->loader = loader;
            this->current_batch = current_batch;
        }

        Iterator& operator=(const Iterator& iterator){
            this->loader = iterator.loader;
            this->current_batch = iterator.current_batch;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_batch != other.current_batch;
        }
        
        Iterator& operator++(){
            ++current_batch;
            return *this;
        }

        Iterator operator++(int){
            Iterator iterator = *this;
            ++(*this);
            return iterator;
        }

        Batch<DType,LType> operator*() const {
            int start = current_batch * loader->batch_size;
            int end = std::min(start + loader->batch_size, static_cast<int>(loader->item_indices.size()));
            
            if(!loader->drop_last && current_batch == loader->nbatch - 1){
                end = loader->ptr_dataset->len();
            }

            auto data_shape = loader->ptr_dataset->get_data_shape();
            data_shape[0] = end - start;
            xt::xarray<DType> batch_data = xt::empty<DType>(data_shape);  
            xt::xarray<LType> batch_label;
            if(loader->ptr_dataset->has_label()) {
                auto label_shape = loader->ptr_dataset->get_label_shape();
                label_shape[0] = end - start;
                batch_label = xt::empty<LType>(label_shape);
            } else {
                batch_label = xt::xarray<LType>();
            }

            for (int i = start; i < end; ++i) {
                auto item = loader->ptr_dataset->getitem(loader->item_indices[i]);
                xt::view(batch_data, i - start) = item.getData();
                if(loader->ptr_dataset->has_label()) {
                    xt::view(batch_label, i - start) = item.getLabel();
                }
            }
            return Batch<DType, LType>(batch_data, batch_label);
        }

    };
    //END of Iterator
    void foreach(function<void(Batch<DType, LType>)> func) {
        xt::xarray<DType> data;
        xt::xarray<LType> labels;
        bool first = true;

        for (auto it = begin(); it != end(); ++it) {
        auto batch = *it;
            if (first) {
                data = batch.getData();
                labels = batch.getLabel();
                first = false;
            } else {
                data = xt::concatenate(xt::xtuple(data, batch.getData()), 0);
                labels = xt::concatenate(xt::xtuple(labels, batch.getLabel()), 0);
            }
        }

        func(Batch<DType, LType>(data, labels));
    } 
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};

#endif /* DATALOADER_H */