#include "DataBaseFactory.hpp"
#include "Executor.hpp"
#include "Generator.hpp"
#include "gArgs.hpp"
#include <DataBaseManger.hpp>
#include <MuserBinding.hpp>
#include <Parser.hpp>
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << "<path to workload> <threads> <database> ..." << std::endl;
    return 1;
  }
  std::string filename = argv[1];
  int threads = std::stoi(argv[2]);
  std::string database = (argv[3]);
  DataBaseFactory::setDatabaseType(database);
  GlobalArgs::parse(argc, argv);
  std::random_device rd;
  std::unique_ptr<CoreWorkloadData> coreWorkloadData =
      std::make_unique<CoreWorkloadData>(filename, rd);
  std::cout << "Initing Workload" << std::endl;
  coreWorkloadData->init(threads);
  // coreWorkloadData->printWorkload();
  std::cout << "========== Load Phase ==========" << threads << std::endl;
  Executor executor(std::move(coreWorkloadData), threads);
  executor.loadPhase();
  std::cout << "========== Run Phase ==========" << threads << std::endl;
  executor.runPhase();
  std::cout << "========== Done ==========" << threads << std::endl;
  return 0;
}
