#include "Generator.hpp"
#include "WorkloadReader.hpp"
#include <CoreWorkloadData.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>
CoreWorkloadData::CoreWorkloadData(std::filesystem::path workloadPath,
                                   std::random_device &rd)
    : coreWorkload(Parser::parse(workloadPath)), workloadPath(workloadPath),
      generator(coreWorkload.getFileName() != ""
                    ? std::make_unique<WorkloadReader>(coreWorkload, rd)
                    : std::make_unique<Generator>(coreWorkload, rd)) {}

bool CoreWorkloadData::loadDataFromFile(const std::filesystem::path &filePath) {
  std::ifstream inFile(filePath, std::ios::binary);
  if (!inFile) {
    return false;
  }

  // Load keys
  size_t keysSize;
  inFile.read(reinterpret_cast<char *>(&keysSize), sizeof(keysSize));
  keys.resize(keysSize);
  for (auto &key : keys) {
    size_t keySize;
    inFile.read(reinterpret_cast<char *>(&keySize), sizeof(keySize));
    key.resize(keySize);
    inFile.read(&key[0], keySize);
  }

  // Load fetchKeys
  size_t fetchKeysSize;
  inFile.read(reinterpret_cast<char *>(&fetchKeysSize), sizeof(fetchKeysSize));
  fetchKeys.resize(fetchKeysSize);
  inFile.read(reinterpret_cast<char *>(fetchKeys.data()),
              fetchKeysSize * sizeof(u_int64_t));

  // Load fieldValues
  size_t fieldValuesSize;
  inFile.read(reinterpret_cast<char *>(&fieldValuesSize),
              sizeof(fieldValuesSize));
  fieldValues.resize(fieldValuesSize);
  for (auto &fieldValue : fieldValues) {
    size_t fieldValueSize;
    inFile.read(reinterpret_cast<char *>(&fieldValueSize),
                sizeof(fieldValueSize));
    fieldValue = std::make_shared<std::vector<std::string>>(fieldValueSize);
    for (auto &value : *fieldValue) {
      size_t valueSize;
      inFile.read(reinterpret_cast<char *>(&valueSize), sizeof(valueSize));
      value.resize(valueSize);
      inFile.read(&value[0], valueSize);
    }
  }

  return true;
}

void CoreWorkloadData::saveDataToFile(const std::filesystem::path &filePath) {
  std::ofstream outFile(filePath, std::ios::binary);
  if (!outFile) {
    throw std::runtime_error("Unable to open file for writing: " +
                             filePath.string());
  }

  // Save keys
  size_t keysSize = keys.size();
  outFile.write(reinterpret_cast<const char *>(&keysSize), sizeof(keysSize));
  for (const auto &key : keys) {
    size_t keySize = key.size();
    outFile.write(reinterpret_cast<const char *>(&keySize), sizeof(keySize));
    outFile.write(key.data(), keySize);
  }

  // Save fetchKeys
  size_t fetchKeysSize = fetchKeys.size();
  outFile.write(reinterpret_cast<const char *>(&fetchKeysSize),
                sizeof(fetchKeysSize));
  outFile.write(reinterpret_cast<const char *>(fetchKeys.data()),
                fetchKeysSize * sizeof(u_int64_t));

  // Save fieldValues
  size_t fieldValuesSize = fieldValues.size();
  outFile.write(reinterpret_cast<const char *>(&fieldValuesSize),
                sizeof(fieldValuesSize));
  for (const auto &fieldValue : fieldValues) {
    size_t fieldValueSize = fieldValue->size();
    outFile.write(reinterpret_cast<const char *>(&fieldValueSize),
                  sizeof(fieldValueSize));
    for (const auto &value : *fieldValue) {
      size_t valueSize = value.size();
      outFile.write(reinterpret_cast<const char *>(&valueSize),
                    sizeof(valueSize));
      outFile.write(value.data(), valueSize);
    }
  }
}
const std::string &CoreWorkloadData::getKey(u_int64_t index) {
  return keys.at(index);
}
std::shared_ptr<std::vector<std::string>>
CoreWorkloadData::getFieldValues(u_int64_t index) {
  return fieldValues.at(index);
}

void CoreWorkloadData::init(uint64_t threadCount) {
  std::filesystem::path dataFilePath =
      coreWorkload.generateFixedSizeHash() + ".data";

  if (loadDataFromFile(dataFilePath)) {
    std::cout << "Loaded data from cache" << std::endl;
    return;
  }

  std::cout << "Generating Data" << std::endl;
  u_int64_t recordCount = generator->getNumRecords();
  std::cout << "Record Count: " << recordCount << std::endl;
  keys = std::vector<std::string>(recordCount);
  fieldValues = std::vector<std::shared_ptr<std::vector<std::string>>>(
      recordCount, nullptr);
  std::vector<std::thread> threads;

  auto initTask = [this](uint64_t start, uint64_t end) {
    for (uint64_t i = start; i < end; ++i) {
      std::cout << "Generating record " << i << std::endl;
      std::shared_ptr<std::vector<std::string>> fieldValues =
          std::make_shared<std::vector<std::string>>();

      std::string key = generator->nextKey(i);
      keys[i] = key;
      u_int64_t fields = generator->nextFieldCount();
      std::cout << "Fields: " << fields << std::endl;
      u_int64_t packetSize = generator->nextPacketSize();
      u_int64_t fieldSize = packetSize / fields;
      for (u_int64_t j = 0; j < fields; ++j) {
        fieldValues->push_back(generator->nextValue(fieldSize));
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
    u_int64_t fetch_index = generator->getNextfetchKey();
    fetchKeys.push_back(fetch_index);
  }
  std::cout << "Saving data to file" << std::endl;
  saveDataToFile(coreWorkload.generateFixedSizeHash() + ".data");
}
