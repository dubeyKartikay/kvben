#include "Executor.hpp"
#include "Generator.hpp"
#include <MuserBinding.hpp>
#include <Parser.hpp>
#include <iostream>
#include <memory>
#include <string>
int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <filename> <threads> <muStore ip <muStore port>"
              << std::endl;
    return 1;
  }
  std::string filename = argv[1];
  int threads = std::stoi(argv[2]);
  std::cout << "Filename: " << filename << std::endl;
  CoreWorkload workload = Parser::parse(filename);
  workload.print();
  std::cout << "========== Load Phase ==========" << threads << std::endl;
  std::string ip = argv[3];
  int port = std::stoi(argv[4]);
  std::unique_ptr<MustoreBinding> mustorePtr =
      std::make_unique<MustoreBinding>(ip, port);
  Generator gen(workload);
  Executor executor(gen, threads, std::move(mustorePtr));
  executor.loadPhase();
  std::cout << "========== Run Phase ==========" << threads << std::endl;
  executor.runPhase();
  return 0;
}
