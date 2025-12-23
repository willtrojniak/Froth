#pragma once

#include "VulkanDevice.h"
#include <vector>

namespace Froth {

class VulkanDescriptorSetLayout {
public:
  VulkanDescriptorSetLayout() = default;
  VulkanDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
  VulkanDescriptorSetLayout(VulkanDescriptorSetLayout const &) = delete;
  void operator=(VulkanDescriptorSetLayout const &) = delete;

  VulkanDescriptorSetLayout(VulkanDescriptorSetLayout &&) noexcept;
  VulkanDescriptorSetLayout &operator=(VulkanDescriptorSetLayout &&) noexcept;

  ~VulkanDescriptorSetLayout();

  operator VkDescriptorSetLayout() const { return m_DescriptorSetLayout; }
  void cleanup();

private:
  VkDescriptorSetLayout m_DescriptorSetLayout;
};

} // namespace Froth
