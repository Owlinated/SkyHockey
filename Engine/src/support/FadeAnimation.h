#ifndef ENGINE_FADEANIMATION_H
#define ENGINE_FADEANIMATION_H

#include <glm/vec3.hpp>

class FadeAnimation {
 private:
  glm::vec3 &property_;
  glm::vec3 from_, to_;
  float animation_duration_, animation_progress_;
 public:
  bool finished;
  FadeAnimation(glm::vec3 &property, glm::vec3 from, glm::vec3 to, float animation_duration);
  void update(float delta_time);
};

#endif //ENGINE_FADEANIMATION_H
