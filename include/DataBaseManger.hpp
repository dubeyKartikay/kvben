#pragma once
#include "IDataBaseBinding.hpp"
#include <DataBaseFactory.hpp>
#include <vector>
class DataBaseManager {
  std::vector<IDataBaseBinding *> connections;

public:
  void createConnections(int64_t count) {
    for (int i = 0; i < count; i++) {
      connections.push_back(DataBaseFactory::createNewConnection());
    }
  }
  void init() {
    for (auto &connection : connections) {
      connection->init();
    }
  }
  void cleanup() {
    for (auto &connection : connections) {
      connection->cleanup();
    }
  }
  std::unique_ptr<Response>
  set(const std::string &key, const std::vector<std::string> &fieldnames,
      const std::shared_ptr<std::vector<std::string>> &fieldvalues,
      int64_t index) {
    return connections[index]->set(key, fieldnames, fieldvalues);
  }
  std::unique_ptr<Response> get(const std::string &key, int64_t index) {
    return connections[index]->get(key);
  }
};
