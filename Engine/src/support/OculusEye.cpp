#include "OculusEye.h"
#include "OVR_CAPI.h"
#include "OVR_CAPI_GL.h"

#include "Logger.h"
#include "src\Config.h"
#include "Extras/OVR_Math.h"
#include <glm/gtc/type_ptr.hpp>


OculusEye::OculusEye(ovrSession session, ovrEyeType_ eye) : session_(session), eye_(eye)
{
  glGenFramebuffers(1, &handle_);
  glBindFramebuffer(GL_FRAMEBUFFER, handle_);

  const auto hmdDescription = ovr_GetHmdDesc(session_);
  const auto textureSize = ovr_GetFovTextureSize(session_, eye, hmdDescription.DefaultEyeFov[eye_], 1.0f);
  render_desc_ = ovr_GetRenderDesc(session_, ovrEye_Left, hmdDescription.DefaultEyeFov[eye_]);
  width_ = textureSize.w;
  height_ = textureSize.h;

  ovrTextureSwapChainDesc desc = {};
  desc.Type = ovrTexture_2D;
  desc.ArraySize = 1;
  desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
  desc.Width = width_;
  desc.Height = height_;
  desc.MipLevels = 1;
  desc.SampleCount = 1;
  desc.StaticImage = ovrFalse;

  ovr_CreateTextureSwapChainGL(session, &desc, &swap_chain_);

  glGenFramebuffers(1, &handle_);
}

OculusEye::~OculusEye()
{
  ovr_DestroyTextureSwapChain(session_, swap_chain_);
}

void OculusEye::bind()
{
  GLuint textureHandle;
  ovr_GetTextureSwapChainBufferGL(session_, swap_chain_, -1, &textureHandle);

  glBindFramebuffer(GL_FRAMEBUFFER, handle_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);

  glViewport(0, 0, width_, height_);
}

void OculusEye::updateViewProjection(const ovrPosef pose)
{
  OVR::Vector3f shiftedEyePos = Config::offset + pose.Position;

  OVR::Matrix4f orientation = OVR::Matrix4f(pose.Orientation);
  OVR::Vector3f finalUp = orientation.Transform(OVR::Vector3f(0, 1, 0));
  OVR::Vector3f finalForward = orientation.Transform(OVR::Vector3f(0, 0, -1));

  OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
  const float near_clipping = 0.01f, far_clipping = 100.0f;
  OVR::Matrix4f proj = ovrMatrix4f_Projection(render_desc_.Fov, near_clipping, far_clipping, ovrProjection_ClipRangeOpenGL);

  // Copy ovr to glm and switch matrix major
  for (auto x = 0; x < 4; x++)
  {
    for (auto y = 0; y < 4; y++)
    {
      view_[x][y] = view.M[y][x];
      projection_[x][y] = proj.M[y][x];
    }
  }

  // Flip projection to compensate for oculus displays
  projection_ = glm::scale(projection_, glm::vec3(1.0f, -1.0f, 1.0f));
}

void OculusEye::endFrame()
{
  ovr_CommitTextureSwapChain(session_, swap_chain_);
}
