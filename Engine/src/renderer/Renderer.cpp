#include <iostream>
#include "Renderer.h"
#include "glm/ext.hpp"
#include "VisualDebug.h"

// Render hidden frame buffers
//#define VISUAL_DEBUG

Renderer::Renderer() : window_(Window::getInstance()),
                       camera_(Camera::getInstance()),
                       depth_shader_("DepthRTT.vert", "DepthRTT.frag"),
                       shadow_shader_("ShadowMapping.vert", "ShadowMapping.frag"),
                       light_inverse_direction_(0, 2, 0),
                       bias_matrix(
                           0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0,
                           0.0, 0.0, 0.5, 0.0,
                           0.5, 0.5, 0.5, 1.0
                       ),
                       depth_projection_matrix_(glm::ortho<float>(-5, 5, -5, 5, 0.1, 5)),
                       depth_view_matrix_(glm::lookAt(light_inverse_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) {
  // or, for spot light :
  //glm::vec3 lightPos(5, 20, 20);
  //glm::mat4 depth_projection_matrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
  //glm::mat4 depth_view_matrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));

  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  glGenFramebuffers(1, &shadow_framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer_);
  shadow_texture_ = Texture::createDepthTexture(1024, 1024);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_texture_->handle(), 0);
  glDrawBuffer(GL_NONE);
}

void Renderer::render_shadow(RenderEntity entity) {
  entity.depth_model_view_projection = depth_projection_matrix_ * depth_view_matrix_ * entity.model;
  glUniformMatrix4fv(depth_shader_.getUniform("depthMVP"), 1, GL_FALSE, &entity.depth_model_view_projection[0][0]);

  entity.shape->bindShadow();
  glDrawElements(GL_TRIANGLES, entity.shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
}

void Renderer::render_screen(RenderEntity entity) {
  auto model_view_projection = camera_.projection * camera_.view * entity.model;
  auto depth_bias_model_view_projection = bias_matrix * entity.depth_model_view_projection;

  glUniformMatrix4fv(shadow_shader_.getUniform("MVP"), 1, GL_FALSE, &model_view_projection[0][0]);
  glUniformMatrix4fv(shadow_shader_.getUniform("M"), 1, GL_FALSE, &entity.model[0][0]);
  glUniformMatrix4fv(shadow_shader_.getUniform("V"), 1, GL_FALSE, &camera_.view[0][0]);
  glUniformMatrix4fv(shadow_shader_.getUniform("DepthBiasMVP"), 1, GL_FALSE, &depth_bias_model_view_projection[0][0]);
  glUniform3f(shadow_shader_.getUniform("LightInvDirection_worldspace"), light_inverse_direction_.x, light_inverse_direction_.y, light_inverse_direction_.z);
  shadow_shader_.bind(entity.texture, const_cast<char *>("myTextureSampler"), 0);
  shadow_shader_.bind(shadow_texture_, const_cast<char *>("shadowMap"), 1);

  entity.shape->bind();
  glDrawElements(GL_TRIANGLES, entity.shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
}

void Renderer::renderFrame(Game &game) {
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer_);
  glViewport(0, 0, 1024, 1024);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  depth_shader_.use();

  for (auto &entity: game.entities) {
    render_shadow(*entity); // NOLINT
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_.width(), window_.height());
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shadow_shader_.use();

  for (auto &entity: game.entities) {
    render_screen(*entity); // NOLINT
  }

#ifdef VISUAL_DEBUG
  VisualDebug::getInstance().pictureInPicture(shadow_texture_);
#endif

  glfwSwapBuffers(window_.handle());
}
