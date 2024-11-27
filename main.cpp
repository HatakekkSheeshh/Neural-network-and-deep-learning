#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

#include <filesystem> //require C++17
namespace fs = std::filesystem;

#include "list/listheader.h"
#include "include/dsaheader.h"
#include "include/graph/IGraph.h"
#include "demo/graph/DGraphDemo.h"
#include "demo/graph/UGraphDemo.h"
/* #include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"
#include "ann/annheader.h"
#include "loader/dataset.h"
#include "loader/dataloader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "optim/Adagrad.h"
#include "optim/Adam.h"
#include "modelzoo/twoclasses.h"
#include "modelzoo/threeclasses.h" */

// g++ -Iinclude -Isrc -std=c++17 main.cpp
int main(int argc, char** argv) {
    //dataloader:
    //case_data_wo_label_1();
    //case_data_wi_label_1();
    //case_batch_larger_nsamples();
    
    //Classification:
    //twoclasses_classification();
    //threeclasses_classification();
//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Directed Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////
    cout << "DGraphDemo1:" << endl;
    DGraphDemo1();
    cout << "DGraphDemo2:" << endl;
    DGraphDemo2();
    cout << "DGraphDemo3:" << endl;
    DGraphDemo3();
    cout << "DijkstraDemo:" << endl;
    dijkstraDemo();
//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Undirected Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////
    cout << "UGraphDemo1" << endl;
    ugraphDemo1();
    cout << "UGraphDemo2" << endl;
    ugraphDemo2();
    cout << "UGraphDemo3" << endl;
    ugraphDemo3();
    return 0;
}
