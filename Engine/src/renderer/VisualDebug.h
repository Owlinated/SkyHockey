#ifndef ENGINE_VISUAL_DEBUG_H
#define ENGINE_VISUAL_DEBUG_H

class VisualDebug {
 private:
  VisualDebug();
  std::shared_ptr<Shape> quad_shape_;
  Shader* quad_shader_;
 public:
  static VisualDebug& getInstance();
  void pictureInPicture(std::shared_ptr<Texture> &texture);
};

#endif //ENGINE_VISUAL_DEBUG_H
