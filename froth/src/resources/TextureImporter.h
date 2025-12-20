#pragma once

#include "Texture2D.h"
#include "src/resources/ResourceMetadata.h"

#include <filesystem>

namespace Froth {

class TextureImporter {
public:
  static std::shared_ptr<Texture2D> ImportTexture2D(const ResourceMetadata &metadata);
  static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path &path);
};

} // namespace Froth
