#ifndef ENGINE_ANIMATOR_H
#define ENGINE_ANIMATOR_H

#include <vector>
#include <memory>
#include "FadeAnimation.h"

class Animator {
 private:
  std::vector<std::unique_ptr<FadeAnimation>> animations;
 public:
  Animator();
  void run(std::unique_ptr<FadeAnimation> animation);
  void update(float delta_time);
};

#endif //ENGINE_ANIMATOR_H
