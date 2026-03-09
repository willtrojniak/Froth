#include "VulkanShaderPipelineManager.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"

namespace Froth {

const VulkanShaderPipeline &VulkanShaderPipelineManager::getOrCreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                                             const Material &mat,
                                                                             const VulkanSwapchainManager &swapchain) {
  ShaderPipelineKey key = getPipelineKey(mat);

  const VulkanShaderModule &vertexShader = shaderModuleManager.getOrCreateShaderModule(mat.vertexShaderHandle());
  const VulkanShaderModule &fragmentShader = shaderModuleManager.getOrCreateShaderModule(mat.fragmentShaderHandle());

  if (!m_PipelineCache.contains(key)) {
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

    // FIXME: Check if the swapchain/renderpass has changed
    m_PipelineCache[key] = PipelineData{
        .pipeline = VulkanShaderPipeline(
            vertexShader,
            fragmentShader,
            layouts,
            swapchain),
        .descSets = mergedLayout.value(),
    };
  }

  return m_PipelineCache[key].pipeline;
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

VulkanShaderPipelineManager::ShaderPipelineKey VulkanShaderPipelineManager::getPipelineKey(const Material &mat) {

  ShaderPipelineKey key{};
  key.vertexShaderHandle = mat.vertexShaderHandle();
  key.fragmentShaderHandle = mat.fragmentShaderHandle();

  return key;
}
} // namespace Froth
