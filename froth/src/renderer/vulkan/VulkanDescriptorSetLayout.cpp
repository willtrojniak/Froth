#include "VulkanDescriptorSetLayout.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include <vector>

namespace Froth {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
  auto props = VulkanContext::get().device().props();
  FROTH_INFO("Max descriptor count: %u", props.limits.maxDescriptorSetSampledImages);

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  VulkanContext &vctx = VulkanContext::get();
  if (vkCreateDescriptorSetLayout(vctx.device(), &layoutInfo, vctx.allocator(), &m_DescriptorSetLayout) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create Descriptor Set Layout")
  }
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout &&o) noexcept
    : m_DescriptorSetLayout(o.m_DescriptorSetLayout) {
  o.m_DescriptorSetLayout = nullptr;
}

VulkanDescriptorSetLayout &VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout &&o) noexcept {
  m_DescriptorSetLayout = o.m_DescriptorSetLayout;
  o.m_DescriptorSetLayout = nullptr;

  return *this;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
  cleanup();
}

void VulkanDescriptorSetLayout::cleanup() {
  if (m_DescriptorSetLayout) {
    VulkanContext &vctx = VulkanContext::get();
    vkDestroyDescriptorSetLayout(vctx.device(), m_DescriptorSetLayout, vctx.allocator());
    m_DescriptorSetLayout = nullptr;
    FROTH_DEBUG("Destroyed Vulkan Descriptor Set Layout")
  }
}

} // namespace Froth
