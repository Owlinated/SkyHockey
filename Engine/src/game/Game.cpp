#include <glm/ext.hpp>
#include <iostream>
#include <src/renderer/material/TexturePNG.h>
#include "Game.h"

bool Game::request_start = false;

Game::Game(std::shared_ptr<Window> window) :
    window_(std::move(window)),
    obj_loader_("geometry.obj"),
    score_offset(0.0375, 0, 0),
    score_depth_offset(0, 0, 0.05),
    color_player_(0, 0, 0.5),
    color_opponent_(0.5, 0, 0),
    winning_score(7),
    running(false){
  auto table_texture = std::make_shared<TexturePNG>("table.png", true);
  auto striker_texture = std::make_shared<TexturePNG>("striker.png", true);
  auto puck_texture = std::make_shared<TexturePNG>("puck.png", true);
  auto score_board_texture = std::make_shared<TexturePNG>("score_board.png", true);
  auto score_center_texture = std::make_shared<TexturePNG>("score_center.png", true);
  auto score_left_right_texture = std::make_shared<TexturePNG>("score_left_right.png", true);

  auto table_shape = obj_loader_.loadShape("Table");
  auto striker_shape = obj_loader_.loadShape("Striker");
  auto puck_shape = obj_loader_.loadShape("Puck");
  auto score_board_shape = obj_loader_.loadShape("Scoreboard");
  auto score_center_shape = obj_loader_.loadShape("ScoreCenter");
  auto score_left_shape = obj_loader_.loadShape("ScoreLeft");
  auto score_right_shape = obj_loader_.loadShape("ScoreRight");

  table = std::make_unique<GameEntity>(table_shape, table_texture, glm::vec3());
  score_board = std::make_unique<GameEntity>(score_board_shape, score_board_texture, glm::vec3());
  score_center = std::make_unique<GameEntity>(score_center_shape, score_center_texture, score_depth_offset);
  striker_player = std::make_unique<Striker>(striker_shape, striker_texture, glm::vec3(0, 0, 1), 0.05f);
  striker_opponent = std::make_unique<Striker>(striker_shape, striker_texture, glm::vec3(0, 0, -1), 0.5f);
  puck = std::make_unique<Puck>(puck_shape, puck_texture, glm::vec3());

  // Change material properties of scoreboard, its not very well lit otherwise
  score_board->material.ambient_multiplier = glm::vec3(0.9, 0.9, 0.9);
  score_board->material.diffuse_multiplier = glm::vec3(0.1, 0.1, 0.1);

  entities = std::vector<GameEntity*>{
    table.get(), score_board.get(), score_center.get(),
    striker_player.get(), striker_opponent.get(), puck.get()
  };

  // Create scoreboard elements
  for (auto i = 0; i < winning_score - 1; ++i) {
    auto offset = score_offset * i;
    auto depth_offset = score_depth_offset * (1 - i / (float)winning_score);
    auto score_left = std::make_unique<GameEntity>(score_left_shape, score_left_right_texture, depth_offset - offset);
    auto score_right = std::make_unique<GameEntity>(score_right_shape, score_left_right_texture, depth_offset + offset);

    entities.push_back(score_left.get());
    entities.push_back(score_right.get());

    scores_player.push_back(std::move(score_left));
    scores_opponent.push_back(std::move(score_right));
  }
}

