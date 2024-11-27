#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

#include <filesystem> //require C++17
namespace fs = std::filesystem;

#include "list/listheader.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"
#include "ann/annheader.h"
#include "loader/dataset.h"
#include "loader/dataloader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "optim/Adagrad.h"
#include "optim/Adam.h"
#include "modelzoo/twoclasses.h"
#include "modelzoo/threeclasses.h"

void test_forward_batch() {
  // DATA
  FCLayer fc_layer(2, 3, true);
  xt::xarray<double> weights = {{0.2, 0.5}, {0.3, 0.7}, {0.4, 0.9}};
  xt::xarray<double> bias = {0.1, -0.1, 0.2};
  fc_layer.set_weights(weights);
  fc_layer.set_bias(bias);
  xt::xarray<double> X = {{1.0, 2.0}, {0.5, 1.5}, {1.5, 0.5}};

  // expected
  xt::xarray<double> expected_output = {
      {1.3, 1.6, 2.4}, {0.95, 1.1, 1.75}, {0.65, 0.7, 1.25}};

  xt::xarray<double> output = fc_layer.forward(X);

  // result
  cout << output << endl;
  assert(xt::allclose(output, expected_output));
  std::cout << "Forward batch test passed!" << std::endl;
}

void test_backward_batch() {
  // DATA
  FCLayer fc_layer(2, 3, true);
  xt::xarray<double> weights = {{0.2, 0.5}, {0.3, 0.7}, {0.4, 0.9}};
  xt::xarray<double> bias = {0.1, -0.1, 0.2};
  fc_layer.set_weights(weights);
  fc_layer.set_bias(bias);
  xt::xarray<double> X = {{1.0, 2.0}, {0.5, 1.5}, {1.5, 0.5}};
  fc_layer.set_working_mode(true);
  fc_layer.forward(X);
  xt::xarray<double> DY = {
      {1.0, 0.5, -0.5}, {0.5, -0.5, 0.0}, {0.0, 1.0, -1.0}};

  // expect use mean
  xt::xarray<double> expected_grad_W = {
      {0.416667, 0.916667}, {0.583333, 0.25}, {-0.666667, -0.5}};
  xt::xarray<double> expected_grad_b = {0.5, 0.333333, -0.5};
  xt::xarray<double> expected_dx = {{0.15, 0.4}, {-0.05, -0.1}, {-0.1, -0.2}};

  // Thực hiện backward
  xt::xarray<double> dX = fc_layer.backward(DY);

  // public m_aGrad_W and m_aGrad_b in FCLayer
  cout << fc_layer.m_aGrad_W << endl;
  cout << fc_layer.m_aGrad_b << endl;
  cout << dX << endl;
  assert(xt::allclose(fc_layer.m_aGrad_W, expected_grad_W));
  assert(xt::allclose(fc_layer.m_aGrad_b, expected_grad_b));
  assert(xt::allclose(dX, expected_dx));
  std::cout << "Backward batch test passed!" << std::endl;
}

void test_softmax_backward() {
  // DATA
  Softmax softmax_layer;
  softmax_layer.set_working_mode(true);
  xt::xarray<double> X = {{1.0, 2.0, 3.0}, {1.0, -1.0, 0.0}};
  softmax_layer.forward(X);
  xt::xarray<double> DY = {{0.1, 0.2, -0.3}, {-0.1, 0.3, 0.0}};
  // expect
  xt::xarray<double> expected_DX = {{0.021754, 0.083605, -0.105359},
                                    {-0.040237, 0.030567, 0.00967}};

  // Thực hiện backward
  xt::xarray<double> DX = softmax_layer.backward(DY);

  // approximately
  cout << "DX :" << DX << endl;
  cout << "approximately expected_DX: " << expected_DX << endl;
}

void twoclasses_classification_1() {
  DSFactory factory("./config.txt");
  xmap<string, TensorDataset<double, double>*>* pMap =
      factory.get_datasets_2cc();
  TensorDataset<double, double>* train_ds = pMap->get("train_ds");
  TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
  TensorDataset<double, double>* test_ds = pMap->get("test_ds");
  DataLoader<double, double> train_loader(train_ds, 50, true, false);
  DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
  DataLoader<double, double> test_loader(test_ds, 50, false, false);

  int nClasses = 2;
  ILayer* layers[] = {new FCLayer(2, 50, true),        new ReLU(),
                      new FCLayer(50, 20, true),       new ReLU(),
                      new FCLayer(20, nClasses, true), new Softmax()};
  MLPClassifier model("./config.txt", "2c-classification", layers,
                      sizeof(layers) / sizeof(ILayer*));

  SGD optim(2e-3);
  CrossEntropy loss;
  ClassMetrics metrics(nClasses);
  model.compile(&optim, &loss, &metrics);
  MLPClassifier pretrained1("./config.txt");
  string base_path = "./models";
  pretrained1.load(base_path + "/" + "2c-classification/checkpoint-1", true);

  // test funtion evaluate
  double_tensor eval_rs1 = pretrained1.evaluate(&test_loader);
  double_tensor expect_1 = {0.995, 0.995,    0.9952,  0.995192,
                            0.995, 0.994998, 0.995002};
  cout << "Eval model : " << endl;
  cout << eval_rs1 << endl;
  cout << "expect Eval model : " << endl;
  cout << expect_1 << endl;
  cout << "----------------------------------" << endl;
}

