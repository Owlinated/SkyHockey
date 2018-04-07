#include <iostream>
#include <src/Config.h>
#include "Renderer.h"
#include "glm/ext.hpp"

Renderer::Renderer(std::shared_ptr<Window> window) :
    window_(window),
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
    motion_blur_framebuffer_(window->width, window->height, 1, false, SamplingMode::Nearest, Precision::Pos16, "motion_blur"),
    horizontal_blur_framebuffer_(window->width, window->height, 1, false, SamplingMode::Nearest, Precision::Pos16, "horizontal_blur"),
    vertical_blur_framebuffer_(window->width, window->height, 1, false, SamplingMode::Nearest, Precision::Pos16, "vertical_blur") {
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Renderer::renderFrame(Game &game) {
  if (Config::forward_rendering){
    renderForward(game);
  } else {
    renderDeferred(game);
  }
  glfwSwapBuffers(window_->handle);
}

void Renderer::renderForward(Game &game) {
  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection = depth_projection_matrix_ * depth_view_matrix_;
  auto depth_view_projection_window = window_matrix_ * depth_view_projection;

  renderShadowMap(game, shadow_map_framebuffer_, depth_view_projection);

  renderBackground(game, *window_);

  window_->bind();
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  static Shader forward_shader("Forward.vert", "Forward.frag");
  forward_shader.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = view_projection * entity->model;
    auto depth_model_view_projection_window = depth_view_projection_window * entity->model;

    forward_shader.bind(entity->model, "u.model");
    forward_shader.bind(game.camera.view, "u.view");
    forward_shader.bind(model_view_projection, "u.model_view_projection");
    forward_shader.bind(depth_model_view_projection_window, "u.depth_model_view_projection_window");

    forward_shader.bind(light_.position_worldspace, "u.light.position_worldspace");
    forward_shader.bind(light_.color, "u.light.color");
    forward_shader.bind(entity->material.ambient_multiplier,  "u.material.ambient_multiplier");
    forward_shader.bind(entity->material.diffuse_multiplier, "u.material.diffuse_multiplier");
    forward_shader.bind(entity->material.specular_multiplier, "u.material.specular_multiplier");

    forward_shader.bind(entity->texture, "u_color_texture", 0);
    forward_shader.bind(shadow_map_framebuffer_.textures[0], "u_shadow_map", 1);

    entity->shape->bind();
    entity->shape->draw();
  }
}

void Renderer::renderDeferred(Game &game) {
  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection = depth_projection_matrix_ * depth_view_matrix_;
  auto depth_view_projection_window = window_matrix_ * depth_view_projection;

  renderShadowMap(game, shadow_map_framebuffer_, depth_view_projection);

  deferred_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  static Shader deferred_prepare_shader("DeferredPrepare.vert", "DeferredPrepare.frag");
  deferred_prepare_shader.use();

  for (auto &entity: game.entities) {
    auto model_view_projection = view_projection * entity->model;
    auto velocity_cameraspace = glm::vec2(model_view_projection * glm::vec4(entity->getVelocity(), 0));

    deferred_prepare_shader.bind(entity->model, "u.model");
    deferred_prepare_shader.bind(model_view_projection, "u.model_view_projection");
    deferred_prepare_shader.bind(velocity_cameraspace, "u.velocity_cameraspace");
    deferred_prepare_shader.bind(entity->id, "u.object_id");

    entity->shape->bind();
    entity->shape->draw();
  }

  // TODO merge space shader into deferred render
  renderBackground(game, *window_);

  static Shader deferred_render_shader("Quad.vert", "DeferredRender.frag");
  deferred_render_shader.use();
  deferred_render_shader.bind(game.camera.view, "u.view");
  deferred_render_shader.bind(view_projection, "u.view_projection");
  deferred_render_shader.bind(depth_view_projection_window, "u.depth_view_projection_window");
  deferred_render_shader.bind(light_.position_worldspace, "u.light.position_worldspace");
  deferred_render_shader.bind(light_.color, "u.light.color");

  deferred_render_shader.bind(deferred_framebuffer_.textures[0], "u_deferred_0", 0);
  deferred_render_shader.bind(deferred_framebuffer_.textures[1], "u_deferred_1", 1);
  deferred_render_shader.bind(deferred_framebuffer_.textures[2], "u_deferred_2", 2);
  deferred_render_shader.bind(shadow_map_framebuffer_.textures[0], "u_shadow_map", 4);

  for (auto &entity: game.entities) {
    auto texture_name = "u_color_texture[" + std::to_string(entity->id) + "]";
    deferred_render_shader.bind(entity->texture, texture_name.c_str(), 5 + entity->id);

    auto material_name = "u.materials[" + std::to_string(entity->id) + "].";
    deferred_render_shader.bind(entity->material.ambient_multiplier, (material_name + "ambient_multiplier").c_str());
    deferred_render_shader.bind(entity->material.diffuse_multiplier, (material_name + "diffuse_multiplier").c_str());
    deferred_render_shader.bind(entity->material.specular_multiplier, (material_name + "specular_multiplier").c_str());
  }

  motion_blur_framebuffer_.bind();
  glDisable(GL_DEPTH_TEST);
  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  quad->draw();

  //renderMotionBlur(motion_blur_framebuffer_.textures[0], deferred_framebuffer_.textures[3], *window_);

  // TODO Debug only
  renderMotionBlur(motion_blur_framebuffer_.textures[0], deferred_framebuffer_.textures[3], horizontal_blur_framebuffer_);
  renderBidirectionalBlur(horizontal_blur_framebuffer_.textures[0], vertical_blur_framebuffer_,
                          vertical_blur_framebuffer_.textures[0], *window_);
}

