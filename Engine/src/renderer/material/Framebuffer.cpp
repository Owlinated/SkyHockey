#include "Framebuffer.h"
#include <utility>
#include <sstream>

/**
 * Wrap an existing framebuffer object.
 * @param handle Handle of framebuffer
 * @param texture Textures which the framebuffer is rendering to.
 */
Framebuffer::Framebuffer(GLuint handle, std::shared_ptr<Texture> texture)
    : handle_(handle) {
  textures.push_back(std::move(texture));
}

/**
 * Create a new framebuffer object.
 * @param width Width of framebuffer to create.
 * @param height Height of framebuffer to create.
 * @param texture_count Number of textures to create and bind.
 * @param depth Whether to create a depth buffer.
 * @param sample The sampling mode to use.
 * @param precision The precision to use for textures.
 * @param fb_name Debug friendly name of framebuffer.
 */
Framebuffer::Framebuffer(int width,
                         int height,
                         int texture_count,
                         bool depth,
                         SamplingMode sample,
                         Precision precision,
                         const std::string &fb_name) :
    width(width),
    height(height),
    texture_count(texture_count),
    draw_buffers(texture_count, 0) {
  glGenFramebuffers(1, &handle_);
  glBindFramebuffer(GL_FRAMEBUFFER, handle_);

  if (depth) {
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
  }

  for (int texture_id = 0; texture_id < texture_count; texture_id++) {
    auto attachment = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + texture_id);
    draw_buffers[texture_id] = attachment;

    GLuint texture_handle;
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, precision, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    auto maxFilter = sample == SamplingMode::Linear ? GL_LINEAR : GL_NEAREST;
    auto minFilter = sample == SamplingMode::Mipmap ? GL_LINEAR_MIPMAP_LINEAR : maxFilter;

    if (sample == SamplingMode::Mipmap) {
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture_handle, 0);

    std::stringstream label;
    label << fb_name << texture_id;
    glObjectLabel(GL_TEXTURE, texture_handle, -1, label.str().c_str());

    textures.push_back(std::make_shared<Texture>(texture_handle, width, height));
  }
}

/**
 * Activate the framebuffer for rendering.
 */
void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle_);
  glDrawBuffers(texture_count, &draw_buffers[0]);
  glViewport(0, 0, width, height);
}
