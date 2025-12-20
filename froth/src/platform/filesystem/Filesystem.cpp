#include "Filesystem.h"

#include "src/core/logger/Logger.h"
#include <algorithm>
#include <exception>
#include <fstream>

#include <stb/stb_image.h>

namespace Froth::Filesystem {

std::vector<char> readFile(const std::filesystem::path &filename) {
  std::vector<char> buffer;
  try {
    size_t filesize = std::filesystem::file_size(filename);
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      FROTH_ERROR("Could not open file %s", filename.c_str());
    }
    buffer.resize(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);
    file.close();

  } catch (std::exception &e) {
    FROTH_ERROR("Exception while reading file: %s", e.what())
  }

  return buffer;
}

void *loadImage(const char *path, int &width, int &height, int &channels) {
  int texChannels;
  int forcedChannels = STBI_rgb_alpha;
  void *pixels = stbi_load(path, &width, &height, &texChannels, forcedChannels);
  if (!pixels) {
    width = 0;
    height = 0;
    channels = 0;
    return nullptr;
  }

  channels = forcedChannels;
  return pixels;
}

void freeImage(void *data) {
  stbi_image_free(data);
}

bool loadObj(const char *path, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path)) {
    FROTH_WARN("Failed to open object file");
    return false;
  }

  for (const tinyobj::shape_t &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};
      vertex.pos = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2],
      };
      vertex.norm = {
          attrib.normals[3 * index.normal_index + 0],
          attrib.normals[3 * index.normal_index + 1],
          attrib.normals[3 * index.normal_index + 2],
      };
      vertex.color = {
          attrib.colors[3 * index.vertex_index + 0],
          attrib.colors[3 * index.vertex_index + 1],
          attrib.colors[3 * index.vertex_index + 2],
      };

      if (!attrib.texcoords.empty()) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
        };
      } else {
        vertex.uv = {0.f, 0.f};
      }

      // FIXME: Use a more efficient data structure to find
      auto it = std::find(vertices.begin(), vertices.end(), vertex);
      if (it == vertices.end()) {
        indices.push_back(vertices.size());
        vertices.push_back(vertex);
      } else {
        indices.push_back(it - vertices.begin());
      }
    }
  }
  return true;
}

} // namespace Froth::Filesystem
