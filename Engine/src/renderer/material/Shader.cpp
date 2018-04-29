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
#include <map>
#include "Shader.h"

/**
 * Find errors that occurred during shader compilation.
 * @param handle Handle of the shader to check.
 * @param file_path Path of the shader file for debugging.
 */
void checkShaderError(GLuint handle, std::string file_path) {
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

/**
 * Find errors that occurred during program linking.
 * @param handle Handle of the program to check.
 * @param vertex_path Path of the loaded vertex shader for debugging.
 * @param fragment_path Path of the loaded fragment shader for debugging.
 */
void checkProgramError(GLuint handle, std::string vertex_path, std::string fragment_path) {
  int info_length;

  glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &info_length);
  if (info_length > 1) {
    auto error_message = std::make_unique<char[]>(info_length + 1);
    glGetProgramInfoLog(handle, info_length, nullptr, error_message.get());
    std::string message(error_message.get());
    throw std::runtime_error(
        Formatter() << "Error while linking " << vertex_path << " and " << fragment_path << ": " << message);
  }
}

/**
 * Load a shader program from files.
 * @param vertex_file_path Path of vertex program to load.
 * @param fragment_file_path Path of fragment program to load.
 */
Shader::Shader(const std::string &vertex_file_path, const std::string &fragment_file_path) {
  std::ifstream vertex_stream("res/" + vertex_file_path);
  std::stringstream vertex_buffer;
  vertex_buffer << vertex_stream.rdbuf();
  auto vertex_string = vertex_buffer.str();
  auto vertex_source = vertex_string.c_str();

  GLuint vertex_handle = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_handle, 1, &vertex_source, nullptr);
  glCompileShader(vertex_handle);
  checkShaderError(vertex_handle, std::string(vertex_file_path));

  std::ifstream fragment_stream("res/" + fragment_file_path);
  std::stringstream fragment_buffer;
  fragment_buffer << fragment_stream.rdbuf();
  auto fragment_string = fragment_buffer.str();
  auto fragment_source = fragment_string.c_str();

  GLuint fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_handle, 1, &fragment_source, nullptr);
  glCompileShader(fragment_handle);
  checkShaderError(fragment_handle, std::string(fragment_file_path));

  handle = glCreateProgram();
  glAttachShader(handle, vertex_handle);
  glAttachShader(handle, fragment_handle);
  glLinkProgram(handle);
  checkProgramError(handle, std::string(vertex_file_path), std::string(fragment_file_path));

  glDetachShader(handle, vertex_handle);
  glDetachShader(handle, fragment_handle);

  glDeleteShader(vertex_handle);
  glDeleteShader(fragment_handle);
}

/**
 * Use program for rendering.
 */
void Shader::use() {
  glUseProgram(handle);
}

/**
 * Get handle to uniform variable by name.
 * @param uniform_name Name of variable to get handle for.
 * @return Handle of uniform variable.
 */
GLint Shader::getUniform(const std::string &uniform_name) {
  // Look name up in cache
  auto cache_result = cache_.find(uniform_name);
  if (cache_result != cache_.end()) {
    return cache_result->second;
  }

  // Look name up in shader
  auto result = glGetUniformLocation(handle, uniform_name.data());
  if (result < 0) {
    throw std::runtime_error(Formatter() << "Could not find uniform: " << std::string(uniform_name));
  }

  // Create cache entry and return value
  cache_[uniform_name] = result;
  return result;
}

/**
 * Bind textures to shader program.
 * @param texture Texture to bind to shader.
 * @param uniform_name Name of variable to bind texture to.
 * @param unit_index Texture unit index to bind texture to.
 */
void Shader::bind(std::shared_ptr<Texture> &texture, const std::string &uniform_name, int unit_index) {
  // Activate texture unit and bind the texture
  glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit_index));
  texture->bind();
  bind(unit_index, uniform_name);
}

/**
 * Bind scalar value to shader program.
 * @param scalar Value to bind to shader.
 * @param uniform_name Name of variable to bind value to.
 */
void Shader::bind(int scalar, const std::string &uniform_name) {
  glUniform1i(getUniform(uniform_name), scalar);
}

/**
 * Bind scalar value to shader program.
 * @param scalar Value to bind to shader.
 * @param uniform_name Name of variable to bind value to.
 */
void Shader::bind(float scalar, const std::string &uniform_name) {
  glUniform1f(getUniform(uniform_name), scalar);
}

/**
 * Bind vector value to shader program.
 * @param vector Value to bind to shader.
 * @param uniform_name Name of variable to bind vector to.
 */
void Shader::bind(glm::vec2 &vector, const std::string &uniform_name) {
  glUniform2f(getUniform(uniform_name), vector.x, vector.y);
}

/**
 * Bind vector value to shader program.
 * @param vector Value to bind to shader.
 * @param uniform_name Name of variable to bind vector to.
 */
void Shader::bind(glm::vec3 &vector, const std::string &uniform_name) {
  glUniform3f(getUniform(uniform_name), vector.x, vector.y, vector.z);
}

/**
 * Bind vector value to shader program.
 * @param vector Value to bind to shader.
 * @param uniform_name Name of variable to bind vector to.
 */
void Shader::bind(glm::vec4 &vector, const std::string &uniform_name) {
  glUniform4f(getUniform(uniform_name), vector.x, vector.y, vector.z, vector.w);
}

/**
 * Bind matrix value to shader program.
 * @param matrix Value to bind to shader.
 * @param uniform_name Name of variable to bind matrix to.
 */
void Shader::bind(glm::mat4 &matrix, const std::string &uniform_name) {
  glUniformMatrix4fv(getUniform(uniform_name), 1, GL_FALSE, &matrix[0][0]);
}

/**
 * Bind vector value to shader program.
 * @param vector Value to bind to shader.
 * @param uniform_name Name of variable to bind vector to.
 */
void Shader::bind(std::vector<int> &vector, const std::string &uniform_name) {
  glUniform1iv(getUniform(uniform_name), vector.size(), vector.data());
}
