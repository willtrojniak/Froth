#pragma once

#include "ShaderModule.h"
#include "src/resources/ResourceMetadata.h"

namespace Froth {
class ShaderModuleImporter {
public:
  static std::shared_ptr<ShaderModule> ImportShaderModule(const ResourceMetadata &metadata);
  static std::shared_ptr<ShaderModule> LoadShaderModule(const std::filesystem::path &path);
};

} // namespace Froth
