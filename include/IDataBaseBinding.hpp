#pragma once
#include <bits/types/struct_iovec.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
struct Response {
  int64_t status;
  std::vector<std::byte> payload;
};
class IDataBaseBinding {
public:
  virtual ~IDataBaseBinding() = default;
  virtual void init() {
    std::cout << "Init function not implemented" << std::endl;
  }
  virtual void cleanup() {
    std::cout << "cleanup function not implemented" << std::endl;
  }
  virtual std::unique_ptr<Response>
  set(const std::string &key, const std::vector<std::string> &fieldnames,
      const std::vector<std::string> &fieldvalues) = 0;
  virtual std::unique_ptr<Response> get(const std::string &key) = 0;
};
