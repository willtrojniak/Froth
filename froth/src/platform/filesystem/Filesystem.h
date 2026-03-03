#pragma once

#include "src/renderer/vulkan/VulkanVertex.h"
#include <expected>
#include <filesystem>
#include <vector>

#include <tiny_obj_loader.h>

namespace Froth::Filesystem {

std::expected<std::vector<char>, bool> readFile(const std::filesystem::path &filename);
std::expected<size_t, bool> fileHash(const std::filesystem::path &filename);
size_t fileHash(const std::vector<char> &filecontents);
std::expected<std::filesystem::file_time_type, bool> readFileLastWriteTime(const std::filesystem::path &filename);

void *loadImage(const char *path, int &width, int &height, int &channels);
void freeImage(void *data);

bool loadObj(const char *path, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

} // namespace Froth::Filesystem
