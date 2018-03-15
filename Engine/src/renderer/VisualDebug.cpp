#include <GL/glew.h>
#include <src/renderer/material/Shader.h>
#include <src/renderer/geometry/Shape.h>
#include <src/renderer/geometry/ObjLoader.h>
#include "VisualDebug.h"

VisualDebug::VisualDebug() {
  quad_shape_ = ObjLoader::createQuad();
  quad_shader_ = new Shader("Passthrough.vert", "SimpleTexture.frag");
}

VisualDebug& VisualDebug::getInstance() {
  static VisualDebug instance;
  return instance;
}

void VisualDebug::pictureInPicture(std::shared_ptr<Texture>& texture) {
  glViewport(0, 0, 512, 512);
  quad_shader_->use();
  quad_shader_->bind(texture, const_cast<char *>("texture"), 0);

  quad_shape_->bindShadow();
  glDrawElements(GL_TRIANGLES, quad_shape_->index_count(), GL_UNSIGNED_SHORT, nullptr);
}
