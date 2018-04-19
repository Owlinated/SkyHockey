#include <glm/ext.hpp>
#include <iostream>
#include <src/renderer/material/TexturePNG.h>
#include "Game.h"

Game::Game(std::shared_ptr<Window> window) :
    window_(std::move(window)),
    obj_loader_("geometry.obj") {
  auto table_texture = std::make_shared<TexturePNG>("table.png", true);
  auto striker_texture = std::make_shared<TexturePNG>("striker.png", true);
  auto puck_texture = std::make_shared<TexturePNG>("puck.png", true);

  auto table_shape = obj_loader_.loadShape("Table");
  auto striker_shape = obj_loader_.loadShape("Striker");
  auto puck_shape = obj_loader_.loadShape("Puck");

  table = std::make_unique<GameEntity>(table_shape, table_texture, glm::vec3());
  score_board = std::make_unique<GameEntity>(puck_shape, puck_texture, glm::vec3());
  striker_player = std::make_unique<Striker>(striker_shape, striker_texture, glm::vec3(0, 0, 1), 0.05f);
  striker_opponent = std::make_unique<Striker>(striker_shape, striker_texture, glm::vec3(0, 0, -1), 0.5f);
  puck = std::make_unique<Puck>(puck_shape, puck_texture, glm::vec3());
  puck->velocity = glm::vec3(0, 0, -0.3f);

  entities = std::vector<GameEntity*>{
    table.get(), score_board.get(),
    striker_player.get(), striker_opponent.get(),
    puck.get()
  };
}

void striker_puck_collision_test(std::unique_ptr<Striker> &striker, std::unique_ptr<Puck> &puck) {
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

void goal_test(std::unique_ptr<Puck> &puck) {
  if (puck->location.z < -1.2f) {
    std::cout << "Point for player!" << std::endl;
    puck->velocity = glm::vec3(0, 0, -0.3f);
    puck->location = glm::vec3();
  } else if (puck->location.z > 1.2f) {
    std::cout << "Point for opponent!" << std::endl;
    puck->velocity = glm::vec3(0, 0, 0.3f);
    puck->location = glm::vec3();
  }
}

void Game::update(float deltaTime) {
  static auto total_time = 0.0f;
  total_time += deltaTime;

  const auto mouse_speed = 0.001f;
  double mouse_x, mouse_y, mid_x = window_->width / 2, mid_y = window_->height / 2;
  glfwGetCursorPos(window_->handle, &mouse_x, &mouse_y);
  glfwSetCursorPos(window_->handle, mid_x, mid_y);

  // update player movement
  striker_player->target_location += glm::vec3((mouse_x - mid_x) * mouse_speed, 0, (mouse_y - mid_y) * mouse_speed);

  // update "ai" movement
  auto location_difference = glm::normalize(puck->location - striker_opponent->location);
  if (puck->location.z < striker_opponent->location.z) {
    striker_opponent->target_location = glm::vec3(0, 0, -1.2f);
  } else if (puck->location.z > 0 || glm::distance2(striker_opponent->location, puck->location) > 0.2f) {
    striker_opponent->target_location = glm::vec3(puck->location.x / (puck->location.z + 2.2), 0, -1);
  } else if (glm::dot(puck->velocity, location_difference) < 0.1f) {
    striker_opponent->target_location = striker_opponent->location + 0.4 * location_difference;
  }

  // update entities
  camera.update(deltaTime);
  for (auto &entity: entities) {
    entity->update(deltaTime);
  }

  // goal test
  goal_test(puck);

  // striker puck collision test
  striker_puck_collision_test(striker_player, puck);
  striker_puck_collision_test(striker_opponent, puck);

  score_board->location = glm::vec3(cos(total_time / 3), 0.1, 1.0);
}
