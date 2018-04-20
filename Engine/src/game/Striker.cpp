#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include "Striker.h"

Striker::Striker(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture, glm::vec3 location, float target_location_time) :
    GameEntity(shape, texture, location),
    target_location(location),
    speed_up(1.0f / target_location_time) {
}

void Striker::updateVelocity(float delta_time) {
  velocity =  (target_location - location) * speed_up;
}

bool Striker::corner_collision_test() {
  // Find collision corner
  const float striker_radius = 0.08, corner_radius = 0.2f;
  static glm::vec3 corner_max(0.4f, 0, 1), corner_min = corner_max * -1.0f, corner;
  if (location.x < corner_min.x && location.z < corner_min.z) {
    corner = corner_min;
  } else if (location.x < corner_min.x && location.z > corner_max.z) {
    corner = glm::vec3(corner_min.x, 0, corner_max.z);
  } else if (location.x > corner_max.x && location.z < corner_min.z) {
    corner = glm::vec3(corner_max.x, 0, corner_min.z);
  } else if (location.x > corner_max.x && location.z > corner_max.z) {
    corner = corner_max;
  } else {
    return false;
  }

  // Find difference between location and corner cetner
  auto location_difference = location - corner;
  auto distance = glm::length(location_difference);
  // Test if location is leaving corner
  if (distance < corner_radius - striker_radius) {
    return false;
  }

  // Calculate some projections onto difference
  location_difference = glm::normalize(location_difference);
  auto velocity_projection = glm::dot(velocity, location_difference);
  auto corner_projection = glm::dot(corner, location_difference);
  auto target_projection = glm::dot(target_location, location_difference);

  // Invert component of velocity that is aligned with location difference
  velocity -= 2.0f * location_difference * velocity_projection;

  // Set location to border of corner
  location = corner + location_difference * (corner_radius - striker_radius);

  // Set target to a position along the tangent of the corner
  target_location += ((corner_projection + corner_radius - striker_radius) - target_projection) * location_difference;

  return true;
}

void Striker::updateLocation(float delta_time) {
  const float radius = 0.08f;
  static glm::vec3 bound_max(0.6 - radius, 0, 1.2 - radius), bound_min = bound_max * -1.0f;

  location += velocity * delta_time;

  if(corner_collision_test()) {
    return;
  }

  if (location.x > bound_max.x) {
    target_location.x = location.x = bound_max.x;
    velocity.x *= -1;
  } else if (location.z > bound_max.z) {
    target_location.z = location.z = bound_max.z;
    velocity.z *= -1;
  } else if (location.x < bound_min.x) {
    target_location.x = location.x = bound_min.x;
    velocity.x *= -1;
  } else if (location.z < bound_min.z) {
    target_location.z = location.z = bound_min.z;
    velocity.z *= -1;
  }
}

void Striker::update(float delta_time) {
  updateVelocity(delta_time);
  updateLocation(delta_time);
}


