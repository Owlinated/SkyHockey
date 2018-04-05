#ifndef ENGINE_RENDERENTITY_H
#define ENGINE_RENDERENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include <src/renderer/material/Texture.h>
#include <src/renderer/geometry/Shape.h>

class RenderEntity {
 private:
  static int id_counter;
 public:
  RenderEntity(std::shared_ptr<Shape> &shape, std::shared_ptr<Texture> &texture);
  int id;
  glm::mat4 depth_model_view_projection;
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Texture> texture;
  glm::mat4 model;
  virtual glm::vec3 getVelocity() { return glm::vec3(); };
};

#endif //ENGINE_RENDERENTITY_H
