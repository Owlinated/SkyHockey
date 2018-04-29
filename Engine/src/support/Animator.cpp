#include "Animator.h"

Animator::Animator() = default;

/**
 * Run an animation.
 * @param animation The animation to run.
 */
void Animator::run(std::unique_ptr<FadeAnimation> animation) {
  animations.push_back(std::move(animation));
}

/**
 * Update all contained animations.
 * @param delta_time Time in seconds since last update.
 */
void Animator::update(float delta_time) {
  for (auto animation = animations.begin(); animation != animations.end();) {
    animation->get()->update(delta_time);
    if (animation->get()->finished) {
      animation = animations.erase(animation);
    } else {
      ++animation;
    }
  }
}
