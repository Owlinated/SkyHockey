#include <iostream>
#include <vector>

#include <png.h>
#include <src/support/Logger.h>
#include "TexturePNG.h"

/**
 * Load texture from .png file
 * @param image_path File of .png file.
 * @param mipmap Whether to load/generate mipmaps.
 */
TexturePNG::TexturePNG(const std::string& image_path, bool mipmap) {
  const auto fp = fopen(("res/" + image_path).c_str(), "rb");
  const auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  const auto info = png_create_info_struct(png);
  png_init_io(png, fp);
  png_read_info(png, info);

  // Add alpha channel if png is RGB
  if (png_get_color_type(png, info) == PNG_COLOR_TYPE_RGB) {
    png_set_add_alpha(png, 0xff, PNG_FILLER_AFTER);
    png_read_update_info(png, info);
  }

  const auto width = png_get_image_width(png, info);
  const auto height = png_get_image_height(png, info);

  std::vector<unsigned char> data;
  data.resize(png_get_rowbytes(png, info) * height);
  const auto row_pointers = new png_bytep[height];
  for (auto y = 0; y < height; y++)
  {
	  row_pointers[height - 1 - y] = data.data() + y * png_get_rowbytes(png, info);
  }

  png_read_image(png, row_pointers);
  delete[] row_pointers;

  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  if(mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  this->width = static_cast<int>(width);
  this->height = static_cast<int>(height);
}
