#ifndef ENGINE_OCULUS_H
#define ENGINE_OCULUS_H

#include "OVR_CAPI.h"
#include "OculusEye.h"
#include <memory>

/**
 * Holds an oculus hmd session. Interfaces with native sdk.
 */
class Oculus {
public:
  Oculus();
  ~Oculus();
  std::shared_ptr<OculusEye> Left, Right;
  void WaitToBeginFrame(long long  frameIndex);
  void BeginFrame(long long  frameIndex);
  void EndFrame(long long  frameIndex);
  glm::vec3 GetActiveHand(long long  frameIndex);

private:
  ovrSession session_;
  ovrGraphicsLuid luid_;
  ovrLayerEyeFov layer_;
};

#endif //ENGINE_OCULUS_H
