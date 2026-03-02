#include "ShaderSourceImporter.h"
#include "src/platform/filesystem/Filesystem.h"

namespace Froth {

std::shared_ptr<ShaderSource> ShaderSourceImporter::ImportShaderSource(const ResourceMetadata &metadata) {
  return LoadShaderSource(metadata.FilePath);
};

std::shared_ptr<ShaderSource> ShaderSourceImporter::LoadShaderSource(const std::filesystem::path &path) {
  const std::vector<char> code = Filesystem::readFile(path);

  return std::make_shared<ShaderSource>(code);
};

} // namespace Froth