void twoclasses_classification_2() {
  DSFactory factory("./config.txt");
  xmap<string, TensorDataset<double, double>*>* pMap =
      factory.get_datasets_2cc();
  TensorDataset<double, double>* train_ds = pMap->get("train_ds");
  TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
  TensorDataset<double, double>* test_ds = pMap->get("test_ds");
  DataLoader<double, double> train_loader(train_ds, 50, true, false);
  DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
  DataLoader<double, double> test_loader(test_ds, 50, false, false);

  int nClasses = 2;
  ILayer* layers[] = {new FCLayer(2, 50, true),        new ReLU(),
                      new FCLayer(50, 20, true),       new ReLU(),
                      new FCLayer(20, nClasses, true), new Softmax()};
  MLPClassifier model("./config.txt", "2c-classification", layers,
                      sizeof(layers) / sizeof(ILayer*));

  SGD optim(2e-3);
  CrossEntropy loss;
  ClassMetrics metrics(nClasses);
  model.compile(&optim, &loss, &metrics);
  MLPClassifier pretrained1("./config.txt");
  string base_path = "./models";
  pretrained1.load(base_path + "/" + "2c-classification/checkpoint-1", true);

  // test funtion predict double_tensor
  double_tensor X_double_tensor = {0.93980859328292, 0.4571194992357245};
  double_tensor predict_double_tensor =
      pretrained1.predict(X_double_tensor, true);
  double_tensor expect_2 = {0.994444, 0.005556};
  cout << "predict_double_tensor model : " << endl;
  cout << predict_double_tensor << endl;
  cout << "expect predict_double_tensor model : " << endl;
  cout << expect_2 << endl;
  cout << "----------------------------------" << endl;
}

void twoclasses_classification_3() {
  DSFactory factory("./config.txt");
  xmap<string, TensorDataset<double, double>*>* pMap =
      factory.get_datasets_2cc();
  TensorDataset<double, double>* train_ds = pMap->get("train_ds");
  TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
  TensorDataset<double, double>* test_ds = pMap->get("test_ds");
  DataLoader<double, double> train_loader(train_ds, 50, true, false);
  DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
  DataLoader<double, double> test_loader(test_ds, 50, false, false);

  int nClasses = 2;
  ILayer* layers[] = {new FCLayer(2, 50, true),        new ReLU(),
                      new FCLayer(50, 20, true),       new ReLU(),
                      new FCLayer(20, nClasses, true), new Softmax()};
  MLPClassifier model("./config.txt", "2c-classification", layers,
                      sizeof(layers) / sizeof(ILayer*));

  SGD optim(2e-3);
  CrossEntropy loss;
  ClassMetrics metrics(nClasses);
  model.compile(&optim, &loss, &metrics);
  MLPClassifier pretrained1("./config.txt");
  string base_path = "./models";
  pretrained1.load(base_path + "/" + "2c-classification/checkpoint-1", true);
   // test funtion predict DataLoader
  double_tensor predict_DataLoader = pretrained1.predict(&test_loader, false);
  double_tensor expect_3 = {
      0., 0., 0., 1., 1., 1., 0., 1., 1., 0., 0., 1., 1., 0., 1., 1., 0.,
      1., 0., 0., 1., 0., 1., 0., 0., 0., 0., 0., 1., 1., 0., 0., 0., 0.,
      1., 1., 1., 1., 1., 0., 0., 1., 0., 0., 1., 1., 0., 0., 1., 0., 1.,
      1., 0., 1., 1., 0., 1., 0., 1., 0., 1., 0., 0., 0., 0., 1., 0., 1.,
      0., 1., 0., 1., 1., 0., 1., 1., 0., 1., 1., 1., 0., 0., 0., 1., 1.,
      0., 1., 0., 1., 0., 0., 1., 1., 0., 0., 1., 1., 0., 0., 1., 0., 0.,
      1., 1., 1., 1., 0., 0., 1., 1., 1., 0., 1., 1., 0., 1., 0., 0., 1.,
      0., 0., 1., 1., 1., 0., 0., 1., 1., 0., 0., 1., 0., 0., 1., 0., 1.,
      0., 1., 0., 1., 1., 1., 1., 0., 0., 0., 0., 1., 0., 0., 1., 1., 0.,
      0., 0., 0., 1., 1., 0., 0., 1., 1., 1., 1., 0., 0., 1., 0., 1., 0.,
      0., 0., 0., 0., 1., 1., 1., 0., 1., 1., 0., 0., 1., 1., 1., 1., 0.,
      1., 0., 0., 1., 1., 0., 1., 0., 1., 0., 0., 1., 1.};
  cout << "predict : ";
  for (const auto& val : predict_DataLoader) {
    std::cout << val << " ";
  }
  cout << endl;

  cout << "expect  : ";
  for (const auto& val : expect_3) {
    std::cout << val << " ";
  }
  cout << endl;

  int correct = 0;
  for (size_t i = 0; i < predict_DataLoader.size(); ++i) {
    if (predict_DataLoader(i) == expect_3(i)) {
      ++correct;
    }
  }

  // 0.995
  std::cout << "Accuracy: "
            << static_cast<double>(correct) / predict_DataLoader.size()
            << std::endl;
  std::cout << "expect Accuracy: " << 0.995 << std::endl;
}

int main(int argc, char** argv) {
    //dataloader:
    //case_data_wo_label_1();
    //case_data_wi_label_1();
    //case_batch_larger_nsamples();
    
    //Classification:
    twoclasses_classification();
    //threeclasses_classification();

    //test_forward_batch();
    //test_backward_batch();
    //test_softmax_backward();
    //twoclasses_classification_1();
    //twoclasses_classification_2();
    //twoclasses_classification_3();
    return 0;
}