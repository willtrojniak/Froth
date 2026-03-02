#pragma once

#include "src/platform/compiler/ShaderCompiler.h"
#include "src/renderer/vulkan/VulkanShaderModule.h"
#include "src/resources/Resource.h"
#include "src/resources/ShaderSource.h"

#include <map>

namespace Froth {
class VulkanShaderModuleManager {
public:
  VulkanShaderModuleManager() = default;
  VulkanShaderModuleManager(std::unique_ptr<ShaderCompiler> compiler);
  const VulkanShaderModule &getOrCreateShaderModule(const ResourceHandle<ShaderSource> &handle);

private:
  // Map shader source handles to shader modules
  std::map<ResourceHandle<ShaderSource>, VulkanShaderModule> m_ShaderModules;
  std::unique_ptr<ShaderCompiler> m_Compiler;
};

} // namespace Froth
