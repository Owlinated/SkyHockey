#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <GL/glew.h>
#include "Shader.h"

Shader::Shader(const char *vertex_file_path, const char *fragment_file_path) {
  std::ifstream vertex_stream(vertex_file_path);
  std::stringstream vertex_buffer;
  vertex_buffer << vertex_stream.rdbuf();
  auto vertex_string = vertex_buffer.str();
  auto vertex_source = vertex_string.c_str();

  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertexShaderID, 1, &vertex_source, nullptr);
  glCompileShader(VertexShaderID);

  std::ifstream fragment_stream(fragment_file_path);
  std::stringstream fragment_buffer;
  fragment_buffer << fragment_stream.rdbuf();
  auto fragment_string = fragment_buffer.str();
  auto fragment_source = fragment_string.c_str();

  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragmentShaderID, 1, &fragment_source, nullptr);
  glCompileShader(FragmentShaderID);

  handle_ = glCreateProgram();
  glAttachShader(handle_, VertexShaderID);
  glAttachShader(handle_, FragmentShaderID);
  glLinkProgram(handle_);

  glDetachShader(handle_, VertexShaderID);
  glDetachShader(handle_, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
}

void Shader::use() {
  glUseProgram(handle_);
}

GLint Shader::getUniform(const char *uniform_name) {
  return glGetUniformLocation(handle_, uniform_name);
}

void Shader::bind(std::shared_ptr<Texture> &texture, char* uniform_name, int unit_index) {
  // Activate texture unit and bind the texture
  glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit_index));
  texture->bind();

  // Find identifier of glsl variable and bind it to the same texture unit
  glUniform1i(getUniform(uniform_name), unit_index);
}
