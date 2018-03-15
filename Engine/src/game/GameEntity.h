#ifndef ENGINE_GAMEENTITY_H
#define ENGINE_GAMEENTITY_H

#include <src/renderer/RenderEntity.h>

#include <utility>

class GameEntity : public RenderEntity {
 public:
  GameEntity(std::shared_ptr<Shape> shape, std::shared_ptr<Texture> texture) :
      RenderEntity(shape, texture) {}
  virtual void update(float deltaTime) {};
};

#endif //ENGINE_GAMEENTITY_H
