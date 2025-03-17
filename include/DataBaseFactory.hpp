#pragma once
#include "IDataBaseBinding.hpp"
class DataBaseFactory {
  static std::string databaseType;

public:
  static IDataBaseBinding *createNewConnection();
  static void setDatabaseType(std::string type);
};
