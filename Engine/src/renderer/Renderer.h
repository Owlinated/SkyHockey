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
#include "src/support/Window.h"
#include "RenderEntity.h"

struct Light {
  Light() : position_worldspace(0, 2, 0), color(1, 1, 1) {}
  glm::vec3 position_worldspace;
  glm::vec3 color;
};

class Renderer {
 private:
  std::shared_ptr<Window> window_;
  Framebuffer shadow_map_framebuffer_, deferred_framebuffer_,
      motion_blur_framebuffer_, horizontal_blur_framebuffer_, vertical_blur_framebuffer_;
  Light light_;

  glm::mat4 window_matrix_, depth_projection_matrix_, depth_view_matrix_;

  void renderForward(Game &game);
  void renderDeferred(Game &game);
  static void renderShadowMap(Game &game, IFramebuffer &output, glm::mat4 depth_view_projection);
  static void renderBackground(Game &game, IFramebuffer &output);
  static void renderMotionBlur(std::shared_ptr<Texture> &color, std::shared_ptr<Texture> &velocity, IFramebuffer &output);
  static void renderBidirectionalBlur(std::shared_ptr<Texture> &color,
                                      IFramebuffer &intermediate,
                                      std::shared_ptr<Texture> &intermediate_color,
                                      IFramebuffer &output);
 public:
  explicit Renderer(std::shared_ptr<Window> window);
  void renderFrame(Game &game);
};

#endif //ENGINE_RENDER_H
