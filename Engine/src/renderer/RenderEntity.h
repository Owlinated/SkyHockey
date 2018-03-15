#ifndef ENGINE_RENDERENTITY_H
#define ENGINE_RENDERENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include <src/renderer/material/Texture.h>
#include <src/renderer/geometry/Shape.h>

class RenderEntity {
 public:
  RenderEntity(std::shared_ptr<Shape> &shape, std::shared_ptr<Texture> &texture):
      shape(std::move(shape)), texture(std::move(texture)) {}
  glm::mat4 depth_model_view_projection;
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Texture> texture;
  glm::mat4 model;
};

#endif //ENGINE_RENDERENTITY_H
