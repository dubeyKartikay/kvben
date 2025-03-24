#include "Executor.hpp"
#include <IDataBaseBinding.hpp>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>
Executor::Executor(std::unique_ptr<CoreWorkloadData> coreWorkloadData,
                   int threadCount)
    : coreWorkloadData(std::move(coreWorkloadData)), threadCount(threadCount) {
  dbManager.createConnections(threadCount);
  dbManager.init();
}
Executor::~Executor() { dbManager.cleanup(); }

void Executor::loadPhase() {
  u_int64_t operationCount = coreWorkloadData->getNumRecords();
  std::thread threads[threadCount];
  u_int64_t indivialWork = operationCount / threadCount;
  for (int i = 0; i < threadCount; i++) {
    std::cout << "Thread " << i << " will load " << indivialWork << " keys "
              << "or  " << indivialWork + operationCount % threadCount
              << std::endl;
    threads[i] = std::thread(&Executor::loadData, this,
                             i == threadCount - 1
                                 ? indivialWork + operationCount % threadCount
                                 : indivialWork,
                             i * indivialWork, i);
  }
  std::cout << "Waiting for threads to finish" << std::endl;
  for (int i = 0; i < threadCount; i++) {
    threads[i].join();
  }
  std::cout << "All threads finished" << std::endl;
}
void Executor::runPhase() {
  u_int64_t operationCount = coreWorkloadData->getOperations();
  if (operationCount > coreWorkloadData->getNumRecords()) {
    std::cout << "Not enough keys loaded for " << operationCount
              << " unique reads" << std::endl;
    return;
  }
  std::thread threads[threadCount];
  u_int64_t indivialWork = operationCount / threadCount;
  for (int i = 0; i < threadCount; i++) {
    std::cout << "Thread " << i << " will run " << indivialWork << " keys "
              << "or  " << indivialWork + operationCount % threadCount
              << std::endl;
    threads[i] = std::thread(&Executor::runOperation, this,
                             i == threadCount - 1
                                 ? indivialWork + operationCount % threadCount
                                 : indivialWork,
                             i * indivialWork, i);
  }
  for (int i = 0; i < threadCount; i++) {
    threads[i].join();
  }
}

void Executor::loadData(u_int64_t operationCount, u_int64_t startIndex,
                        u_int64_t databaseIndex) {
  for (int i = 0; i < operationCount; i++) {
    std::string key = coreWorkloadData->getKey(startIndex + i);
    std::vector<std::string> fieldnames;
    auto fieldvalues = coreWorkloadData->getFieldValues(startIndex + i);
    for (int j = 0; j < fieldvalues->size(); j++) {
      fieldnames.push_back("field" + std::to_string(j));
    }
    dbManager.set(key, fieldnames, fieldvalues, databaseIndex);
  }
}
void Executor::runOperation(u_int64_t operationCount, u_int64_t startIndex,
                            u_int64_t databaseIndex) {
  for (int i = 0; i < operationCount; i++) {
    dbManager.get(coreWorkloadData->getFetchKey(i), databaseIndex);
  }
}
