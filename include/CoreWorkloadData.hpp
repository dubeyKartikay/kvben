#include "CoreWorkload.hpp"
#include "Generator.hpp"
#include <Parser.hpp>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

class CoreWorkloadData {
  CoreWorkload coreWorkload;
  Generator generator;
  std::vector<std::string> keys;
  std::vector<u_int64_t> fetchKeys;
  std::vector<std::shared_ptr<std::vector<std::string>>> fieldValues;

public:
  CoreWorkloadData(std::filesystem::path workloadPath, std::random_device &rd);
  void init(u_int64_t threadCount);
  const std::string &getKey(u_int64_t index);
  std::shared_ptr<std::vector<std::string>> getFieldValues(u_int64_t index);
  u_int64_t getNumRecords() { return coreWorkload.getNumRecords(); }
  u_int64_t getOperations() { return coreWorkload.getOperations(); }
  const std::string &getFetchKey(u_int64_t index) {
    return keys.at(fetchKeys.at(index));
  }
};
