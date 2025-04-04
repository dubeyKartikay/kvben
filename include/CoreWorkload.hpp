#pragma once
#include <Crypto.hpp>
#include <cstddef>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <utility>
#include <vector>
class CoreWorkload {
  std::vector<std::pair<u_int64_t, u_int64_t>> bins;
  std::vector<double> weights;
  std::vector<std::pair<u_int64_t, u_int64_t>> fields;
  std::vector<double> field_weights;
  std::filesystem::path workload_path;
  int operations;
  int num_records;

  const std::vector<std::pair<u_int64_t, u_int64_t>> &getBins() { return bins; }
  const std::vector<double> &getWeights() { return weights; }
  int getOperations() { return operations; }

  int getNumRecords() { return num_records; }
  const std::vector<std::pair<u_int64_t, u_int64_t>> &getFields() {
    return fields;
  }
  const std::vector<double> &getFieldWeights() { return field_weights; }

public:
  CoreWorkload(const std::vector<std::pair<u_int64_t, u_int64_t>> &bins,
               const std::vector<double> &weights,
               const std::vector<std::pair<u_int64_t, u_int64_t>> &fields,
               const std::vector<double> &field_weights, int operations,
               int num_records)
      : bins(bins.begin(), bins.end()), weights(weights.begin(), weights.end()),
        operations(operations), fields(fields.begin(), fields.end()),
        field_weights(field_weights.begin(), field_weights.end()),
        num_records(num_records), workload_path("") {
    print();
  }
  CoreWorkload(std::filesystem::path workload_path)
      : workload_path(workload_path) {
    print();
  }

  const std::filesystem::path &getFileName() const {
    return this->workload_path;
  }
  std::string generateFixedSizeHash() const {
    std::ostringstream oss;
    for (const auto &bin : bins) {
      oss << bin.first << bin.second;
    }
    for (const auto &weight : weights) {
      oss << weight;
    }
    for (const auto &field : fields) {
      oss << field.first << field.second;
    }
    for (const auto &field_weight : field_weights) {
      oss << field_weight;
    }
    oss << operations << num_records;

    std::string data = oss.str();
    std::string hash = Crypto::getSHA256Hash(data);
    if (this->getFileName() != "") {
      std::string fileHash = Crypto::getSHA256Hash(this->getFileName());
      hash = Crypto::getSHA256Hash(hash + fileHash);
    }
    return hash;
  }

  void print() {
    std::cout << "CoreWorkLoad: " << std::endl;
    if (workload_path != "") {
      std::cout << "Workload Path: " << workload_path << std::endl;
      return;
    }
    std::cout << "Bins: " << std::endl;
    for (auto bin : bins) {
      std::cout << "[" << bin.first << "," << bin.second << "]" << std::endl;
    }
    std::cout << "Weights: " << std::endl;
    for (auto weight : weights) {
      std::cout << weight << std::endl;
    }
    std::cout << "Operations: " << operations << std::endl;
    std::cout << operations << std::endl;
    std::cout << "Num Records: " << num_records << std::endl;
    std::cout << "Fields: " << std::endl;
    for (auto field : fields) {
      std::cout << "[" << field.first << "," << field.second << "]"
                << std::endl;
    }
    std::cout << "Field Weights: " << std::endl;
    for (auto field_weight : field_weights) {
      std::cout << field_weight << std::endl;
    }
  }
  friend class Generator;
};
