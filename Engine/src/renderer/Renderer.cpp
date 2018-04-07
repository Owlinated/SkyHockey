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
    space_shader_("Quad.vert", "Space.frag"),
    // Sets x, y, and z to a/2 + 1/2. Moving them from [-1,1] to [0,1].
    window_matrix_(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    ),
    // Setup 2:1 shadow map with depth buffer and 50° fov
    depth_projection_matrix_(glm::perspective<float>(glm::radians(50.0f), 2.0f / 1.0f, 1.0f, 4.0f)),
    depth_view_matrix_(glm::lookAt(light_.position_worldspace, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0))),
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
    shadow_map_shader_.bind(depth_model_view_projection, "u.model_view_projection");

    entity->shape->bindVertexOnly();
    entity->shape->draw();
  }

  glDisable(GL_DEPTH_TEST);

  // TODO generate mipmaps after blurring
  // glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::renderForward(Game &game) {
  renderBackground(game, *window_);
  window_->bind();
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  forward_shader_.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = game.camera.projection * game.camera.view * entity->model;
    auto depth_model_view_projection_window = window_matrix_ * depth_projection_matrix_ * depth_view_matrix_ * entity->model;

    forward_shader_.bind(entity->model, "u.model");
    forward_shader_.bind(game.camera.view, "u.view");
    forward_shader_.bind(model_view_projection, "u.model_view_projection");
    forward_shader_.bind(depth_model_view_projection_window, "u.depth_model_view_projection_window");

    forward_shader_.bind(light_.position_worldspace, "u.light.position_worldspace");
    forward_shader_.bind(light_.color, "u.light.color");
    forward_shader_.bind(entity->material.ambient_multiplier,  "u.material.ambient_multiplier");
    forward_shader_.bind(entity->material.diffuse_multiplier, "u.material.diffuse_multiplier");
    forward_shader_.bind(entity->material.specular_multiplier, "u.material.specular_multiplier");

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
    auto velocity_cameraspace = glm::vec2(model_view_projection * glm::vec4(entity->getVelocity(), 0));

    deferred_prepare_shader_.bind(entity->model, "u.model");
    deferred_prepare_shader_.bind(model_view_projection, "u.model_view_projection");
    deferred_prepare_shader_.bind(velocity_cameraspace, "u.velocity_cameraspace");
    deferred_prepare_shader_.bind(entity->id, "u.object_id");

    entity->shape->bind();
    entity->shape->draw();
  }

  // TODO merge space shader into deferred render
  renderBackground(game, *window_);

  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection_window = window_matrix_ * depth_projection_matrix_ * depth_view_matrix_;

  deferred_render_shader_.use();
  deferred_render_shader_.bind(game.camera.view, "u.view");
  deferred_render_shader_.bind(view_projection, "u.view_projection");
  deferred_render_shader_.bind(depth_view_projection_window, "u.depth_view_projection_window");
  deferred_render_shader_.bind(light_.position_worldspace, "u.light.position_worldspace");
  deferred_render_shader_.bind(light_.color, "u.light.color");

  deferred_render_shader_.bind(deferred_framebuffer_.textures[0], "u_deferred_0", 0);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[1], "u_deferred_1", 1);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[2], "u_deferred_2", 2);
  deferred_render_shader_.bind(deferred_framebuffer_.textures[3], "u_deferred_3", 3);
  deferred_render_shader_.bind(shadow_map_framebuffer_.textures[0], "u_shadow_map", 4);

  for (auto &entity: game.entities) {
    auto texture_name = "u_color_texture[" + std::to_string(entity->id) + "]";
    deferred_render_shader_.bind(entity->texture, texture_name.c_str(), 5 + entity->id);

    auto material_name = "u.materials[" + std::to_string(entity->id) + "].";
    deferred_render_shader_.bind(entity->material.ambient_multiplier, (material_name + "ambient_multiplier").c_str());
    deferred_render_shader_.bind(entity->material.diffuse_multiplier, (material_name + "diffuse_multiplier").c_str());
    deferred_render_shader_.bind(entity->material.specular_multiplier, (material_name + "specular_multiplier").c_str());
  }

  window_->bind();
  glDisable(GL_DEPTH_TEST);
  quad_->bindVertexOnly();
  quad_->draw();
}

void Renderer::renderBackground(Game &game, IFramebuffer &framebuffer) {
  if(!Config::fancy_background) {
    framebuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  auto currentFrame = static_cast<float>(glfwGetTime());
  auto resolution = glm::vec3(framebuffer.getWidth(), framebuffer.getHeight(), 0);
  //auto frequencies = glm::vec4(1.0, 0.2, 0.2, 0.2);
  auto a = glm::fastLog(1.0f + glm::fastLength(game.striker_player->velocity)) * 0.1f;
  auto b = glm::fastLog(1.0f + glm::fastLength(game.puck->velocity)) * 0.3f;
  auto c = glm::fastLog(1.0f + glm::fastLength(game.striker_opponent->velocity)) * 0.1f;

  space_shader_.use();
  space_shader_.bind(currentFrame, "u.time");
  space_shader_.bind(resolution, "u.resolution");
  space_shader_.bind(1.0f, "u.frequencies[0]");
  space_shader_.bind(a, "u.frequencies[1]");
  space_shader_.bind(b, "u.frequencies[2]");
  space_shader_.bind(c, "u.frequencies[3]");

  framebuffer.bind();
  glDisable(GL_DEPTH_TEST);
  quad_->bindVertexOnly();
  quad_->draw();
}
