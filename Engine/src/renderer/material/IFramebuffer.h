#ifndef ENGINE_IFRAMEBUFFER_H
#define ENGINE_IFRAMEBUFFER_H

class IFramebuffer {
 public:
  virtual int getWidth() = 0;
  virtual int getHeight() = 0;
  virtual GLuint getHandle() { return 0; }
  virtual void bind() = 0;
};

#endif //ENGINE_IFRAMEBUFFER_H
