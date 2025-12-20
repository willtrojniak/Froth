#pragma once

#include "Mesh.h"
#include "src/resources/ResourceMetadata.h"

namespace Froth {
class MeshImporter {
public:
  static std::shared_ptr<Mesh> ImportMesh(const ResourceMetadata &metadata);
  static std::shared_ptr<Mesh> LoadMesh(const std::filesystem::path &path);
};
} // namespace Froth
