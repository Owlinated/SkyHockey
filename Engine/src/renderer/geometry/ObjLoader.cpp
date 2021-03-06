#include <memory>
#include <vector>
#include <cstdio>
#include <string>
#include <map>
#include <tuple>
#include <cstring>
#include <glm/glm.hpp>
#include <stdexcept>
#include <iostream>
#include <tiny_obj_loader.h>
#include <src/support/Logger.h>
#include "ObjLoader.h"

/**
 * Create a new loader to load shapes from an .obj file.
 * @param path Path to .obj file.
 */
ObjLoader::ObjLoader(const std::string& path) {
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, ("res/" +  path).c_str());

  if (!ret) {
    Logger::error("Could not parse obj file: " + err);
  }
}

/**
 * Load shape from .obj file with the specified name.
 * @param name Name of shape to load.
 * @return Loaded shape.
 */
std::shared_ptr<Shape> ObjLoader::loadShape(const std::string &name) {
  for (auto &shape: shapes) {
    if (shape.name == name) {
      return loadShape(shape);
    }
  }
  Logger::error("Could not find object with name: " + name);
  return loadShape(shapes[0]);
}

/**
 * Load a shape internally. Converts a tinyobj shape into useful arrays.
 * @param shape Shape to load into memory.
 * @return A shape containing the loaded arrays
 */
std::shared_ptr<Shape> ObjLoader::loadShape(const tinyobj::shape_t shape) {
  std::vector<unsigned short> indices;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> textures;
  std::vector<glm::vec3> normals;

  auto index_offset = 0;
  unsigned short next_index = 0;
  std::map<std::tuple<int, int, int>, unsigned short> index_map;

  for (auto face_vertices: shape.mesh.num_face_vertices) {
    for (size_t v = 0; v < face_vertices; v++) {
      auto index = shape.mesh.indices[index_offset + v];
      std::tuple<int, int, int> index_map_key(index.vertex_index, index.normal_index, index.texcoord_index);
      if (index_map.find(index_map_key) == index_map.end()) {
        vertices.emplace_back(
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]);

        normals.emplace_back(
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2]);

        textures.emplace_back(
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1]);

        index_map[index_map_key] = next_index++;
      }
      indices.emplace_back(index_map[index_map_key]);
    }
    index_offset += face_vertices;
  }
  // return std::make_shared<Shape>(indices, vertices, normals, textures);
  return std::make_shared<Shape>(indices, vertices, normals, textures);
}

/**
 * Create a screen sizes quad.
 * @return Shape containing the quad.
 */
std::shared_ptr<Shape> createQuad() {
  std::vector<unsigned short> indices = {0, 1, 2, 2, 1, 3};
  std::vector<glm::vec3> vertices = {glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0), glm::vec3(-1, 1, 0), glm::vec3(1, 1, 0)};
  std::vector<glm::vec2> textures = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1)};
  std::vector<glm::vec3> normals = {glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)};

  return std::make_shared<Shape>(indices, vertices, normals, textures);
}

std::shared_ptr<Shape> ObjLoader::getQuad() {
  static std::shared_ptr<Shape> quad = createQuad();
  return quad;
}
