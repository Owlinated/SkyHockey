#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H

#include <src/support/Window.h>

class Camera {
 private:
  glm::vec3 position_, look_at_, up_;
  float fov_;
 public:
  Camera();
  void update(float delta_time);
  glm::mat4 view, projection;
};

#endif