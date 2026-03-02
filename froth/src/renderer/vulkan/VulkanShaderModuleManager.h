#pragma once

#include "src/renderer/vulkan/VulkanShaderModule.h"
#include "src/resources/Resource.h"

#include <map>

namespace Froth {
class VulkanShaderModuleManager {
public:
  VulkanShaderModuleManager() = default;
  const VulkanShaderModule &getOrCreateShaderModule(const ResourceHandle &handle);

private:
  // Map shader source handles to shader modules
  std::map<ResourceHandle, VulkanShaderModule> m_ShaderModules;
};

} // namespace Froth
