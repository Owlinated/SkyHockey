#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H


#include <glm/mat4x4.hpp>

/**
 * Class to hold the current camera properties.
 */
class ICamera {
public:
  virtual glm::mat4 getView() = 0;
  virtual glm::mat4 getProjection() = 0;
};

#endif