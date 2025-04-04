#pragma once
#include "CoreWorkload.hpp"
#include <random>
#include <sys/types.h>
#include <vector>
class Generator {
  CoreWorkload workload;
  std::vector<double> weightsCdf;
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
  std::vector<double> fieldWeightsCdf;
  std::string generateString(u_int64_t length);

public:
  Generator(const CoreWorkload &workload, std::random_device &rd);
  virtual u_int64_t nextPacketSize();
  virtual std::string nextKey(u_int64_t keyNum);
  virtual std::string nextValue(u_int64_t length);
  virtual u_int64_t nextFieldCount();
  virtual u_int64_t getNextfetchKey();
  virtual u_int64_t getOperations() { return workload.getOperations(); }
  virtual u_int64_t getNumRecords() { return workload.getNumRecords(); }
  double getRandom();
};
