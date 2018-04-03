#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <stdexcept>
#include "src/support/Formatter.h"
#include "Texture.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

Texture::Texture(GLuint handle) {
  handle_ = handle;
  bind();
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width_);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height_);
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, handle_);
}

std::shared_ptr<Texture> Texture::loadDds(const char *image_path) {
  unsigned char header[124];

  auto fp = fopen(image_path, "rb");
  if (fp == nullptr) {
    throw std::runtime_error(Formatter() << "Could not load texture. " << image_path << " could not be opened.");
  }

  /* verify the type of file */
  char file_code[4];
  fread(file_code, 1, 4, fp);
  if (strncmp(file_code, "DDS ", 4) != 0) {
    fclose(fp);
    throw std::runtime_error(Formatter() << "File is not DDS but " << file_code);
  }

  fread(&header, 124, 1, fp);
  unsigned int height = *(unsigned int *) &(header[8]);
  unsigned int width = *(unsigned int *) &(header[12]);
  unsigned int linear_size = *(unsigned int *) &(header[16]);
  unsigned int mip_map_count = *(unsigned int *) &(header[24]);
  unsigned int fourCC = *(unsigned int *) &(header[80]);

  unsigned char *buffer;
  unsigned int buffer_size;
  buffer_size = mip_map_count > 1 ? linear_size * 2 : linear_size;
  buffer = (unsigned char *) malloc(buffer_size * sizeof(unsigned char));
  fread(buffer, 1, buffer_size, fp);
  fclose(fp);

  unsigned int format;
  switch (fourCC) {
    case FOURCC_DXT1:format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      break;
    case FOURCC_DXT3:format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      break;
    case FOURCC_DXT5:format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      break;
    default: free(buffer);
      throw std::runtime_error(Formatter() << "Unexpected DDS format " << fourCC);
  }

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  unsigned int offset = 0;

  // Load mipmaps
  for (unsigned int level = 0; level < mip_map_count && (width || height); ++level) {
    unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

    offset += size;
    width /= 2;
    height /= 2;

    if (width < 1) width = 1;
    if (height < 1) height = 1;
  }

  free(buffer);
  return std::make_shared<Texture>(textureID);
}

std::shared_ptr<Texture> Texture::createDepthTexture(int width, int height) {
  // REMARK: This only makes sense due to variance shadow mapping
  glActiveTexture(GL_TEXTURE0);
  GLuint depthTexture;
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, 0);
  glGenerateMipmapEXT(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
  return std::make_shared<Texture>(depthTexture);
}
