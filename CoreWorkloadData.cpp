#include <CoreWorkloadData.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <vector>
CoreWorkloadData::CoreWorkloadData(std::filesystem::path workloadPath,
                                   std::random_device &rd)
    : coreWorkload(Parser::parse(workloadPath)), generator(coreWorkload, rd) {
  std::cout << "CoreWorkloadData: " << std::endl;
}
const std::string &CoreWorkloadData::getKey(u_int64_t index) {
  return keys.at(index);
}
std::shared_ptr<std::vector<std::string>>
CoreWorkloadData::getFieldValues(u_int64_t index) {
  return fieldValues.at(index);
}

void CoreWorkloadData::init(uint64_t threadCount) {
  std::cout << "Generating Data" << std::endl;
  u_int64_t recordCount = coreWorkload.getNumRecords();
  keys = std::vector<std::string>(recordCount);
  fieldValues = std::vector<std::shared_ptr<std::vector<std::string>>>(
      recordCount, nullptr);
  std::vector<std::thread> threads;

  auto initTask = [this](uint64_t start, uint64_t end) {
    for (uint64_t i = start; i < end; ++i) {
      std::shared_ptr<std::vector<std::string>> fieldValues =
          std::make_shared<std::vector<std::string>>();

      std::string key = generator.nextKey(i);
      keys[i] = key;
      u_int64_t fields = generator.nextFieldCount();
      u_int64_t packetSize = generator.nextPacketSize();
      u_int64_t fieldSize = packetSize / fields;
      for (u_int64_t j = 0; j < fields; ++j) {
        fieldValues->push_back(generator.nextValue(fieldSize));
      }
      this->fieldValues[i] = fieldValues;
    }
  };

  u_int64_t recordsPerThread = recordCount / threadCount;
  std::cout << "Records per thread: " << recordsPerThread << std::endl;
  for (u_int64_t t = 0; t < threadCount; ++t) {
    u_int64_t start = t * recordsPerThread;
    u_int64_t end =
        (t == threadCount - 1) ? recordCount : start + recordsPerThread;
    threads.emplace_back(initTask, start, end);
  }
  std::cout << "Waiting for threads to finish" << std::endl;
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  for (int i = 0; i < getOperations(); i++) {
    u_int64_t random_index = generator.getRandom() * keys.size();
    if (random_index == keys.size()) {
      random_index--;
    }
    fetchKeys.push_back(random_index);
  }
}
