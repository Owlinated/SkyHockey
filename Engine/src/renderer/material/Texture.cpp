#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glad/glad.h>
#include <stdexcept>
#include "Texture.h"

/**
 * Default constructor for subclasses.
 */
Texture::Texture() = default;

/**
 * Wrap an existing texture.
 * @param handle Handle of texture to wraps.
 * @param width The texture's width.
 * @param height The texture's height.
 */
Texture::Texture(GLuint handle, int width, int height):
  handle(handle), width(width), height(height) {
}

/**
 * Bind the texture.
 */
void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, handle);
}
