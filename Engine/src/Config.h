#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include <string>
#include <vector>
class Config {
 public:
  static bool full_screen;
  static bool forward_rendering;
  static bool fancy_background;
  static int shadow_blur_size;
  static int motion_blur_steps;
  static float motion_blur_step_size;
  static bool perf_overlay;
  static float perf_overlay_scale;
  static void parse(std::vector<std::string> arguments);
  static int anti_aliasing_level;
};

#endif //ENGINE_CONFIG_H
