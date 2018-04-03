#ifndef ENGINE_TEXTUREDDS_H
#define ENGINE_TEXTUREDDS_H

#include "Texture.h"

class TextureDDS: public Texture{
 public:
  TextureDDS(const char * image_path);
};

#endif //ENGINE_TEXTUREDDS_H
