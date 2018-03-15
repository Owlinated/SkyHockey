#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <src/renderer/geometry/ObjLoader.h>
#include "GameEntity.h"
#include "Camera.h"
#include "Striker.h"
#include "Puck.h"

class Game {
 private:
  Window &window_;
  ObjLoader obj_loader_;
  Camera &camera_;

  std::shared_ptr<GameEntity> table, debug_puck;
  std::shared_ptr<Puck> puck;
  std::shared_ptr<Striker> striker_player, striker_opponent;
 public:
  Game();
  std::vector<std::shared_ptr<GameEntity>> entities;
  void update(float deltaTime);
};

#endif //ENGINE_GAME_H
