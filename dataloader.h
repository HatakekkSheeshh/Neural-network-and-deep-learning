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
#include "ann/xtensor_lib.h"
#include "ann/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    class Iterator;
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int seed;
    xt::xarray<int> indices;
    int total_batches;

    void reset_indices() {
        indices = xt::arange<int>(ptr_dataset->len());
        
        if (shuffle) {
            if(seed >= 0){
                xt::random::seed(seed);
                xt::random::default_engine_type engine(seed);
                xt::random::shuffle(indices, engine);
            }
            else if(seed < 0){
                xt::random::shuffle(indices);
            }
            
        }
    }

public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size, bool shuffle=true, bool drop_last=false, int seed=-1){
        this->ptr_dataset = ptr_dataset;
        this->batch_size = batch_size;
        this->shuffle = shuffle;
        this->drop_last = drop_last;
        this->seed = seed;
        reset_indices();
        int datasetLen = ptr_dataset->len();
        total_batches = datasetLen / batch_size;
    }

    virtual ~DataLoader(){}

    Iterator begin() {
        reset_indices();
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, total_batches);
    }

    class Iterator{
    private:
        DataLoader* loader;
        int current_batch;
    public:
        Iterator(DataLoader* loader, int current_batch){
            this->loader = loader;
            this->current_batch = current_batch;
        }

        Iterator& operator=(const Iterator& iterator) {
            loader = iterator.loader;
            current_batch = iterator.current_batch;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_batch != other.current_batch;
        }

        Iterator& operator++() {
            ++current_batch;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++current_batch;
            return temp;
        }

        Batch<DType, LType> operator*() const {
            // Tính toán chỉ số bắt đầu và kết thúc của batch hiện tại
            int start = current_batch * loader->batch_size;
            int end = std::min(start + loader->batch_size, static_cast<int>(loader->indices.size()));

            if (!loader->drop_last && current_batch == loader->total_batches - 1) {
                // Batch cuối sẽ chứa tất cả các phần tử còn lại, bao gồm cả dữ liệu dư
                end = loader->ptr_dataset->len();  // Lấy tất cả phần dữ liệu còn lại
            }

            // Lấy kích thước dữ liệu và nhãn từ dataset
            auto data_shape = loader->ptr_dataset->get_data_shape();
            data_shape[0] = end - start;
            xt::xarray<DType> batch_data = xt::empty<DType>(data_shape);  
            
            xt::xarray<LType> batch_label;
            if(loader->ptr_dataset->has_label()) {
                auto label_shape = loader->ptr_dataset->get_label_shape();
                label_shape[0] = end - start;
                batch_label = xt::empty<LType>(label_shape);
            } else {
                // Tạo nhãn rỗng nếu không có nhãn
                batch_label = xt::xarray<LType>();  // Khởi tạo mảng nhãn rỗng
            }

            // Sao chép dữ liệu vào batch
            for (int i = start; i < end; ++i) {
                auto item = loader->ptr_dataset->getitem(loader->indices[i]);
                xt::view(batch_data, i - start) = item.getData();
                if(loader->ptr_dataset->has_label()) {
                    xt::view(batch_label, i - start) = item.getLabel();
                }
            }

            // Trả về Batch với cả dữ liệu và nhãn (nhãn có thể là rỗng)
            return Batch<DType, LType>(batch_data, batch_label);
        }

    };

     void foreach(function<void(Batch<DType, LType>)> func) {
        xt::xarray<DType> all_data;
        xt::xarray<LType> all_labels;
        bool first_batch = true;

        for (auto it = begin(); it != end(); ++it) {
        auto batch = *it;
            if (first_batch) {
                all_data = batch.getData();
                all_labels = batch.getLabel();
                first_batch = false;
            } else {
                all_data = xt::concatenate(xt::xtuple(all_data, batch.getData()), 0);
                all_labels = xt::concatenate(xt::xtuple(all_labels, batch.getLabel()), 0);
            }
        }

        func(Batch<DType, LType>(all_data, all_labels));
    } 
};

#endif /* DATALOADER_H */