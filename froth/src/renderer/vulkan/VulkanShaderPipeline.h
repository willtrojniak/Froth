#pragma once

#include "src/renderer/vulkan/VulkanPipeline.h"
#include "src/renderer/vulkan/VulkanShaderModule.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"

namespace Froth {
class VulkanShaderPipeline {
public:
  VulkanShaderPipeline() = default;
  VulkanShaderPipeline(const VulkanShaderModule &vertexShader,
                       const VulkanShaderModule &fragmentShader,
                       const std::vector<VkDescriptorSetLayout> &descriptorSets,
                       const VulkanSwapchainManager &swapchainManager);
  VulkanShaderPipeline(VulkanShaderPipeline const &) = delete;
  VulkanShaderPipeline &operator=(VulkanShaderPipeline const &) = delete;
  VulkanShaderPipeline(VulkanShaderPipeline &&) noexcept;
  VulkanShaderPipeline &operator=(VulkanShaderPipeline &&) noexcept;

  const VulkanPipelineLayout &pipelineLayout() const noexcept { return m_PipelineLayout; }
  const VulkanPipeline &pipeline() const noexcept { return m_Pipeline; }

private:
  VulkanPipelineLayout m_PipelineLayout;
  VulkanPipeline m_Pipeline;
};

} // namespace Froth
