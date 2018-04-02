#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <GL/glew.h>
#include <src/support/Formatter.h>
#include "Shader.h"

void checkError(GLuint handle, std::string file_path) {
  GLint result = GL_FALSE;
  int info_length;

  glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
  glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &info_length);
  if (info_length > 1) {
    auto error_message = std::make_unique<char[]>(info_length + 1);
    glGetShaderInfoLog(handle, info_length, nullptr, error_message.get());
    std::string message(error_message.get());
    throw std::runtime_error(Formatter() << "Shader error in " << file_path << ": " << message);
  }
}

Shader::Shader(const char *vertex_file_path, const char *fragment_file_path) {
  std::ifstream vertex_stream(vertex_file_path);
  std::stringstream vertex_buffer;
  vertex_buffer << vertex_stream.rdbuf();
  auto vertex_string = vertex_buffer.str();
  auto vertex_source = vertex_string.c_str();

  GLuint vertex_handle = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_handle, 1, &vertex_source, nullptr);
  glCompileShader(vertex_handle);
  checkError(vertex_handle, std::string(vertex_file_path));

  std::ifstream fragment_stream(fragment_file_path);
  std::stringstream fragment_buffer;
  fragment_buffer << fragment_stream.rdbuf();
  auto fragment_string = fragment_buffer.str();
  auto fragment_source = fragment_string.c_str();

  GLuint fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_handle, 1, &fragment_source, nullptr);
  glCompileShader(fragment_handle);
  checkError(fragment_handle, std::string(fragment_file_path));

  handle_ = glCreateProgram();
  glAttachShader(handle_, vertex_handle);
  glAttachShader(handle_, fragment_handle);
  glLinkProgram(handle_);

  glDetachShader(handle_, vertex_handle);
  glDetachShader(handle_, fragment_handle);

  glDeleteShader(vertex_handle);
  glDeleteShader(fragment_handle);
}

void Shader::use() {
  glUseProgram(handle_);
}

GLint Shader::getUniform(const char *uniform_name) {
  return glGetUniformLocation(handle_, uniform_name);
}

void Shader::bind(std::shared_ptr<Texture> &texture, char *uniform_name, int unit_index) {
  // Activate texture unit and bind the texture
  glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit_index));
  texture->bind();

  // Find identifier of glsl variable and bind it to the same texture unit
  glUniform1i(getUniform(uniform_name), unit_index);
}
