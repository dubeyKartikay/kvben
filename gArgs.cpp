#include <gArgs.hpp>
TArgs GlobalArgs::gArgs;
void GlobalArgs::parse(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.rfind("--", 0) == 0) {     // Starts with "--"
      std::string key = arg.substr(2); // Remove "--"
      std::string value = ((i + 1) < argc && argv[i + 1][0] != '-')
                              ? argv[++i]
                              : ""; // Get next arg if not a flag
      gArgs[key] = value;
    }
  }
}
