#include "CoreWorkload.hpp"
#include "Generator.hpp"
#include <Parser.hpp>
#include <filesystem>
#include <fstream>
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
  std::filesystem::path workloadPath;
  std::vector<std::shared_ptr<std::vector<std::string>>> fieldValues;
  bool loadDataFromFile(const std::filesystem::path &filePath);
  void saveDataToFile(const std::filesystem::path &filePath);

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
  void printWorkload() {
    std::cout << "Keys:" << std::endl;
    for (const auto &key : keys) {
      std::cout << key << std::endl;
    }

    std::cout << "Fetch Keys:" << std::endl;
    for (const auto &fetchKey : fetchKeys) {
      std::cout << fetchKey << std::endl;
    }

    std::cout << "Field Values:" << std::endl;
    for (const auto &fieldValue : fieldValues) {
      if (fieldValue) {
        for (const auto &value : *fieldValue) {
          std::cout << value << " ";
        }
        std::cout << std::endl;
      }
    }
  }
};
