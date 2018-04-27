#include <iostream>
#include <src/Config.h>
#include <src/support/Formatter.h>
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
    shadow_width_(2048),
    shadow_height_(1024),
    shadow_clip_near_(1.0f),
    shadow_clip_far_(4.0f),
    total_time_(0.0f),
    depth_projection_matrix_(glm::perspective<float>(glm::radians(50.0f), shadow_width_ / shadow_height_,
                                                     shadow_clip_near_, shadow_clip_far_)),
    depth_view_matrix_(glm::lookAt(light_.position_worldspace, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0))),
    shadow_map_framebuffer_(
        shadow_width_, shadow_height_, 1, true, SamplingMode::Linear, Precision::Float32, "shadowmap"),
    deferred_framebuffer_(
        window->width, window->height, 4, true, SamplingMode::Nearest, Precision::Float32, "deferred"),
    forward_framebuffer_(
        window->width, window->height, 1, true, SamplingMode::Nearest, Precision::Pos16, "forward"),
    motion_blur_framebuffer_(
        window->width, window->height, 1, false, SamplingMode::Nearest, Precision::Pos16, "motion_blur"),
    horizontal_blur_framebuffer_(
        shadow_width_, shadow_height_, 1, false, SamplingMode::Linear, Precision::Float32, "horizontal_blur"),
    vertical_blur_framebuffer_(
        shadow_width_, shadow_height_, 1, false, SamplingMode::Linear, Precision::Float32, "vertical_blur"),
    aliasing_blur_framebuffer_(
        window->width, window->height, 1, false, SamplingMode::Nearest, Precision::Pos16, "aliasing_blur") {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Renderer::renderFrame(Game &game, float delta_time) {
  total_time_ += delta_time;

  if (Config::fancy_background) {
    renderBackground(game, *window_, total_time_);
  } else {
    window_->bind();
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (Config::forward_rendering){
    renderForward(game, delta_time);
  } else {
    renderDeferred(game, delta_time);
  }

  if (Config::perf_overlay) {
    renderPerfOverlay(*window_, delta_time);
  }

  glfwSwapBuffers(window_->handle);
}

void Renderer::renderForward(Game &game, float delta_time) {
  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection = depth_projection_matrix_ * depth_view_matrix_;
  auto depth_view_projection_window = window_matrix_ * depth_view_projection;
  auto depth_attenuation = shadow_clip_far_ - shadow_clip_near_;

  renderShadowMap(game, shadow_map_framebuffer_, depth_view_projection, depth_attenuation,
                  horizontal_blur_framebuffer_, vertical_blur_framebuffer_);

  forward_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  static Shader forward_shader("Forward.vert", "Forward.frag");
  forward_shader.use();

  for (auto &entity: game.entities) {
    auto model = entity->model();
    auto model_view_projection = view_projection * model;
    auto depth_model_view_projection_window = depth_view_projection_window * model;

    forward_shader.bind(model, "u.model");
    forward_shader.bind(game.camera.view, "u.view");
    forward_shader.bind(model_view_projection, "u.model_view_projection");
    forward_shader.bind(depth_model_view_projection_window, "u.depth_model_view_projection_window");
    forward_shader.bind(depth_attenuation, "u.depth_attenuation");

    forward_shader.bind(light_.position_worldspace, "u.light.position_worldspace");
    forward_shader.bind(light_.color, "u.light.color");
    forward_shader.bind(entity->material.ambient_multiplier,  "u.material.ambient_multiplier");
    forward_shader.bind(entity->material.diffuse_multiplier, "u.material.diffuse_multiplier");
    forward_shader.bind(entity->material.specular_multiplier, "u.material.specular_multiplier");

    forward_shader.bind(entity->texture, "u_color_texture", 0);
    forward_shader.bind(vertical_blur_framebuffer_.textures[0], "u_shadow_map", 1);

    entity->shape->bind();
    entity->shape->draw();
  }

  renderAliasingBlur(forward_framebuffer_.textures[0], *window_);
}

void Renderer::renderDeferred(Game &game, float delta_time) {
  auto view_projection = game.camera.projection * game.camera.view;
  auto depth_view_projection = depth_projection_matrix_ * depth_view_matrix_;
  auto depth_view_projection_window = window_matrix_ * depth_view_projection;
  auto depth_attenuation = shadow_clip_far_ - shadow_clip_near_;

  renderShadowMap(game, shadow_map_framebuffer_, depth_view_projection, depth_attenuation,
                  horizontal_blur_framebuffer_, vertical_blur_framebuffer_);

  deferred_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  static Shader deferred_prepare_shader("DeferredPrepare.vert", "DeferredPrepare.frag");
  deferred_prepare_shader.use();

  for (auto &entity: game.entities) {
    auto model = entity->model();
    auto model_view_projection = view_projection * model;
    auto velocity_cameraspace = glm::vec2(model_view_projection * glm::vec4(entity->velocity, 0));

    deferred_prepare_shader.bind(model, "u.model");
    deferred_prepare_shader.bind(model_view_projection, "u.model_view_projection");
    deferred_prepare_shader.bind(velocity_cameraspace, "u.velocity_cameraspace");
    deferred_prepare_shader.bind(entity->id, "u.object_id");

    deferred_prepare_shader.bind(entity->texture, "u_color_texture", 0);

    entity->shape->bind();
    entity->shape->draw();
  }

  static Shader deferred_render_shader("Quad.vert", "DeferredRender.frag");
  deferred_render_shader.use();
  deferred_render_shader.bind(game.camera.view, "u.view");
  deferred_render_shader.bind(view_projection, "u.view_projection");
  deferred_render_shader.bind(depth_view_projection_window, "u.depth_view_projection_window");
  deferred_render_shader.bind(depth_attenuation, "u.depth_attenuation");
  deferred_render_shader.bind(light_.position_worldspace, "u.light.position_worldspace");
  deferred_render_shader.bind(light_.color, "u.light.color");

  deferred_render_shader.bind(deferred_framebuffer_.textures[0], "u_deferred_0", 0);
  deferred_render_shader.bind(deferred_framebuffer_.textures[1], "u_deferred_1", 1);
  deferred_render_shader.bind(deferred_framebuffer_.textures[2], "u_deferred_2", 2);
  deferred_render_shader.bind(vertical_blur_framebuffer_.textures[0], "u_shadow_map", 4);

  for (auto &entity: game.entities) {
    auto material_name = "u.materials[" + std::to_string(entity->id) + "].";
    deferred_render_shader.bind(entity->material.ambient_multiplier, (material_name + "ambient_multiplier").c_str());
    deferred_render_shader.bind(entity->material.diffuse_multiplier, (material_name + "diffuse_multiplier").c_str());
    deferred_render_shader.bind(entity->material.specular_multiplier, (material_name + "specular_multiplier").c_str());
  }

  motion_blur_framebuffer_.bind();
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  quad->draw();

  renderMotionBlur(motion_blur_framebuffer_.textures[0], deferred_framebuffer_.textures[3], aliasing_blur_framebuffer_);

  renderAliasingBlur(aliasing_blur_framebuffer_.textures[0], *window_);
}

/// Render and blur shadow map. The resulting blurred and mipmapped texture is in vertical_blur_framebuffer
/// \param game Game which objects to render
/// \param output Texture to write shadows to
/// \param depth_view_projection Matrix for transforming world coordinates into shadow cameraspace
/// \param horizontal_blur_framebuffer Buffer to store horizontally blurred image in
/// \param vertical_blur_framebuffer Buffer to store vertically blurred image in
void Renderer::renderShadowMap(Game &game,
                               Framebuffer &output,
                               glm::mat4 depth_view_projection,
                               float depth_attenuation,
                               Framebuffer &horizontal_blur_framebuffer,
                               Framebuffer &vertical_blur_framebuffer) {
  static Shader shadow_map_shader("ShadowMap.vert", "ShadowMap.frag");
  shadow_map_shader.use();

  output.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (auto &entity: game.entities) {
    auto model = entity->model();
    auto depth_model_view_projection = depth_view_projection * model;
    shadow_map_shader.bind(depth_model_view_projection, "u.model_view_projection");
    shadow_map_shader.bind(depth_attenuation, "u.depth_attenuation");

    entity->shape->bindVertexOnly();
    entity->shape->draw();
  }

  glDisable(GL_DEPTH_TEST);
  renderBidirectionalBlur(output.textures[0], horizontal_blur_framebuffer,
                          horizontal_blur_framebuffer.textures[0], vertical_blur_framebuffer);

  vertical_blur_framebuffer.textures[0]->bind();
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::renderBackground(Game &game, IFramebuffer &output, float total_time) {
  auto resolution = glm::vec3(output.getWidth(), output.getHeight(), 0);
  auto a = glm::fastLog(1.0f + glm::fastLength(game.striker_player->velocity)) * 0.1f;
  auto b = glm::fastLog(1.0f + glm::fastLength(game.puck->velocity)) * 0.3f;
  auto c = glm::fastLog(1.0f + glm::fastLength(game.striker_opponent->velocity)) * 0.1f;

  static Shader space_shader("Quad.vert", "Space.frag");
  space_shader.use();
  space_shader.bind(total_time, "u.time");
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

void Renderer::renderMotionBlur(std::shared_ptr<Texture> &color, std::shared_ptr<Texture> &velocity,
                                IFramebuffer &output) {
  static Shader motion_blur_shader("Quad.vert", "BlurMotion.frag");
  motion_blur_shader.use();
  motion_blur_shader.bind(Config::motion_blur_steps, "u.sample_count");
  motion_blur_shader.bind(Config::motion_blur_step_size, "u.step_size");
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
  // Cache gaussian/normal distribution for blurring
  const float sigma = 3.0f, two_sigma_squared = 2 * sigma * sigma;
  static auto cached_size = 0;
  static std::vector<float> weights;
  if(cached_size != Config::shadow_blur_size) {
    cached_size = Config::shadow_blur_size;

    // Simplification: no offset
    auto weight_sum = 0.0f;
    weights.clear();
    for (auto i = 0; i < cached_size; ++i) {
      auto weight = std::exp((i * i) / two_sigma_squared);
      weights.push_back(weight);
      // Since values are summed along two directions, all values but the first are used twice
      weight_sum += (i == 0 ? 1 : 2) * weight;
    }

    // Normalize with discrete sum
    for (auto i = 0; i < cached_size; ++i) {
      weights[i] /= weight_sum;
    }
  }

  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  glDisable(GL_DEPTH_TEST);

  static Shader blur_shader("Quad.vert", "BlurUnidirectional.frag");
  blur_shader.use();
  blur_shader.bind(cached_size, "u.sample_count");
  for (auto i = 0; i < cached_size; ++i) {
    auto weight_name = "u.normalized_weights[" + std::to_string(i) + "]";
    blur_shader.bind(weights[i], weight_name.c_str());
  }

  {
    blur_shader.bind(1, "u.horizontal_step");
    blur_shader.bind(0, "u.vertical_step");
    blur_shader.bind(color, "u_color_texture", 0);

    intermediate.bind();
    quad->draw();
  }

  {
    blur_shader.bind(0, "u.horizontal_step");
    blur_shader.bind(1, "u.vertical_step");
    blur_shader.bind(intermediate_color, "u_color_texture", 0);

    output.bind();
    quad->draw();
  }
}

void Renderer::renderPerfOverlay(IFramebuffer &output, float deltaTime) {
  static Shader perf_overlay_shader("Quad.vert", "PerfOverlay.frag");
  // Circular buffer of frame times (in ms)
  const int frame_count = 100;
  static std::vector<int> frame_times(frame_count);
  static int frame_times_offset;
  frame_times[frame_times_offset++] = static_cast<int>(deltaTime * 1000);
  frame_times_offset %= frame_count;

  perf_overlay_shader.use();
  perf_overlay_shader.bind(frame_times, "u.frame_times");
  perf_overlay_shader.bind(frame_times_offset, "u.frame_times_offset");
  perf_overlay_shader.bind(Config::perf_overlay_scale, "u.scale");

  output.bind();
  glDisable(GL_DEPTH_TEST);
  // Blend over game
  glEnable(GL_BLEND);

  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bindVertexOnly();
  quad->draw();

  glDisable(GL_BLEND);
}

Shader fxaaShader(int level) {
  switch (level) {
    case 0:
      static Shader fxaa0("FXAA.vert", "FXAA_0.frag");
      return fxaa0;
    case 1:
      static Shader fxaa1("FXAA.vert", "FXAA_1.frag");
      return fxaa1;
    case 2:
      static Shader fxaa2("FXAA.vert", "FXAA_2.frag");
      return fxaa2;
    case 3:
      static Shader fxaa3("FXAA.vert", "FXAA_3.frag");
      return fxaa3;
    case 4:
      static Shader fxaa4("FXAA.vert", "FXAA_4.frag");
      return fxaa4;
    default:
      throw std::runtime_error(Formatter() << "Invalid fxaa level: " << level);
  }
}

void Renderer::renderAliasingBlur(std::shared_ptr<Texture> &color, IFramebuffer &output) {
  Shader blur_aliasing_shader = fxaaShader(Config::anti_aliasing_level);

  glm::vec2 rcp_frame(1.0/float(output.getWidth()), 1.0/float(output.getHeight()));

  blur_aliasing_shader.use();
  blur_aliasing_shader.bind(color, "uSourceTex", 0);
  blur_aliasing_shader.bind(rcp_frame, "RCPFrame");

  output.bind();
  glDisable(GL_DEPTH_TEST);
  // Blend over non anti aliased background
  glEnable(GL_BLEND);

  static std::shared_ptr<Shape> quad = ObjLoader::getQuad();
  quad->bind();
  quad->draw();

  glDisable(GL_BLEND);
}
