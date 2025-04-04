#pragma once
#include "CoreWorkload.hpp"
#include "Generator.hpp"
#include <vector>
class WorkloadReader : public Generator {
  CoreWorkload coreWorkload;
  std::vector<u_int64_t> packetSizes;
  std::vector<u_int64_t> fetchKeys;
  std::vector<u_int64_t> fieldCounts;
  u_int64_t currentIndexfetchkey = 0;
  u_int64_t currentIndexFieldCount = 0;
  u_int64_t currentIndexPacketSize = 0;

public:
  WorkloadReader(const CoreWorkload &workload, std::random_device &rd);
  u_int64_t nextPacketSize() override;
  std::string nextKey(u_int64_t keyNum) override;
  std::string nextValue(u_int64_t length) override;
  u_int64_t getNextfetchKey() override;
  u_int64_t nextFieldCount() override;
  u_int64_t getOperations() override { return fetchKeys.size(); }
  u_int64_t getNumRecords() override { return packetSizes.size(); }
};
