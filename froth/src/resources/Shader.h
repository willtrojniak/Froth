#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/renderer/vulkan/VulkanPipeline.h"
#include "src/renderer/vulkan/VulkanShaderModule.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/resources/Resource.h"

namespace Froth {
class Shader : public Resource {
public:
  Shader() = default;
  Shader(const VulkanShaderModule &vert, const VulkanShaderModule &frag, const VulkanSwapchainManager &swapchainManager);
  Shader(Shader const &) = delete;
  Shader &operator=(Shader const &) = delete;
  Shader(Shader &&) noexcept;
  Shader &operator=(Shader &&) noexcept;

  virtual ResourceType type() const noexcept { return ResourceType::Shader; };

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
