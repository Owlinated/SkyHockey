#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include <src/renderer/material/Shader.h>
#include <src/renderer/geometry/Shape.h>
#include <src/renderer/geometry/ObjLoader.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/game/Camera.h>
#include <src/renderer/material/Texture.h>
#include <src/game/Game.h>
#include <src/renderer/material/Framebuffer.h>
#include <src/renderer/material/Light.h>
#include "src/support/Window.h"
#include "RenderEntity.h"

class Renderer {
 private:
  int shadow_width_, shadow_height_;
  float shadow_clip_near_, shadow_clip_far_;
  float total_time_;

  std::shared_ptr<Window> window_;
  Framebuffer shadow_map_framebuffer_, deferred_framebuffer_,
      motion_blur_framebuffer_, horizontal_blur_framebuffer_, vertical_blur_framebuffer_;
  Light light_;

  glm::mat4 window_matrix_, depth_projection_matrix_, depth_view_matrix_;

  void renderForward(Game &game, float delta_time);
  void renderDeferred(Game &game, float delta_time);
  static void renderShadowMap(Game &game,
                              Framebuffer &output,
                              glm::mat4 depth_view_projection,
                              float depth_attenuation,
                              Framebuffer &horizontal_blur_framebuffer,
                              Framebuffer &vertical_blur_framebuffer);
  static void renderBackground(Game &game, IFramebuffer &output, float deltaTime);
  static void renderMotionBlur(std::shared_ptr<Texture> &color, std::shared_ptr<Texture> &velocity,
                               IFramebuffer &output);
  static void renderBidirectionalBlur(std::shared_ptr<Texture> &color,
                                      IFramebuffer &intermediate,
                                      std::shared_ptr<Texture> &intermediate_color,
                                      IFramebuffer &output);
  void renderPerfOverlay(IFramebuffer &output, float total_time);
 public:
  explicit Renderer(std::shared_ptr<Window> window);
  void renderFrame(Game &game, float delta_time);
};

#endif //ENGINE_RENDER_H
