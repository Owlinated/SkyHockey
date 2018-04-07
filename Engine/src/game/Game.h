#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <src/renderer/geometry/ObjLoader.h>
#include "GameEntity.h"
#include "Camera.h"
#include "Striker.h"
#include "Puck.h"

class Game {
 private:
  std::shared_ptr<Window> window_;
  ObjLoader obj_loader_;
 public:
  explicit Game(std::shared_ptr<Window> window);
  std::unique_ptr<GameEntity> table;
  std::unique_ptr<Puck> puck;
  std::unique_ptr<Striker> striker_player, striker_opponent;
  std::vector<GameEntity*> entities;
  Camera camera;
  void update(float deltaTime);
};

#endif //ENGINE_GAME_H
