#include <string>
#include <system_error>
#include <unordered_map>
typedef std::unordered_map<std::string, std::string> TArgs;
class GlobalArgs {
  static TArgs gArgs;

public:
  static std::string &get(std::string key) { return gArgs[key]; }
  static void parse(int argc, char *argv[]);
};
