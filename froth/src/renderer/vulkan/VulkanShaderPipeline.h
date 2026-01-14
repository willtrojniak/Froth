#pragma once

#include "src/renderer/vulkan/VulkanPipeline.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/resources/Material.h"

namespace Froth {
class VulkanShaderPipeline {
public:
  VulkanShaderPipeline() = default;
  VulkanShaderPipeline(const Material &mat, const VulkanSwapchainManager &swapchainManager);
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
