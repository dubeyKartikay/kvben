#pragma once
#include "DataBaseManger.hpp"
#include "Generator.hpp"
#include <deque>
#include <mutex>
#include <sys/types.h>
class Executor {
  int threadCount;
  std::mutex keyVectorMutex;
  std::vector<std::string> keysLoaded;
  Generator gen;
  DataBaseManager dbManager;
  void loadData(u_int64_t operationCount, u_int64_t databaseIndex);
  void runOperation(u_int64_t operationCount, u_int64_t startIndex,
                    u_int64_t databaseIndex);

public:
  Executor(Generator gen, int threadCount);
  ~Executor();
  void loadPhase();
  void runPhase();
};
