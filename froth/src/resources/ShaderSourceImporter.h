#pragma once

#include "ShaderSource.h"
#include "src/resources/ResourceMetadata.h"

namespace Froth {
class ShaderSourceImporter {
public:
  static std::shared_ptr<ShaderSource> ImportShaderSource(const ResourceMetadata &metadata);
  static std::shared_ptr<ShaderSource> LoadShaderSource(const std::filesystem::path &path);
};

} // namespace Froth
