#include "Config.h"

bool Config::full_screen;

bool Config::forward_rendering;

bool Config::fancy_background = true;

int Config::shadow_blur_size = 4;

int Config::motion_blur_steps = 8;

float Config::motion_blur_step_size = 0.5f;

bool Config::perf_overlay;

float Config::perf_overlay_scale = 5.0f;

int Config::anti_aliasing_level = 3;

// Table is 0.8m above floor. Assuming eye height of 1.7m, gives us eye offset height of 0.9m.
OVR::Vector3f Config::offset = OVR::Vector3f(0, 0.9, 1.5);

/**
 * Parse and apply a list of program arguments.
 * @param arguments Arguments to apply to config.
 */
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
