#include <glm/gtx/transform.hpp>
#include "RenderEntity.h"

int RenderEntity::id_counter = 0;

/**
 * Create render entity.
 * @param shape Geometry for rendering.
 * @param texture Color texture for rendering.
 * @param location Location to render entity at.
 * @param velocity Veloctiy for motion blur.
 */
RenderEntity::RenderEntity(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture,
                           glm::vec3 location, glm::vec3 velocity) :
    shape(std::move(shape)), texture(std::move(texture)), id(RenderEntity::id_counter++),
    location(location), velocity(velocity) {
}

/**
 * Get the current model matrix.
 * @return The model matrix.
 */
glm::mat4 RenderEntity::model() {
  static glm::vec3 last_location;
  static glm::mat4 last_model;
  if(location != last_location) {
    last_location = location;
    last_model = glm::translate(glm::mat4(), location);
  }
  return last_model;
}
