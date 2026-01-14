#include "VulkanShaderPipelineManager.h"

namespace Froth {

const VulkanShaderPipeline &VulkanShaderPipelineManager::registerMaterial(const Material &mat,
                                                                          const VulkanSwapchainManager &swapchain) {
  ShaderPipelineKey key = getPipelineKey(mat);

  if (!m_Cache.contains(key)) // FIXME: Check if the swapchain/renderpass has changed
    m_Cache[key] = VulkanShaderPipeline(mat, swapchain);

  return m_Cache[key];
}

const VulkanShaderPipeline &VulkanShaderPipelineManager::getShader(const Material &mat, const VulkanSwapchainManager &swapchain) {
  return registerMaterial(mat, swapchain);
}

VulkanShaderPipelineManager::ShaderPipelineKey VulkanShaderPipelineManager::getPipelineKey(const Material &mat) {

  ShaderPipelineKey key{};
  key.vertexShaderHandle = mat.vertexShaderHandle();
  key.fragmentShaderHandle = mat.fragmentShaderHandle();

  return key;
}
} // namespace Froth
