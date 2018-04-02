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
#include "src/support/Window.h"
#include "RenderEntity.h"

class Renderer {
 private:
  Window &window_;
  Camera &camera_;
  Shader depth_shader_, shadow_shader_;
  std::shared_ptr<Texture> shadow_texture_;
  GLuint shadow_framebuffer_;

  glm::vec3 light_inv_direction_;
  glm::mat4 bias_matrix, depth_projection_matrix_, depth_view_matrix_;

  void renderShadow(RenderEntity& entity);
  void renderScreen(RenderEntity& entity);
 public:
  Renderer();
  void renderFrame(Game &game);
};

#endif //ENGINE_RENDER_H
