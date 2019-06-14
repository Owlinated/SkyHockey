#ifndef ENGINE_OCULUS_EYE_H
#define ENGINE_OCULUS_EYE_H

#include <GL/glew.h>
#include <src/renderer/material/IFramebuffer.h>
#include "OVR_CAPI.h"
#include "src/renderer/ICamera.h"

/**
 * Represents an eye as a render target.
 */
class OculusEye : public IFramebuffer, public ICamera
{
  friend class Oculus;
public:
  OculusEye(ovrSession session, ovrEyeType_ eye);
  ~OculusEye();
  void bind() override;
  int getWidth() override { return width_; }
  int getHeight() override { return height_; }
  void updateViewProjection(ovrPosef);
  void endFrame();
  glm::mat4 getView() override { return view_; }
  glm::mat4 getProjection() override { return projection_; }

private:
  GLuint handle_;
  ovrSession session_;
  ovrEyeType_ eye_;
  ovrTextureSwapChain swap_chain_ { };
  ovrEyeRenderDesc render_desc_ { };
  int width_, height_;
  glm::mat4 view_ { }, projection_ { };
};

#endif //ENGINE_OCULUS_EYE
