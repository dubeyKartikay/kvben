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
  u_int64_t nextPacketSize();
  std::string nextKey(u_int64_t keyNum);
  std::string nextValue(u_int64_t length);
  u_int64_t nextFieldCount();
  u_int64_t getOperations() { return workload.getOperations(); }
  u_int64_t getNumRecords() { return workload.getNumRecords(); }
};
