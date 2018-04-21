#include "Config.h"

bool Config::full_screen;

bool Config::forward_rendering;

bool Config::fancy_background;

int Config::shadow_blur_size = 4;

int Config::motion_blur_steps = 8;

float Config::motion_blur_step_size = 0.5f;

bool Config::perf_overlay = false;

float Config::perf_overlay_scale = 5.0f;

void Config::parse(std::vector<std::string> arguments) {
  for (const auto &arg: arguments) {
    if (arg == "-f" || arg == "--fullscreen") {
      full_screen = true;
    } else if (arg == "-p" || arg == "--perf") {
      perf_overlay = true;
    } else if (arg == "--forward") {
      forward_rendering = true;
    } else if (arg == "--space") {
      fancy_background = true;
    }
  }
}
