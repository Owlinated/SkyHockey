#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/support/FadeAnimation.h>
#include <memory>
#include "Camera.h"

Camera::Camera(): position_(0, 0.5f, 0.5f), fov_(45), look_at_(0, 0.5f, 0), up_(0, 1, 0) {
  static float ratio = 16.0f / 9.0f, near_clipping = 0.01f, far_clipping = 100.0f;
  projection = glm::perspective(glm::radians(fov_), ratio, near_clipping, far_clipping);
}

/**
 * Move camera into the player's perspective.
 */
void Camera::start() {
  animator_.run(std::make_unique<FadeAnimation>(position_, position_, glm::vec3(0, 0.9f, 2.0f), 3));
  animator_.run(std::make_unique<FadeAnimation>(look_at_, look_at_, glm::vec3(0, -0.3f, 0), 3));
}

/**
 * Reset camera position to look at scoreboard.
 */
void Camera::stop() {
  animator_.run(std::make_unique<FadeAnimation>(position_, position_, glm::vec3(0, 0.5f, 0.5f), 3));
  animator_.run(std::make_unique<FadeAnimation>(look_at_, look_at_, glm::vec3(0, 0.5f, 0), 3));
}

/**
 * Update the camera's view.
 * @param delta_time Time in seconds since last frame.
 */
void Camera::update(float delta_time) {
  animator_.update(delta_time);
  view = glm::lookAt(position_, look_at_, up_);
}
