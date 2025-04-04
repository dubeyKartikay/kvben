#include "WorkloadReader.hpp"
#include "CoreWorkload.hpp"
#include "csv.hpp"
#include <iostream>
#include <sys/types.h>

WorkloadReader::WorkloadReader(const CoreWorkload &workload,
                               std::random_device &rd)
    : coreWorkload(workload), Generator(workload, rd) {
  std::string fileName = coreWorkload.getFileName();
  std::ifstream file(fileName);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + fileName);
  }
  std::cout << "Reading workload from file: " << fileName << std::endl;
  csv::CSVReader reader(file);
  auto header = reader.get_col_names();
  if (std::find(header.begin(), header.end(), "packetSize") == header.end() ||
      std::find(header.begin(), header.end(), "fieldCount") == header.end() ||
      std::find(header.begin(), header.end(), "fetchKey") == header.end()) {
    throw std::runtime_error("Required columns are missing in the file: " +
                             fileName);
  }
  u_int64_t index = 0;
  for (const auto &row : reader) {
    packetSizes.push_back(row["packetSize"].get<u_int64_t>());
    fieldCounts.push_back(row["fieldCount"].get<u_int64_t>());
    bool isFetched = row["fetchKey"].get<bool>();
    if (isFetched) {
      fetchKeys.push_back(index);
    }
    index++;
  }
  std::cout << "Read " << packetSizes.size() << " records from file."
            << std::endl;
  file.close();
  if (packetSizes.size() != fieldCounts.size()) {
    throw std::runtime_error("Mismatch in number of records in the file: " +
                             fileName);
  }
}
u_int64_t WorkloadReader::getNextfetchKey() {
  if (currentIndexfetchkey >= fetchKeys.size()) {
    throw std::out_of_range("No more fetch keys available.");
  }
  return fetchKeys[currentIndexfetchkey++];
}
u_int64_t WorkloadReader::nextPacketSize() {
  if (currentIndexPacketSize >= packetSizes.size()) {
    throw std::out_of_range("No more packet sizes available.");
  }
  return packetSizes[currentIndexPacketSize++];
}
std::string WorkloadReader::nextKey(u_int64_t keyNum) {
  return Generator::nextKey(keyNum);
}
std::string WorkloadReader::nextValue(u_int64_t length) {
  return Generator::nextValue(length);
}
u_int64_t WorkloadReader::nextFieldCount() {
  if (currentIndexFieldCount >= fieldCounts.size()) {
    throw std::out_of_range("No more field counts available.");
  }
  return fieldCounts[currentIndexFieldCount++];
}
