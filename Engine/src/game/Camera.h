#ifndef ENGINE_CONTROLS_H
#define ENGINE_CONTROLS_H

#include <src/support/Window.h>
#include <src/support/Animator.h>

class Camera {
 private:
  glm::vec3 position_, look_at_, up_;
  float fov_;
  Animator animator_;
 public:
  Camera();
  void start();
  void stop();
  void update(float delta_time);
  glm::mat4 view, projection;
};

#endif