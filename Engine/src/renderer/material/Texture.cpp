#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <stdexcept>
#include "src/support/Formatter.h"
#include "Texture.h"

Texture::Texture() = default;

Texture::Texture(GLuint handle): handle(handle) {
  init();
}

void Texture::init() {
  bind();
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, handle);
}
