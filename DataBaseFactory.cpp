#include "MuserBinding.hpp"
#include <DataBaseFactory.hpp>
std::string DataBaseFactory::databaseType;

IDataBaseBinding *DataBaseFactory::createNewConnection() {
  if (databaseType == "Mustore") {
    return new MustoreBinding();
  }
  throw std::runtime_error("Invalid database type");
}
void DataBaseFactory::setDatabaseType(std::string type) {
  if (type == "Mustore") {
    databaseType = type;
  } else {
    throw std::runtime_error("Invalid database type");
  }
}
