#include <glm/ext.hpp>
#include <iostream>
#include "Game.h"

Game::Game() :
    window_(Window::getInstance()),
    obj_loader_("geometry.obj"),
    camera_(Camera::getInstance()) {
  auto texture = Texture::loadDds("uvmap.DDS");
  auto striker_shape = obj_loader_.loadShape("Striker");
  auto puck_shape = obj_loader_.loadShape("Puck");

  table = std::make_shared<GameEntity>(obj_loader_.loadShape("Table"), texture);
  puck = std::make_shared<Puck>(puck_shape, texture);
  striker_player = std::make_shared<Striker>(striker_shape, texture, glm::vec3(0, 0, 1), 0.05f);
  striker_opponent = std::make_shared<Striker>(striker_shape, texture, glm::vec3(0, 0, -1), 0.5f);

  entities.emplace_back(table);
  entities.emplace_back(std::static_pointer_cast<GameEntity>(puck));
  entities.emplace_back(std::static_pointer_cast<GameEntity>(striker_player));
  entities.emplace_back(std::static_pointer_cast<GameEntity>(striker_opponent));
}

void striker_puck_collision_test(std::shared_ptr<Striker> &striker, std::shared_ptr<Puck> &puck) {
  const float radius = 0.08f;
  auto location_difference = puck->location - striker->location;
  float distance = glm::length(location_difference);
  if (distance <= radius * 2) {
    location_difference = glm::normalize(location_difference);

    // Reflect puck velocity
    float puck_velocity_projection = glm::dot(puck->velocity, location_difference);
    puck->velocity -= 2.0f * location_difference * puck_velocity_projection;
    puck->location = striker->location + location_difference * (radius + 0.001f) * 2;

    // Transfer striker velocity
    float striker_velocity_projection = glm::dot(striker->velocity, location_difference);
    if (striker_velocity_projection > 0) {
      puck->velocity += location_difference * striker_velocity_projection;
    }
  }
}

void goal_test(std::shared_ptr<Puck> &puck) {
  if (puck->location.z < -1.2f) {
    std::cout << "Point for player!" << std::endl;
    puck->location = glm::vec3(0, 0, -0.8f);
  } else if (puck->location.z > 1.2f) {
    std::cout << "Point for opponent!" << std::endl;
    puck->location = glm::vec3(0, 0, 0.8f);
  } else {
    return;
  }
  puck->velocity = glm::vec3();
}

void Game::update(float deltaTime) {
  const auto mouse_speed = 0.001f;
  double mouse_x, mouse_y, mid_x = window_.width() / 2, mid_y = window_.height() / 2;
  glfwGetCursorPos(window_.handle(), &mouse_x, &mouse_y);
  glfwSetCursorPos(window_.handle(), mid_x, mid_y);

  // update player movement
  striker_player->target_location += glm::vec3((mouse_x - mid_x) * mouse_speed, 0, (mouse_y - mid_y) * mouse_speed);

  // update "ai" movement
  auto location_difference = glm::normalize(puck->location - striker_opponent->location);
  if (puck->location.z < striker_opponent->location.z) {
    striker_opponent->target_location = glm::vec3(0, 0, -1.2f);
  } else if (puck->location.z > 0 || glm::distance2(striker_opponent->location, puck->location) > 0.2f) {
    striker_opponent->target_location = glm::vec3(puck->location.x / (puck->location.z + 2.2), 0, -1);
  } else if (glm::dot(puck->velocity, location_difference) < 0.1f) {
    striker_opponent->target_location = striker_opponent->location + 0.3 * location_difference;
  }

  // update entities
  camera_.update(deltaTime);
  for (auto &entity: entities) {
    entity->update(deltaTime);
  }

  // goal test
  goal_test(puck);

  // striker puck collision test
  striker_puck_collision_test(striker_player, puck);
  striker_puck_collision_test(striker_opponent, puck);
}