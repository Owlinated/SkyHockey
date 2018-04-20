#ifndef ENGINE_GAMEENTITY_H
#define ENGINE_GAMEENTITY_H

#include <src/renderer/RenderEntity.h>

#include <utility>

class GameEntity : public RenderEntity {
 public:
  GameEntity(const std::shared_ptr<Shape> &shape, const std::shared_ptr<Texture> &texture, glm::vec3 location) :
      RenderEntity(shape, texture, location, glm::vec3()) {}
  virtual void update(float deltaTime);
};

#endif //ENGINE_GAMEENTITY_H
