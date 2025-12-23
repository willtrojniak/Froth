#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"

namespace Froth {

class VulkanPipelineLayout {
public:
  VulkanPipelineLayout() = default;
  VulkanPipelineLayout(const std::vector<VulkanDescriptorSetLayout> &descSetLayouts);
  VulkanPipelineLayout(VulkanPipelineLayout const &) = delete;
  void operator=(VulkanPipelineLayout const &) = delete;
  VulkanPipelineLayout(VulkanPipelineLayout &&);
  VulkanPipelineLayout &operator=(VulkanPipelineLayout &&);
  ~VulkanPipelineLayout();

  operator VkPipelineLayout() const { return m_Layout; }

private:
  VkPipelineLayout m_Layout = nullptr;

  void cleanup();
};

} // namespace Froth
