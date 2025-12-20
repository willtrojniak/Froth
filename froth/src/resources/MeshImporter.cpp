#include "MeshImporter.h"
#include "src/platform/filesystem/Filesystem.h"

namespace Froth {

std::shared_ptr<Mesh> MeshImporter::ImportMesh(const ResourceMetadata &metadata) {
  return LoadMesh(metadata.FilePath);
}

std::shared_ptr<Mesh> MeshImporter::LoadMesh(const std::filesystem::path &path) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  Filesystem::loadObj(path.c_str(), vertices, indices);

  return std::make_shared<Mesh>(vertices, indices);
}

} // namespace Froth
