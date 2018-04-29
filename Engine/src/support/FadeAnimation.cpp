#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include "FadeAnimation.h"

/**
 * Create a new fade in/out animation.
 * @param property Property to animate.
 * @param from Starting point to animate from.
 * @param to Final animations state.
 * @param animation_duration Duration of animation.
 */
FadeAnimation::FadeAnimation(glm::vec3 &property, glm::vec3 from, glm::vec3 to, float animation_duration) :
    property_(property), from_(from), to_(to), animation_duration_(animation_duration),
    animation_progress_(0.0f), finished(false) { }

/**
 * Update the fade animation's property.
 * @param delta_time Time in seconds since last update.
 */
void FadeAnimation::update(float delta_time) {
  const auto pi = glm::pi<float>();
  animation_progress_ += delta_time;

  if (animation_progress_ >= animation_duration_) {
    property_ = to_;
    finished = true;
    return;
  }

  auto value = animation_progress_ / animation_duration_;
  auto easeValue = 0.5f * glm::sin(value * pi - 0.5f * pi) + 0.5f;
  property_ = (1-easeValue) * from_ + easeValue * to_;
}
