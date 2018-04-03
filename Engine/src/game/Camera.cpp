#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

Camera::Camera(): position_(0, 1, 2), fov_(45), look_at_(0, -0.3f, 0), up_(0, 1, 0) {
}

void Camera::update(float delta_time) {
  static float ratio = 16.0f / 9.0f, near_clipping = 0.01f, far_clipping = 100.0f;
  projection = glm::perspective(glm::radians(fov_), ratio, near_clipping, far_clipping);
  view = glm::lookAt(position_, look_at_, up_);
}
