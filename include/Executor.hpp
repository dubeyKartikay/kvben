#pragma once
#include "Generator.hpp"
#include "MuserBinding.hpp"
#include <memory>
#include <sys/types.h>
#include <vector>
class Executor {
  int threadCount;
  std::unique_ptr<MustoreBinding> mustore;
  std::vector<std::string> keysLoaded;
  Generator gen;
  void loadData(u_int64_t operationCount);
  void runOperation(u_int64_t operationCount);

public:
  Executor(Generator gen, int threadCount,
           std::unique_ptr<MustoreBinding> mustore);
  void loadPhase();
  void runPhase();
};
