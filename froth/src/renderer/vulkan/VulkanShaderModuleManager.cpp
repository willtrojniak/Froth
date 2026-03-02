#include "VulkanShaderModuleManager.h"
#include "src/core/Application.h"
#include "src/resources/ShaderSource.h"

namespace Froth {

const VulkanShaderModule &VulkanShaderModuleManager::getOrCreateShaderModule(const ResourceHandle &handle) {
  if (!m_ShaderModules.contains(handle)) {
    auto shaderSrc = Application::getInstance().resourceManager().getResource<ShaderSource>(handle);
    m_ShaderModules[handle] = VulkanShaderModule(shaderSrc->data());
  }

  return m_ShaderModules[handle];
}

} // namespace Froth