void Renderer::renderShadowMap(Game &game, IFramebuffer &output, glm::mat4 depth_view_projection) {
  static Shader shadow_map_shader("ShadowMap.vert", "ShadowMap.frag");
  shadow_map_shader.use();

  output.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (auto &entity: game.entities) {
    auto depth_model_view_projection = depth_view_projection * entity->model;
    shadow_map_shader.bind(depth_model_view_projection, "u.model_view_projection");

    entity->shape->bindVertexOnly();
    entity->shape->draw();
  }

  glDisable(GL_DEPTH_TEST);

  // TODO generate mipmaps after blurring
  // glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::renderBackground(Game &game, IFramebuffer &output) {
  if(!Config::fancy_background) {
    output.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  auto currentFrame = static_cast<float>(glfwGetTime());
  auto resolution = glm::vec3(output.getWidth(), output.getHeight(), 0);
  auto a = glm::fastLog(1.0f + glm::fastLength(game.striker_player->velocity)) * 0.1f;
  auto b = glm::fastLog(1.0f + glm::fastLength(game.puck->velocity)) * 0.3f;
  auto c = glm::fastLog(1.0f + glm::fastLength(game.striker_opponent->velocity)) * 0.1f;

  static Shader space_shader("Quad.vert", "Space.frag");
  space_shader.use();
  space_shader.bind(currentFrame, "u.time");
  space_shader.bind(resolution, "u.resolution");
  space_shader.bind(1.0f, "u.frequencies[0]");
  space_shader.bind(a, "u.frequencies[1]");
  space_shader.bind(b, "u.frequencies[2]");
  space_shader.bind(c, "u.frequencies[3]");

  output.bind();
  glDisable(GL_DEPTH_TEST);
  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  quad->draw();
}

void Renderer::renderMotionBlur(std::shared_ptr<Texture> &color, std::shared_ptr<Texture> &velocity, IFramebuffer &output) {
  static Shader motion_blur_shader("Quad.vert", "BlurMotion.frag");
  motion_blur_shader.use();
  motion_blur_shader.bind(8, "u.sample_count");
  motion_blur_shader.bind(0.2f, "u.step_size");
  motion_blur_shader.bind(color, "u_color_texture", 0);
  motion_blur_shader.bind(velocity, "u_velocity_map", 1);

  output.bind();
  glDisable(GL_DEPTH_TEST);

  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  quad->draw();
}

void Renderer::renderBidirectionalBlur(std::shared_ptr<Texture> &color,
                                       IFramebuffer &intermediate,
                                       std::shared_ptr<Texture> &intermediate_color,
                                       IFramebuffer &output) {
  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  glDisable(GL_DEPTH_TEST);

  static Shader horizontal_blur_shader("Quad.vert", "BlurHorizontal.frag");
  horizontal_blur_shader.use();
  horizontal_blur_shader.bind(8, "u.sample_count");
  horizontal_blur_shader.bind(1.0f, "u.step_size");
  horizontal_blur_shader.bind(color, "u_color_texture", 0);

  intermediate.bind();
  quad->draw();

  static Shader vertical_blur_shader("Quad.vert", "BlurVertical.frag");
  horizontal_blur_shader.use();
  horizontal_blur_shader.bind(8, "u.sample_count");
  horizontal_blur_shader.bind(1.0f, "u.step_size");
  horizontal_blur_shader.bind(intermediate_color, "u_color_texture", 0);

  output.bind();
  quad->draw();
}
