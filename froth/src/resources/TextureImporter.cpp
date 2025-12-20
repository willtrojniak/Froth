#include "TextureImporter.h"
#include "src/core/logger/Logger.h"
#include "src/platform/filesystem/Filesystem.h"

namespace Froth {

std::shared_ptr<Texture2D> TextureImporter::ImportTexture2D(const ResourceMetadata &metadata) {
  return LoadTexture2D(metadata.FilePath);
}

std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path &path) {

  int width, height, channels;
  void *data = Filesystem::loadImage(path.c_str(), width, height, channels);

  if (data == nullptr) {
    FROTH_ERROR("TextureImporter::LoadTexture2D - Could not load texture from filepath %s", path.c_str());
    return nullptr;
  }

  Extent2D extent{
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
  };

  // TODO: Dynamic format based on data
  std::shared_ptr<Texture2D> pTexture = std::make_shared<Texture2D>(extent, data);

  Filesystem::freeImage(data);
  return pTexture;
}
} // namespace Froth
