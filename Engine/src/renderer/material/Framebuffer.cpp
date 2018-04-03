#include "Framebuffer.h"
#include <utility>
Framebuffer::Framebuffer(GLuint handle, std::shared_ptr<Texture> texture)
    : handle_(handle), texture(std::move(texture)) {
}

Framebuffer::Framebuffer(int width, int height, bool depth, bool mipmap) {
  glGenFramebuffers(1, &handle_);
  glBindFramebuffer(GL_FRAMEBUFFER, handle_);

  if (depth) {
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
  }

  glActiveTexture(GL_TEXTURE0);
  GLuint texture_handle;
  glGenTextures(1, &texture_handle);
  glBindTexture(GL_TEXTURE_2D, texture_handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  if(mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_handle, 0);

  texture = std::make_shared<Texture>(texture_handle);
}

void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle_);
}
