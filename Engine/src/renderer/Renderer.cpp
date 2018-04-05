#include <iostream>
#include "Renderer.h"
#include "glm/ext.hpp"

Renderer::Renderer(std::shared_ptr<Window> window) :
    window_(window),
    shadow_map_shader_("ShadowMap.vert", "ShadowMap.frag"),
    forward_shader_("Forward.vert", "Forward.frag"),
    deferred_shader_("DeferredPrepare.vert", "DeferredPrepare.frag"),
    light_position_(0, 2, 0),
    // Sets x, y, and z to a/2 + 1/2. Moving it from [-1,1] to [0,1].
    window_matrix_(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    ),
    depth_projection_matrix_(glm::perspective<float>(glm::radians(50.0f), 2.0f / 1.0f, 1.0f, 4.0f)),
    depth_view_matrix_(glm::lookAt(light_position_, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0))),
    shadow_map_framebuffer_(2048, 1024, 1, true, false, "shadowmap"),
    deferred_framebuffer_(window->width, window->height, 4, true, false, "deferred") {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Renderer::renderShadowMap(Game &game) {
  shadow_map_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shadow_map_shader_.use();

  for (auto &entity: game.entities) {
    entity->depth_model_view_projection = depth_projection_matrix_ * depth_view_matrix_ * entity->model;
    glUniformMatrix4fv(shadow_map_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &entity->depth_model_view_projection[0][0]);

    entity->shape->bindShadow();
    glDrawElements(GL_TRIANGLES, entity->shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
  }

  // TODO generate mipmaps after blurring
  // glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::renderForward(Game &game) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_->width, window_->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  forward_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto depth_window_model_view_projection = window_matrix_ * entity->depth_model_view_projection;

    glUniformMatrix4fv(forward_shader_.getUniform("u.model"), 1, GL_FALSE, &entity->model[0][0]);
    glUniformMatrix4fv(forward_shader_.getUniform("u.view"), 1, GL_FALSE, &game.camera.view[0][0]);
    glUniformMatrix4fv(forward_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &model_view_projection[0][0]);
    glUniformMatrix4fv(forward_shader_.getUniform("u.depth_window_model_view_projection"),
                       1,
                       GL_FALSE,
                       &depth_window_model_view_projection[0][0]);
    glUniform3f(forward_shader_.getUniform("u.light_position_worldspace"),
                light_position_.x,
                light_position_.y,
                light_position_.z);
    forward_shader_.bind(entity->texture, "u_color_texture", 0);
    forward_shader_.bind(shadow_map_framebuffer_.textures[0], "u_shadow_map", 1);

    entity->shape->bind();
    glDrawElements(GL_TRIANGLES, entity->shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
  }
}

void Renderer::renderDeferred(Game &game) {
  deferred_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  deferred_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto velocity_cameraspace = model_view_projection * glm::vec4(entity->getVelocity(), 0);

    glUniformMatrix4fv(deferred_shader_.getUniform("u.model"), 1, GL_FALSE, &entity->model[0][0]);
    glUniformMatrix4fv(deferred_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &model_view_projection[0][0]);
    glUniform2f(deferred_shader_.getUniform("u.velocity_cameraspace"),
                velocity_cameraspace.x,
                velocity_cameraspace.y);
    glUniform1i(deferred_shader_.getUniform("u.object_id"), entity->id);

    entity->shape->bind();
    glDrawElements(GL_TRIANGLES, entity->shape->index_count(), GL_UNSIGNED_SHORT, nullptr);
  }
}

void Renderer::renderFrame(Game &game) {
  renderShadowMap(game);
  renderDeferred(game);
  renderForward(game);
  glfwSwapBuffers(window_->handle);
}
