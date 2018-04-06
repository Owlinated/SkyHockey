#include "Config.h"

bool Config::full_screen;

bool Config::forward_rendering;

void Config::parse(std::vector<std::string> arguments) {
  for (auto arg: arguments) {
    if (arg == "-f" || arg == "--fullscreen") {
      full_screen = true;
    } else if (arg == "--forward") {
      forward_rendering = true;
    }
  }
}
