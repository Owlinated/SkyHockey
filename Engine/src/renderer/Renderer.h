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

class Renderer {
 private:
  std::shared_ptr<Window> window_;
  Shader depth_shader_, shadow_shader_;
  Framebuffer depth_framebuffer_;

  glm::vec3 light_position_;
  glm::mat4 bias_matrix, depth_projection_matrix_, depth_view_matrix_;
 public:
  Renderer(std::shared_ptr<Window> window);
  void renderFrame(Game &game);
};

#endif //ENGINE_RENDER_H
