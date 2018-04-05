#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include "Puck.h"

Puck::Puck(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture) :
    GameEntity(std::move(shape), std::move(texture)) {
}

void Puck::updateVelocity(float delta_time){
  const double linear_friction = 0.01, constant_friction = 0.05;

  velocity *= 1.0 - (linear_friction * delta_time);

  auto constant_friction_normalized = constant_friction * delta_time;

  if(velocity.x < constant_friction_normalized && velocity.x > -constant_friction_normalized) {
    velocity.x = 0;
  } else {
    velocity.x -= velocity.x > 0 ? constant_friction_normalized : -constant_friction_normalized;
  }

  if(velocity.z < constant_friction_normalized && velocity.z > -constant_friction_normalized) {
    velocity.z = 0;
  } else {
    velocity.z -= velocity.z > 0 ? constant_friction_normalized : -constant_friction_normalized;
  }
}

bool Puck::corner_collision_test() {
  // Find collision corner
  const float puck_radius = 0.08, corner_radius = 0.2f;
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
  if (distance < corner_radius - puck_radius) {
    return false;
  }

  // Calculate some projections onto difference
  location_difference = glm::normalize(location_difference);
  auto velocity_projection = glm::dot(velocity, location_difference);
  auto corner_projection = glm::dot(corner, location_difference);

  // Invert component of velocity that is aligned with location difference
  velocity -= 2.0f * location_difference * velocity_projection;

  // Set location to border of corner
  location = corner + location_difference * (corner_radius - puck_radius);

  return true;
}

void Puck::updateLocation(float delta_time) {
  const float bounce_slowdown = 0.8, radius = 0.08;
  static glm::vec3 bound_max(0.6 - radius, 0, 1.2 - radius), bound_min = bound_max * -1.0f;
  static float goal_max = 0.2f - radius, goal_min = goal_max * -1.0f;

  location += velocity * delta_time;

  if (corner_collision_test()) {
    return;
  }

  if (location.x > bound_max.x) {
    location.x = bound_max.x;
    velocity.x *= -1;
  } else if (location.z > bound_max.z && (location.x < goal_min || location.x > goal_max)) {
    location.z = bound_max.z;
    velocity.z *= -1;
  } else if (location.x < bound_min.x) {
    location.x = bound_min.x;
    velocity.x *= -1;
  } else if (location.z < bound_min.z && (location.x < goal_min || location.x > goal_max)) {
    location.z = bound_min.z;
    velocity.z *= -1;
  } else {
    return;
  }

  velocity *= bounce_slowdown;
}

void Puck::update(float delta_time) {
  updateVelocity(delta_time);
  updateLocation(delta_time);

  model = glm::translate(glm::mat4(), location);
}
