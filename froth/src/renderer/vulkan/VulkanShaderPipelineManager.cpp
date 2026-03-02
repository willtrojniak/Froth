#include "VulkanShaderPipelineManager.h"

namespace Froth {

const VulkanShaderPipeline &VulkanShaderPipelineManager::getOrCreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                                             const Material &mat,
                                                                             const VulkanSwapchainManager &swapchain) {
  ShaderPipelineKey key = getPipelineKey(mat);

  if (!m_Cache.contains(key)) // FIXME: Check if the swapchain/renderpass has changed
    m_Cache[key] = VulkanShaderPipeline(
        shaderModuleManager.getOrCreateShaderModule(mat.vertexShaderHandle()),
        shaderModuleManager.getOrCreateShaderModule(mat.fragmentShaderHandle()),
        mat.descriptorSetLayouts(),
        swapchain);

  return m_Cache[key];
}

VulkanShaderPipelineManager::ShaderPipelineKey VulkanShaderPipelineManager::getPipelineKey(const Material &mat) {

  ShaderPipelineKey key{};
  key.vertexShaderHandle = mat.vertexShaderHandle();
  key.fragmentShaderHandle = mat.fragmentShaderHandle();

  return key;
}
} // namespace Froth
