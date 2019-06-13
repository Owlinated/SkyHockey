#include "Oculus.h"
#include "Extras/OVR_StereoProjection.h"

Oculus::Oculus()
{
  OVR_ASSERT(ovr_Initialize(nullptr), "Ovr initialization failed");
  OVR_ASSERT(ovr_Create(&session_, &luid_), "Ovr session create failed");

  // Set up render targets
  Left = std::make_shared<OculusEye>(session_, ovrEye_Left);
  Right = std::make_shared<OculusEye>(session_, ovrEye_Right);

  // Set up oculus output layer
  layer_.Header.Type = ovrLayerType_EyeFov;
  layer_.Header.Flags = 0;
  layer_.ColorTexture[0] = Left->swap_chain_;
  layer_.ColorTexture[1] = Right->swap_chain_;
  layer_.Fov[0] = Left->render_desc_.Fov;
  layer_.Fov[1] = Right->render_desc_.Fov;
  layer_.Viewport[0] = OVR::Recti(0, 0, Left->getWidth(), Left->getHeight());
  layer_.Viewport[1] = OVR::Recti(0, 0, Right->getWidth(), Right->getHeight());
}

Oculus::~Oculus()
{
  ovr_Destroy(session_);
  ovr_Shutdown();
}

void Oculus::WaitToBeginFrame(long long frameIndex)
{
  OVR_ASSERT(ovr_WaitToBeginFrame(session_, frameIndex), "Ovr wait to begin frame failed");
}

void Oculus::BeginFrame(long long frameIndex)
{
  // Get both eye poses simultaneously, with IPD offset already included.
  auto displayMidpointSeconds = ovr_GetPredictedDisplayTime(session_, 0);
  auto hmdState = ovr_GetTrackingState(session_, displayMidpointSeconds, ovrTrue);
  ovrPosef_ hmdToEyeViewPose[] = {
    Left->render_desc_.HmdToEyePose,
    Right->render_desc_.HmdToEyePose
  };

  ovr_CalcEyePoses(hmdState.HeadPose.ThePose, hmdToEyeViewPose, layer_.RenderPose);
  Left->updateViewProjection(layer_.RenderPose[Left->eye_]);
  Right->updateViewProjection(layer_.RenderPose[Right->eye_]);

  OVR_ASSERT(ovr_BeginFrame(session_, frameIndex), "Ovr begin frame failed");
}

void Oculus::EndFrame(long long frameIndex)
{
  OVR_ASSERT(ovr_CommitTextureSwapChain(session_, Left->swap_chain_), "Ovr commit left swap chain failed");
  OVR_ASSERT(ovr_CommitTextureSwapChain(session_, Right->swap_chain_), "Ovr commit right swap chain failed");

  const ovrLayerHeader* header = &layer_.Header;
  OVR_ASSERT(ovr_EndFrame(session_, frameIndex, nullptr, &header, 1), "Ovr end frame failed");
}
