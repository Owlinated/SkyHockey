#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include <glm/vec3.hpp>

/**
 * Holds light's properties.
 */
struct Light {
  Light() : position_worldspace(0, 2, 0), color(1, 1, 1) {}
  glm::vec3 position_worldspace;
  glm::vec3 color;
};

#endif //ENGINE_LIGHT_H
