#include <iostream>
#include <src/Config.h>
#include "Renderer.h"
#include "glm/ext.hpp"

Renderer::Renderer(std::shared_ptr<Window> window) :
    window_(window),
    shadow_map_shader_("ShadowMap.vert", "ShadowMap.frag"),
    forward_shader_("Forward.vert", "Forward.frag"),
    deferred_prepare_shader_("DeferredPrepare.vert", "DeferredPrepare.frag"),
    deferred_render_shader_("Quad.vert", "DeferredRender.frag"),
    light_position_(0, 2, 0),
    // Sets x, y, and z to a/2 + 1/2. Moving it from [-1,1] to [0,1].
    window_matrix_(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    ),
    // Setup 2:1 shadow map with depth buffer and 50° fov
    depth_projection_matrix_(glm::perspective<float>(glm::radians(50.0f), 2.0f / 1.0f, 1.0f, 4.0f)),
    depth_view_matrix_(glm::lookAt(light_position_, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0))),
    shadow_map_framebuffer_(2048, 1024, 1, true, SamplingMode::Linear, Precision::Pos16, "shadowmap"),
    deferred_framebuffer_(window->width, window->height, 4, true, SamplingMode::Nearest, Precision::Float32, "deferred"),
    quad_(ObjLoader::createQuad()) {
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Renderer::renderFrame(Game &game) {
  renderShadowMap(game);
  if (Config::forward_rendering){
    renderForward(game);
  } else {
    renderDeferred(game);
  }
  glfwSwapBuffers(window_->handle);
}

void Renderer::renderShadowMap(Game &game) {
  shadow_map_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  shadow_map_shader_.use();

  for (auto &entity: game.entities) {
    auto depth_model_view_projection = depth_projection_matrix_ * depth_view_matrix_ * entity->model;
    glUniformMatrix4fv(shadow_map_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &depth_model_view_projection[0][0]);

    entity->shape->bindVertexOnly();
    entity->shape->draw();
  }

  glDisable(GL_DEPTH_TEST);

  // TODO generate mipmaps after blurring
  // glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::renderForward(Game &game) {
  window_->bindAsFramebuffer();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  forward_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto depth_window_model_view_projection = window_matrix_ * depth_projection_matrix_ * depth_view_matrix_ * entity->model;

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
    entity->shape->draw();
  }
}

void Renderer::renderDeferred(Game &game) {
  deferred_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  deferred_prepare_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto velocity_cameraspace = model_view_projection * glm::vec4(entity->getVelocity(), 0);

    glUniformMatrix4fv(deferred_prepare_shader_.getUniform("u.model"), 1, GL_FALSE, &entity->model[0][0]);
    glUniformMatrix4fv(deferred_prepare_shader_.getUniform("u.model_view_projection"),
                       1,
                       GL_FALSE,
                       &model_view_projection[0][0]);
    glUniform2f(deferred_prepare_shader_.getUniform("u.velocity_cameraspace"),
                velocity_cameraspace.x,
                velocity_cameraspace.y);
    glUniform1i(deferred_prepare_shader_.getUniform("u.object_id"), entity->id);

    entity->shape->bind();
    entity->shape->draw();
  }

  window_->bindAsFramebuffer();
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  deferred_render_shader_.use();

  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection_window = window_matrix_ * depth_projection_matrix_ * depth_view_matrix_;

  glUniformMatrix4fv(deferred_render_shader_.getUniform("u.view"), 1, GL_FALSE, &game.camera.view[0][0]);
  glUniformMatrix4fv(deferred_render_shader_.getUniform("u.view_projection"),
                     1,
                     GL_FALSE,
                     &view_projection[0][0]);
  glUniformMatrix4fv(deferred_render_shader_.getUniform("u.depth_view_projection_window"),
                     1,
                     GL_FALSE,
                     &depth_view_projection_window[0][0]);
  glUniform3f(deferred_render_shader_.getUniform("u.light_position_worldspace"),
              light_position_.x,
              light_position_.y,
              light_position_.z);

  deferred_render_shader_.bind(deferred_framebuffer_.textures[0], "u_deferred_0", 0);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[1], "u_deferred_1", 1);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[2], "u_deferred_2", 2);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[3], "u_deferred_3", 3);
  deferred_render_shader_.bind(shadow_map_framebuffer_.textures[0], "u_shadow_map", 4);
  for (auto &entity: game.entities) {
    auto texture_name = "u_color_texture[" + std::to_string(entity->id) + "]";
    deferred_render_shader_.bind(entity->texture, texture_name.c_str(), 5 + entity->id);
  }
  // TODO setup uniforms (textures, light, matrices etc.)

  quad_->bindVertexOnly();
  quad_->draw();
}
