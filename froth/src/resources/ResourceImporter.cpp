#include "ResourceImporter.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "src/core/logger/Logger.h"

namespace Froth {

std::shared_ptr<Resource> ResourceImporter::ImportResource(const ResourceMetadata &metadata) {
  switch (metadata.Type) {
  case ResourceType::Mesh:
    return MeshImporter::ImportMesh(metadata);
  case ResourceType::Texture2D:
    return TextureImporter::ImportTexture2D(metadata);
  case ResourceType::None:
  default:
    FROTH_ERROR("No importer available for resource type: %s", Resource::ResourceTypeToString(metadata.Type));
    return nullptr;
  }
}

} // namespace Froth
