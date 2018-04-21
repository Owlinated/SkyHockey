#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H

#include <glm/vec3.hpp>

struct Material {
  Material() : ambient_multiplier(0.5, 0.5, 0.5), diffuse_multiplier(0.5, 0.5, 0.5), specular_multiplier(1, 1, 1) {}
  glm::vec3 ambient_multiplier;
  glm::vec3 diffuse_multiplier;
  glm::vec3 specular_multiplier;
};

#endif //ENGINE_MATERIAL_H
