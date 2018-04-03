#include <iostream>
#include "Renderer.h"
#include "glm/ext.hpp"

Renderer::Renderer(std::shared_ptr<Window> window) :
    window_(window),
    depth_shader_("Depth.vert", "Depth.frag"),
    shadow_shader_("ShadowMapping.vert", "ShadowMapping.frag"),
    light_inv_direction_(0, 2, 0),
    // Maps to view space, adds depth bias
    bias_matrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.49, 0.0,
        0.5, 0.5, 0.5, 1.0
    ),
    depth_projection_matrix_(glm::perspective<float>(glm::radians(70.0f), 1.0f, 1.0f, 4.0f)),
    depth_view_matrix_(glm::lookAt(light_inv_direction_, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0))),
    depth_framebuffer_(1024, 1024, true, false) {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Renderer::renderFrame(Game &game) {
  depth_framebuffer_.bind();
  glViewport(0, 0, 1024, 1024);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  depth_shader_.use();

  for (auto &entity: game.entities) {
    entity->depth_model_view_projection = depth_projection_matrix_ * depth_view_matrix_ * entity->model;
    glUniformMatrix4fv(depth_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &entity->depth_model_view_projection[0][0]);

    entity->shape->bindShadow();
    glDrawElements(GL_TRIANGLES, entity->shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
  }

  // TODO generate mipmaps after blurring
  // glGenerateMipmap(GL_TEXTURE_2D);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_->width, window_->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shadow_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto depth_bias_model_view_projection = bias_matrix * entity->depth_model_view_projection;

    glUniformMatrix4fv(shadow_shader_.getUniform("u.model"), 1, GL_FALSE, &entity->model[0][0]);
    glUniformMatrix4fv(shadow_shader_.getUniform("u.view"), 1, GL_FALSE, &game.camera.view[0][0]);
    glUniformMatrix4fv(shadow_shader_.getUniform("u.model_view_projection"), 1, GL_FALSE, &model_view_projection[0][0]);
    glUniformMatrix4fv(shadow_shader_.getUniform("u.depth_bias_model_view_projection"),
                       1,
                       GL_FALSE,
                       &depth_bias_model_view_projection[0][0]);
    glUniform3f(shadow_shader_.getUniform("u.light_inv_direction_worldspace"),
                light_inv_direction_.x,
                light_inv_direction_.y,
                light_inv_direction_.z);
    shadow_shader_.bind(entity->texture, "u_color_texture", 0);
    shadow_shader_.bind(depth_framebuffer_.texture, "u_shadow_map", 1);

    entity->shape->bind();
    glDrawElements(GL_TRIANGLES, entity->shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
  }

  glfwSwapBuffers(window_->handle);
}
