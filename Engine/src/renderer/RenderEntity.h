#ifndef ENGINE_RENDERENTITY_H
#define ENGINE_RENDERENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include <src/renderer/material/Texture.h>
#include <src/renderer/geometry/Shape.h>

struct Material {
  Material() : ambient_multiplier(0.1, 0.1, 0.1), diffuse_multiplier(1, 1, 1), specular_color(0.3, 0.3, 0.3) {}
  glm::vec3 ambient_multiplier;
  glm::vec3 diffuse_multiplier;
  glm::vec3 specular_color;
};

class RenderEntity {
 private:
  static int id_counter;
 public:
  RenderEntity(std::shared_ptr<Shape> &shape, std::shared_ptr<Texture> &texture);
  int id;
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Texture> texture;
  glm::mat4 model;
  virtual glm::vec3 getVelocity() { return glm::vec3(); };
  Material material;
};

#endif //ENGINE_RENDERENTITY_H
