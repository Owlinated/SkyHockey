#ifndef ENGINE_TEXTUREDDS_H
#define ENGINE_TEXTUREDDS_H

#include "Texture.h"

class TexturePNG: public Texture{
 public:
  TexturePNG(const std::string& image_path, bool mipmap);
};

#endif //ENGINE_TEXTUREDDS_H
