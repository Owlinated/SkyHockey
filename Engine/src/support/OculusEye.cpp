#include "OculusEye.h"
#include "OVR_CAPI.h"
#include "OVR_CAPI_GL.h"

#include "Logger.h"
#include "Extras/OVR_Math.h"
#include <glm/gtc/type_ptr.hpp>


OculusEye::OculusEye(ovrSession session, ovrEyeType_ eye) : session_(session), eye_(eye)
{
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
}

void OculusEye::bind()
{
  glBindTexture(GL_TEXTURE_2D, getHandle());
  glViewport(0, 0, width_, height_);
}

GLuint OculusEye::getHandle()
{
  GLuint handle;
  ovr_GetTextureSwapChainBufferGL(session_, swap_chain_, -1, &handle);
  return handle;
}

void OculusEye::updateViewProjection(const ovrPosef pose)
{
  auto originPos = OVR::Vector3f::Zero();
  auto originRot = OVR::Matrix4f::Identity();

  auto pos = originPos + originRot.Transform(pose.Position);
  auto rot = originRot * OVR::Matrix4f(pose.Orientation);

  auto finalUp = rot.Transform(OVR::Vector3f(0, 1, 0));
  auto finalForward = rot.Transform(OVR::Vector3f(0, 0, -1));

  auto view = OVR::Matrix4f::LookAtRH(pos, pos + finalForward, finalUp);
  auto proj = ovrMatrix4f_Projection(render_desc_.Fov, 0.2f, 1000.0f, 0);

  // Copy OVR matrices to glm
  // TODO compute them in glm from the get go
  for(auto x = 0; x < 4; x++)
  {
    for (auto y = 0; y < 4; y++)
    {
      view_[x][y] = view.M[x][y];
      projection_[x][y] = proj.M[x][y];
    }
  }
}
