#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/renderer/vulkan/VulkanPipeline.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/resources/Material.h"

namespace Froth {
class Shader {
public:
  Shader() = default;
  Shader(const Material &mat, const VulkanSwapchainManager &swapchainManager);
  Shader(Shader const &) = delete;
  Shader &operator=(Shader const &) = delete;
  Shader(Shader &&) noexcept;
  Shader &operator=(Shader &&) noexcept;

  const VulkanPipelineLayout &pipelineLayout() const noexcept { return m_PipelineLayout; }
  const VulkanPipeline &pipeline() const noexcept { return m_Pipeline; }
  const std::vector<VulkanDescriptorSetLayout> &descriptorSets() noexcept { return m_DescriptorSetLayouts; }

private:
  VulkanPipelineLayout m_PipelineLayout;
  VulkanPipeline m_Pipeline;

  // TODO: Move out to global cache for reuse across shaders
  std::vector<VulkanDescriptorSetLayout> m_DescriptorSetLayouts;
};

} // namespace Froth
