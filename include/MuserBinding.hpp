#pragma once
#include "IDataBaseBinding.hpp"
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
class MustoreBinding : public IDataBaseBinding {
private:
  std::string ip;
  int port;
  int server_fd;
  std::mutex mutex;
  std::unique_ptr<Response> recieve();

public:
  void cleanup() override;
  void init() override;
  std::unique_ptr<Response>
  set(const std::string &key, const std::vector<std::string> &fieldnames,
      const std::shared_ptr<std::vector<std::string>> &fieldvalues) override;
  std::unique_ptr<Response> get(const std::string &key) override;
};
