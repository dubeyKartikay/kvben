#pragma once
#include "CoreWorkloadData.hpp"
#include "DataBaseManger.hpp"
#include "Generator.hpp"
#include <deque>
#include <memory>
#include <mutex>
#include <sys/types.h>
class Executor {
  int threadCount;
  std::unique_ptr<CoreWorkloadData> coreWorkloadData;
  DataBaseManager dbManager;
  void loadData(u_int64_t operationCount, u_int64_t startIndex,
                u_int64_t databaseIndex);
  void runOperation(u_int64_t operationCount, u_int64_t startIndex,
                    u_int64_t databaseIndex);

public:
  Executor(std::unique_ptr<CoreWorkloadData> coreWorkloadData, int threadCount);
  ~Executor();
  void loadPhase();
  void runPhase();
};
