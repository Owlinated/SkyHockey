#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <src/support/Window.h>
#include <src/support/Animator.h>
#include <src/renderer/ICamera.h>

/**
 * Class to hold the current camera properties.
 */
class Camera : public ICamera {
 private:
  glm::vec3 position_, look_at_, up_;
  glm::mat4 view, projection;
  float fov_;
  Animator animator_;
 public:
  Camera();
  void start();
  void stop();
  void update(float delta_time);
  glm::mat4 getView() override { return view; }
  glm::mat4 getProjection() override { return projection; }
};

#endif