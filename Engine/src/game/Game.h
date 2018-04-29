#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <src/renderer/geometry/ObjLoader.h>
#include <src/support/Animator.h>
#include "GameEntity.h"
#include "Camera.h"
#include "Striker.h"
#include "Puck.h"

/**
 * Class to hold the current game state.
 */
class Game {
 private:
  std::shared_ptr<Window> window_;
  ObjLoader obj_loader_;
  Animator animator_;
  int score_player_, score_opponent_;
  glm::vec3 color_player_, color_opponent_;
  bool running;
  void reset();
 public:
  explicit Game(std::shared_ptr<Window> window);
  std::unique_ptr<GameEntity> table, score_board, score_center;
  std::vector<std::unique_ptr<GameEntity>> scores_player, scores_opponent;
  std::unique_ptr<Puck> puck;
  std::unique_ptr<Striker> striker_player, striker_opponent;
  std::vector<GameEntity*> entities;
  glm::vec3 score_offset, score_depth_offset;
  int winning_score;
  Camera camera;
  void update(float delta_time);
  void striker_puck_collision_test(std::unique_ptr<Striker> &striker, std::unique_ptr<Puck> &puck);
  void goal_test(std::unique_ptr<Puck> &puck);
  static bool request_start;
};

#endif //ENGINE_GAME_H
