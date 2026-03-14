#include "VulkanShaderPipelineManager.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"

namespace Froth {

const VulkanShaderPipeline &VulkanShaderPipelineManager::getOrCreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                                             const Material &mat,
                                                                             const VulkanSwapchainManager &swapchain) {
  ShaderPipelineKey key = getPipelineKey(mat);

  // FIXME: Check if the swapchain/renderpass has changed
  if (!m_PipelineCache.contains(key)) {
    m_PipelineCache[key] = createPipeline(shaderModuleManager, mat, swapchain);
    m_ShaderDependents[mat.vertexShaderHandle()].push_back(mat);
    m_ShaderDependents[mat.fragmentShaderHandle()].push_back(mat);
  }

  return m_PipelineCache[key].pipeline;
}

const std::optional<VulkanShaderPipeline> VulkanShaderPipelineManager::recreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                                                        const Material &mat,
                                                                                        const VulkanSwapchainManager &swapchain) {
  std::optional<VulkanShaderPipeline> oldPipeline;
  ShaderPipelineKey key = getPipelineKey(mat);
  auto it = m_PipelineCache.find(key);
  if (it != m_PipelineCache.end()) {
    oldPipeline = std::move(it->second.pipeline);
  } else {
    oldPipeline = std::nullopt;
  }

  m_PipelineCache[key] = createPipeline(shaderModuleManager, mat, swapchain);
  FROTH_DEBUG("Recreated pipeline");

  // FIXME: Cleanup old descriptor set layouts

  return oldPipeline;
}

void VulkanShaderPipelineManager::onShaderLoaded(VulkanShaderModuleManager &shaderModuleManager,
                                                 ResourceHandle<ShaderSource> shaderHandle,
                                                 const VulkanSwapchainManager &swapchain) {
  const auto it = m_ShaderDependents.find(shaderHandle);
  if (it == m_ShaderDependents.end())
    return;

  const auto &dependents = it->second;
  if (dependents.empty())
    return;

  // 1. Rebuild the shader module
  auto oldModule = shaderModuleManager.recreateShaderModule(shaderHandle);

  // 2. Rebuild each dependent pipeline
  for (const auto &mat : dependents) {
    auto oldPipeline = recreatePipeline(shaderModuleManager, mat, swapchain);
    // FIXME: Cleanup old pipelines properly waiting for no longer in use
  }
}

const std::expected<std::vector<VkDescriptorSetLayout>, bool> VulkanShaderPipelineManager::getMaterialDescSetLayouts(const Material &mat) {
  ShaderPipelineKey key = getPipelineKey(mat);

  auto it = m_PipelineCache.find(key);
  if (it == m_PipelineCache.end())
    return std::unexpected(false);

  std::vector<VkDescriptorSetLayout> descSets;
  descSets.reserve(it->second.descSets.size());
  for (const auto &descSetData : it->second.descSets) {
    descSets.push_back(m_LayoutCache.at(descSetData));
  }

  return descSets;
}

VulkanShaderPipelineManager::PipelineData VulkanShaderPipelineManager::createPipeline(VulkanShaderModuleManager &shaderModuleManager, const Material &mat, const VulkanSwapchainManager &swapchain) {
  const VulkanShaderModule &vertexShader = shaderModuleManager.getOrCreateShaderModule(mat.vertexShaderHandle());
  const VulkanShaderModule &fragmentShader = shaderModuleManager.getOrCreateShaderModule(mat.fragmentShaderHandle());
  FROTH_DEBUG("Creating pipeline");

  const auto mergedLayout = VulkanShaderModule::mergeDescriptorSetLayouts(vertexShader.descriptorSetLayouts(), fragmentShader.descriptorSetLayouts());
  if (!mergedLayout) {
    FROTH_ERROR("Failed to merge shader descriptor set layouts");
  }
  FROTH_DEBUG("Merged layouts: %u", mergedLayout.value().size());

  for (const auto &set : mergedLayout.value()) {
    FROTH_DEBUG("Merged set %u:", set.setNumber);
    for (const auto &binding : set.bindings) {
      FROTH_DEBUG("Binding %u: type = %u", binding.binding, binding.descriptorType);
    }
  }

  std::vector<VkDescriptorSetLayout> layouts;
  for (const auto &set : mergedLayout.value()) {
    if (!m_LayoutCache.contains(set)) {
      m_LayoutCache[set] = VulkanDescriptorSetLayout(set.bindings);
    }
    layouts.emplace_back(m_LayoutCache[set]);
  }

  return PipelineData{
      .pipeline = VulkanShaderPipeline(
          vertexShader,
          fragmentShader,
          layouts,
          swapchain),
      .descSets = mergedLayout.value(),
  };
}

VulkanShaderPipelineManager::ShaderPipelineKey VulkanShaderPipelineManager::getPipelineKey(const Material &mat) {

  ShaderPipelineKey key{};
  key.vertexShaderHandle = mat.vertexShaderHandle();
  key.fragmentShaderHandle = mat.fragmentShaderHandle();

  return key;
}
} // namespace Froth
