#ifndef ENGINE_STRIKER_H
#define ENGINE_STRIKER_H

#include <glm/detail/type_mat.hpp>
#include <glm/vec3.hpp>
#include "GameEntity.h"

class Striker : public GameEntity {
 private:
  void updateVelocity(float delta_time);
  bool corner_collision_test();
  void updateLocation(float delta_time);
  float speed_up;
 public:
  Striker(std::shared_ptr<Shape> shape,
          std::shared_ptr<Texture> texture,
          glm::vec3 location,
          float target_location_time);
  glm::vec3 target_location;
  void update(float delta_time) override;
};

#endif //ENGINE_STRIKER_H