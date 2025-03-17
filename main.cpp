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

  std::cout << "Filename: " << filename << std::endl;
  CoreWorkload workload = Parser::parse(filename);
  workload.print();

  std::random_device rd;
  Generator gen(workload, rd);

  std::cout << "========== Load Phase ==========" << threads << std::endl;

  Executor executor(gen, threads);
  executor.loadPhase();
  std::cout << "========== Run Phase ==========" << threads << std::endl;
  executor.runPhase();
  return 0;
}
