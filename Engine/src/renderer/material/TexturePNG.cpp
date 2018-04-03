#include <lodepng.h>
#include <iostream>
#include "src/support/Formatter.h"
#include "TexturePNG.h"

void recode() {
  std::vector<unsigned char> image;
  unsigned w, h;
  std::vector<unsigned char> buffer;
  lodepng::State state;
  unsigned error;

  state.decoder.color_convert = 0;
  state.decoder.remember_unknown_chunks = 1; //make it reproduce even unknown chunks in the saved image

  lodepng::load_file(buffer, "table.png");
  error = lodepng::decode(image, w, h, state, buffer);
  if(error != 0) {
    throw std::runtime_error(Formatter() << "Failed to decode png: " << lodepng_error_text(error));
  }

  buffer.clear();

  state.encoder.text_compression = 1;

  error = lodepng::encode(buffer, image, w, h, state);
  if(error != 0) {
    throw std::runtime_error(Formatter() << "Failed to encode png: " << lodepng_error_text(error));
  }

  lodepng::save_file(buffer, "table2.png");
}

TexturePNG::TexturePNG(const std::string& image_path, bool mipmap) {
  //recode();

  std::vector<unsigned char> buffer, image_flipped, image;
  lodepng::load_file(buffer, image_path);
  unsigned width, height;
  lodepng::State state;
  state.decoder.color_convert = 0;
  state.decoder.remember_unknown_chunks = 1;
  auto error = lodepng::decode(image_flipped, width, height, state, buffer);
  if(error != 0) {
    throw std::runtime_error(Formatter() << "Failed to load png: " << lodepng_error_text(error));
  }

  // png has origin in lower left, opengl in upper left, so need to flip
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width * 4; col++) {
      image.push_back(image_flipped[(height - 1 - row) * width * 4 + col]);
    }
  }

  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

  /*
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   */
  if(mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  init();
}
