/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction){
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

double CrossEntropy::forward(xt::xarray<double> Y, xt::xarray<double> t){
    // YOUR CODE IS HERE
    m_aCached_Ypred = Y;
    m_aYtarget = t;
    bool mean = (m_eReduction == REDUCE_MEAN);
    //cout << "Test 0..." << endl;
    double loss = cross_entropy(Y,t,mean);
    //cout << "Done..." << endl;
    return loss;
}

xt::xarray<double> CrossEntropy::backward() {
    // YOUR CODE IS HERE
    // Get batch size (N)
    size_t N = m_aCached_Ypred.shape()[0];
    //cout << "Test 1..." << endl;
    xt::xarray<double> gradient = -m_aYtarget / (m_aCached_Ypred + 1e-7);
    
    // Apply different reduction methods for gradient
    switch(m_eReduction) {
        case REDUCE_MEAN:
            gradient /= N;  
            break;
        case REDUCE_SUM:
            break;
        case REDUCE_NONE:

            return gradient;
    }
    //cout << "Done..." << endl;
    return gradient;
}