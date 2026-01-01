#include "ShaderModuleImporter.h"
#include "src/platform/filesystem/Filesystem.h"

namespace Froth {

std::shared_ptr<ShaderModule> ShaderModuleImporter::ImportShaderModule(const ResourceMetadata &metadata) {
  return LoadShaderModule(metadata.FilePath);
};

std::shared_ptr<ShaderModule> ShaderModuleImporter::LoadShaderModule(const std::filesystem::path &path) {
  const std::vector<char> code = Filesystem::readFile(path);

  return std::make_shared<ShaderModule>(code);
};

} // namespace Froth
