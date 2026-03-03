#include "ShaderSourceImporter.h"
#include "src/core/logger/Logger.h"
#include "src/platform/filesystem/Filesystem.h"

namespace Froth {

std::shared_ptr<ShaderSource> ShaderSourceImporter::ImportShaderSource(const ResourceMetadata &metadata) {
  return LoadShaderSource(metadata.FilePath);
};

std::shared_ptr<ShaderSource> ShaderSourceImporter::LoadShaderSource(const std::filesystem::path &path) {
  auto code = Filesystem::readFile(path);
  if (!code) {
    FROTH_WARN("Shader Source Importer: Failed to import shader");
    return nullptr;
  }

  return std::make_shared<ShaderSource>(code.value());
};

} // namespace Froth
