#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <stdexcept>
#include "src/support/Formatter.h"
#include "Texture.h"

Texture::Texture() = default;

Texture::Texture(GLuint handle, int width, int height):
  handle(handle), width(width), height(height) {
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, handle);
}
