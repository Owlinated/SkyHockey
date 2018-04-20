#ifndef ENGINE_RENDERENTITY_H
#define ENGINE_RENDERENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include <src/renderer/material/Texture.h>
#include <src/renderer/geometry/Shape.h>

struct Material {
  Material() : ambient_multiplier(0.2, 0.2, 0.2), diffuse_multiplier(1, 1, 1), specular_multiplier(1, 1, 1) {}
  glm::vec3 ambient_multiplier;
  glm::vec3 diffuse_multiplier;
  glm::vec3 specular_multiplier;
};

class RenderEntity {
 private:
  static int id_counter;
 public:
  RenderEntity(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture,
               glm::vec3 location, glm::vec3 velocity);
  int id;
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Texture> texture;
  glm::mat4 model();
  Material material;
  glm::vec3 location, velocity;
};

#endif //ENGINE_RENDERENTITY_H
