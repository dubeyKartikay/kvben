#include <Generator.hpp>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <sys/types.h>

Generator::Generator(const CoreWorkload &workload, std::random_device &rd)
    : workload(workload) {
  weightsCdf.push_back(this->workload.getWeights()[0]);
  srand(time(NULL));
  for (int i = 1; i < this->workload.getWeights().size(); i++) {
    weightsCdf.push_back(weightsCdf[i - 1] + this->workload.getWeights()[i]);
  }
  fieldWeightsCdf.push_back(this->workload.getFieldWeights()[0]);
  for (int i = 1; i < this->workload.getFieldWeights().size(); i++) {
    fieldWeightsCdf.push_back(fieldWeightsCdf[i - 1] +
                              this->workload.getFieldWeights()[i]);
  }
  gen = std::mt19937(rd());
  dist = std::uniform_real_distribution<double>(0, 1);
}
double Generator::getRandom() { return dist(gen); }

u_int64_t Generator::nextFieldCount() {
  double r = dist(gen);
  auto it = std::lower_bound(fieldWeightsCdf.begin(), fieldWeightsCdf.end(), r);
  std::pair<u_int64_t, u_int64_t> bin =
      this->workload.getFields()[it - fieldWeightsCdf.begin()];
  double r2 = dist(gen);
  return bin.first + r2 * (bin.second - bin.first + 1);
}
u_int64_t Generator::nextPacketSize() {
  double r = dist(gen);
  auto it = std::lower_bound(weightsCdf.begin(), weightsCdf.end(), r);
  std::pair<u_int64_t, u_int64_t> bin =
      this->workload.getBins()[it - weightsCdf.begin()];
  double r2 = dist(gen);
  int64_t res = (int64_t)bin.first + r2 * (double)(bin.second - bin.first + 1);
  return res;
}

std::string Generator::generateString(u_int64_t length) {
  std::string gen(length, 'a');
  std::string characterSet =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  for (int i = 0; i < length; i++) {
    gen[i] = characterSet[rand() % 62];
  }
  return gen;
}

std::string Generator::nextKey(u_int64_t keyNum) {
  std::string keynumStr = std::to_string(keyNum);
  return generateString(24 - keynumStr.size()) + keynumStr;
}

std::string Generator::nextValue(u_int64_t length) {
  return generateString(length);
}
