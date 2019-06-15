#include "Oculus.h"
#include "Extras/OVR_StereoProjection.h"
#include "Logger.h"
#include "src\Config.h"

void OvrLogCallback(uintptr_t userData, int level, const char* message)
{
  if(level == ovrLogLevel_Error && std::string(message).rfind("Prediction interval too high:") != 0)
  {
    Logger::error(message);
  } else
  {
    Logger::info(message);
  }
}

Oculus::Oculus()
{
  ovrInitParams initParams = { 0, 0, OvrLogCallback, 0, 0 };
  auto result = ovr_Initialize(&initParams);
  if (!OVR_SUCCESS(result))
  {
    Logger::error("Ovr initialization failed :" + std::to_string(result));
  }

  ovr_Create(&session_, &luid_);

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
  // Free swap chains before shutting session down
  Left.reset();
  Right.reset();

  ovr_Destroy(session_);
  ovr_Shutdown();
}

void Oculus::WaitToBeginFrame(long long frameIndex)
{
  ovr_WaitToBeginFrame(session_, frameIndex);
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

  ovr_BeginFrame(session_, frameIndex);
}

void Oculus::EndFrame(long long frameIndex)
{
  Left->endFrame();
  Right->endFrame();

  const ovrLayerHeader* header = &layer_.Header;
  const auto numLayers = 1;
  ovr_EndFrame(session_, frameIndex, nullptr, &header, numLayers);
}

glm::vec3 Oculus::GetActiveHand(long long frameIndex)
{
  auto trackState = ovr_GetTrackingState(session_, ovr_GetPredictedDisplayTime(session_, frameIndex), ovrTrue);
  auto position = trackState.HandPoses[0].ThePose.Position;
  auto origin = trackState.CalibratedOrigin.Position;
  return glm::vec3(position.x + origin.x + Config::offset.x, position.y + origin.y + Config::offset.y, position.z + origin.z + Config::offset.z);
}
