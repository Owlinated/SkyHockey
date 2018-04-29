#ifndef ENGINE_RENDERENTITY_H
#define ENGINE_RENDERENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include <src/renderer/material/Texture.h>
#include <src/renderer/geometry/Shape.h>
#include <src/renderer/material/Material.h>

/**
 * Entity used for rendering.
 */
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
