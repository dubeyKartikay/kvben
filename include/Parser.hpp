#pragma once
#include "CoreWorkload.hpp"
#include <regex>
#include <string>
class Parser {
  static void
  parse2DintArray(std::string line,
                  std::vector<std::pair<u_int64_t, u_int64_t>> &array);
  static void parseDoubleArray(std::string line, std::vector<double> &array);
  static void parseIntArray(std::string line, std::vector<int> &array);
  static bool tryMatchWeights(const std::string &line, std::smatch &match,
                              std::vector<double> &weights);
  static bool tryMatchFieldWeights(const std::string &line, std::smatch &match,
                                   std::vector<double> &fieldWeights);
  static bool
  tryMatchFields(const std::string &line, std::smatch &match,
                 std::vector<std::pair<u_int64_t, u_int64_t>> &fields);
  static bool tryMatchBins(const std::string &line, std::smatch &match,
                           std::vector<std::pair<u_int64_t, u_int64_t>> &bins);
  static bool tryMatchOperations(const std::string &line, std::smatch &match,
                                 int &operations);

public:
  static CoreWorkload parse(const std::string &);
};