void Game::reset() {
  score_player_ = 0;
  score_opponent_ = 0;
  animator_.run(std::make_unique<FadeAnimation>(score_center->location, score_center->location, glm::vec3(), 5));
  animator_.run(std::make_unique<FadeAnimation>(
      score_center->material.ambient_multiplier, score_center->material.ambient_multiplier, glm::vec3(0, 0.5, 0), 5));
  for (int i = 0; i < winning_score - 1; ++i) {
    auto score_player = scores_player[i].get();
    auto score_opponent = scores_opponent[i].get();
    auto offset = score_offset * i;
    animator_.run(std::make_unique<FadeAnimation>(score_player->location, score_player->location, -offset, 5));
    animator_.run(std::make_unique<FadeAnimation>(
        score_player->material.ambient_multiplier,
        score_player->material.ambient_multiplier,
        glm::mix(glm::vec3(1, 1, 1), color_player_, i / (float)(2 * winning_score)),
        5));
    animator_.run(std::make_unique<FadeAnimation>(score_opponent->location, score_opponent->location, offset, 5));
    animator_.run(std::make_unique<FadeAnimation>(
        score_opponent->material.ambient_multiplier,
        score_opponent->material.ambient_multiplier,
        glm::mix(glm::vec3(1, 1, 1), color_opponent_, i / (float)(2 * winning_score)),
        5));
  }
  puck->location = glm::vec3();
  puck->velocity = glm::vec3(0, 0, -0.3f);
}

void Game::striker_puck_collision_test(std::unique_ptr<Striker> &striker, std::unique_ptr<Puck> &puck) {
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

void Game::goal_test(std::unique_ptr<Puck> &puck) {
  if (puck->location.z < -1.2f) {
    score_player_ ++;
    if(score_player_ < winning_score) {
      auto score_index = winning_score - score_player_ - 1;
      auto score_element = scores_player[score_index].get();

      animator_.run(std::make_unique<FadeAnimation>(
          score_element->location, score_element->location, score_depth_offset - score_offset * score_index, 1));
      animator_.run(std::make_unique<FadeAnimation>(
          score_element->material.ambient_multiplier, score_element->material.ambient_multiplier, color_player_, 1));

      puck->velocity = glm::vec3(0, 0, -0.3f);
      puck->location = glm::vec3();
    } else {
      animator_.run(std::make_unique<FadeAnimation>(
          score_center->location, score_center->location, score_depth_offset, 3));
      animator_.run(std::make_unique<FadeAnimation>(
          score_center->material.ambient_multiplier, score_center->material.ambient_multiplier, color_player_, 1));

      puck->velocity = glm::vec3(0, 0, 0);
      puck->location = glm::vec3(0, -1, 0);

      request_start = false;
      running = false;
      camera.stop();
    }
  } else if (puck->location.z > 1.2f) {
    score_opponent_ ++;
    if(score_opponent_ < winning_score) {
      auto score_index = winning_score - score_opponent_ - 1;
      auto score_element = scores_opponent[score_index].get();

      animator_.run(std::make_unique<FadeAnimation>(
          score_element->location, score_element->location, score_depth_offset + score_offset * score_index, 1));
      animator_.run(std::make_unique<FadeAnimation>(
          score_element->material.ambient_multiplier, score_element->material.ambient_multiplier, color_opponent_, 1));

      puck->velocity = glm::vec3(0, 0, 0.3f);
      puck->location = glm::vec3();
    } else {
      animator_.run(std::make_unique<FadeAnimation>(
          score_center->location, score_center->location, score_depth_offset, 3));
      animator_.run(std::make_unique<FadeAnimation>(
          score_center->material.ambient_multiplier, score_center->material.ambient_multiplier, color_opponent_, 1));

      puck->velocity = glm::vec3(0, 0, 0);
      puck->location = glm::vec3(0, -1, 0);

      request_start = false;
      running = false;
      camera.stop();
    }
  }
}

void Game::update(float delta_time) {
  static auto total_time = 0.0f;
  total_time += delta_time;

  if (!running && request_start) {
    running = true;
    reset();
    camera.start();
  }

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
  camera.update(delta_time);
  for (auto &entity: entities) {
    entity->update(delta_time);
  }

  // goal test
  goal_test(puck);

  // striker puck collision test
  striker_puck_collision_test(striker_player, puck);
  striker_puck_collision_test(striker_opponent, puck);

  animator_.update(delta_time);
}
