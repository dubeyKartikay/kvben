#include "Parser.hpp"
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <utility>

bool Parser::tryMatchWeights(const std::string &line, std::smatch &match,
                             std::vector<double> &weights) {
  if (match.size() != 5) {
    throw std::runtime_error("Invalid weights format : " + line +
                             " should be weights=[0.25,0.25,0.25,0.25]");
  }
  if (match[1] != "weights") {
    return false;
  }
  parseDoubleArray(match[2], weights);
  double sum = 0;
  for (auto weight : weights) {
    sum += weight;
  }
  if (sum != 1) {
    throw std::runtime_error("Weights should sum to 1 : sum=" +
                             std::to_string(sum));
  }
  return true;
}
bool Parser::tryMatchFieldWeights(const std::string &line, std::smatch &match,
                                  std::vector<double> &fieldWeights) {
  if (match.size() != 5) {
    throw std::runtime_error("Invalid weights format : " + line +
                             " should be fieldWeights=[0.25,0.25,0.25,0.25]");
  }
  if (match[1] != "fieldWeights") {
    return false;
  }
  parseDoubleArray(match[2], fieldWeights);
  double sum = 0;
  for (auto weight : fieldWeights) {
    sum += weight;
  }
  if (sum != 1) {
    throw std::runtime_error("Weights should sum to 1 : sum=" +
                             std::to_string(sum));
  }
  return true;
}

bool Parser::tryMatchFields(
    const std::string &line, std::smatch &match,
    std::vector<std::pair<u_int64_t, u_int64_t>> &fields) {
  if (match.size() != 5) {
    throw std::runtime_error(
        "Invalid bins format : " + line +
        "should be bins=[[200,500],[600,1024],[2048,4096],[8192,10000]]");
  }
  if (match[1] != "fields") {
    return false;
  }
  parse2DintArray(match[2], fields);
  return true;
}

bool Parser::tryMatchBins(const std::string &line, std::smatch &match,
                          std::vector<std::pair<u_int64_t, u_int64_t>> &bins) {
  if (match.size() != 5) {
    throw std::runtime_error(
        "Invalid bins format : " + line +
        "should be bins=[[200,500],[600,1024],[2048,4096],[8192,10000]]");
  }
  if (match[1] != "bins") {
    return false;
  }
  parse2DintArray(match[2], bins);
  return true;
}

bool Parser::tryMatchNumRecords(const std::string &line, std::smatch &match,
                                int &num_records) {
  if (match.size() != 3) {
    throw std::runtime_error("Invalid num_records format : " + line +
                             "should be numRecords=1000000");
  }
  std::ssub_match sub_match = match[1];
  if (sub_match.str() != "numRecords") {
    return false;
  }
  num_records = std::stoi(match[2]);
  return true;
}
bool Parser::tryMatchOperations(const std::string &line, std::smatch &match,
                                int &operations) {
  if (match.size() != 3) {
    throw std::runtime_error("Invalid operations format : " + line +
                             "should be operations=1000");
  }
  std::ssub_match sub_match = match[1];
  if (sub_match.str() != "operations") {
    return false;
  }
  operations = std::stoi(match[2]);
  return true;
}

CoreWorkload Parser::parse(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  std::string line;
  std::regex kv_regezx("([a-zA-Z]+)=([0-9]+)");
  std::regex IntArray2D_regex("([a-zA-Z]+)=(\\[(?:\\[(\\d+),(\\d+)\\],?)+\\])");
  std::regex DoubleArray_regex("([a-zA-Z]+)=(\\[((1|0|0.[0-9]+),?)+\\])");
  std::vector<double> weights;
  std::vector<double> fieldWeights;
  std::vector<std::pair<u_int64_t, u_int64_t>> bins;
  std::vector<std::pair<u_int64_t, u_int64_t>> fieldBins;
  int operations;
  int num_records;
  while (std::getline(file, line)) {
    std::smatch match;
    if (std::regex_match(line, match, kv_regezx)) {
      if (!tryMatchOperations(line, match, operations) &&
          !tryMatchNumRecords(line, match, num_records)) {
        throw std::runtime_error("Invalid line : " + line);
      }
    } else if (std::regex_match(line, match, IntArray2D_regex)) {
      if (!tryMatchBins(line, match, bins) &&
          !tryMatchFields(line, match, fieldBins)) {
        throw std::runtime_error("Invalid line : " + line);
      }
    } else if (std::regex_match(line, match, DoubleArray_regex)) {
      if (!tryMatchWeights(line, match, weights) &&
          !tryMatchFieldWeights(line, match, fieldWeights)) {
        throw std::runtime_error("Invalid line : " + line);
      }
    } else {
      if (line.empty()) {
        continue;
      }
      throw std::runtime_error("Invalid line : " + line);
    }
  }
  CoreWorkload workload(bins, weights, fieldBins, fieldWeights, operations,
                        num_records);
  return workload;
}
void Parser::parse2DintArray(
    std::string line, std::vector<std::pair<u_int64_t, u_int64_t>> &array) {
  std::regex bin_regex("\\[(\\d+),(\\d+)\\]");
  std::smatch bin_match;
  while (std::regex_search(line, bin_match, bin_regex)) {
    if (bin_match.size() != 3) {
      throw std::runtime_error("Invalid bin format : " + bin_match.str() +
                               " should be [200,500]");
    }
    u_int64_t binval[2] = {std::stoull(bin_match[1]),
                           std::stoull(bin_match[2])};
    array.push_back(std::make_pair(binval[0], binval[1]));
    line = bin_match.suffix();
  }
}
void Parser::parseDoubleArray(std::string line, std::vector<double> &array) {
  std::regex weight_regex("([1]|(0.[0-9]+)),?");
  std::smatch weight_match;
  while (std::regex_search(line, weight_match, weight_regex)) {
    if (weight_match.size() != 3) {
      throw std::runtime_error("Invalid weight format : " + weight_match.str() +
                               "should be 0.25");
    }
    array.push_back(std::stod(weight_match[1]));
    line = weight_match.suffix();
  }
}
void Parser::parseIntArray(std::string line, std::vector<int> &array) {
  std::regex operation_regex("([0-9]+),?");
  std::smatch operation_match;
  while (std::regex_search(line, operation_match, operation_regex)) {
    if (operation_match.size() != 2) {
      throw std::runtime_error("Invalid operation format : " +
                               operation_match.str() + "should be 1000");
    }
    array.push_back(std::stoi(operation_match[1]));
    line = operation_match.suffix();
  }
}
