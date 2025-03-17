#include "Executor.hpp"
#include <IDataBaseBinding.hpp>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

Executor::Executor(Generator gen, int threadCount)
    : gen(gen), threadCount(threadCount) {
  dbManager.createConnections(threadCount);
  dbManager.init();
}
Executor::~Executor() { dbManager.cleanup(); }

void Executor::loadPhase() {
  u_int64_t operationCount = gen.getNumRecords();
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
                             i);
  }
  std::cout << "Waiting for threads to finish" << std::endl;
  for (int i = 0; i < threadCount; i++) {
    threads[i].join();
  }
  std::cout << "All threads finished" << std::endl;
}
void Executor::runPhase() {
  u_int64_t operationCount = gen.getOperations();
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

void Executor::loadData(u_int64_t operationCount, u_int64_t databaseIndex) {
  std::vector<std::string> keysLoaded;
  for (int i = 0; i < operationCount; i++) {
    std::string key = gen.nextKey(i);
    u_int64_t fields = gen.nextFieldCount();
    u_int64_t packetSize = gen.nextPacketSize();
    u_int64_t fieldSize = packetSize / fields;
    keysLoaded.push_back(key);
    std::vector<std::string> fieldnames;
    std::vector<std::string> fieldvalues;
    for (int j = 0; j < fields; j++) {
      fieldnames.push_back("field" + std::to_string(j));
      fieldvalues.push_back(gen.nextValue(fieldSize));
    }
    dbManager.set(key, fieldnames, fieldvalues, databaseIndex);
  }
  {
    std::lock_guard<std::mutex> lock(keyVectorMutex);
    this->keysLoaded.insert(this->keysLoaded.end(), keysLoaded.begin(),
                            keysLoaded.end());
  }
}
void Executor::runOperation(u_int64_t operationCount, u_int64_t startIndex,
                            u_int64_t databaseIndex) {
  for (int i = 0; i < operationCount; i++) {
    std::string key = keysLoaded.at(startIndex + i);
    dbManager.get(key, databaseIndex);
  }
}
