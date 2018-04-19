#include <glm/gtx/transform.hpp>
#include "GameEntity.h"

void GameEntity::update(float deltaTime) {
  model = glm::translate(glm::mat4(), location);
}