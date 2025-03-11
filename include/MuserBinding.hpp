#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>
class MustoreBinding {
private:
  std::string ip;
  int port;
  int server_fd;
  std::mutex mutex;

public:
  MustoreBinding(const std::string &ip, int port);
  ~MustoreBinding();
  int64_t set(const std::string &key,
              const std::vector<std::string> &fieldnames,
              const std::vector<std::string> &fieldvalues);
  int64_t get(const std::string &key);
  std::string recieve();
};
