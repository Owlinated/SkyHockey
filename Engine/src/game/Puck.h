#ifndef ENGINE_PUCK_H
#define ENGINE_PUCK_H

#include <glm/vec3.hpp>
#include "GameEntity.h"

/**
 * Holds a puck, which is used to score goals.
 */
class Puck : public GameEntity {
 private:
  void updateVelocity(float delta_time);
  bool corner_collision_test();
  void updateLocation(float delta_time);
 public:
  Puck(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture, glm::vec3 location);
  void update(float delta_time) override;
};

#endif //ENGINE_PUCK_H