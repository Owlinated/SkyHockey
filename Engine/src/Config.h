#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include <string>
#include <vector>
class Config {
 public:
  static bool full_screen;
  static bool forward_rendering;
  static void parse(std::vector<std::string> arguments);
};

#endif //ENGINE_CONFIG_H
