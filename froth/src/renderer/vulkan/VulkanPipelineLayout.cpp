#include "VulkanPipelineLayout.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanContext.h"

#include <array>

namespace Froth {

VulkanPipelineLayout::VulkanPipelineLayout(const std::vector<VulkanDescriptorSetLayout> &descSetLayouts) {
  std::vector<VkDescriptorSetLayout> vkDescSetLayouts(descSetLayouts.size());
  for (size_t i = 0; i < descSetLayouts.size(); i++) {
    vkDescSetLayouts[i] = descSetLayouts[i];
  }

  // TODO: Configurable push constants?
  FROTH_INFO("Maximum push constant size: %u bytes", VulkanContext::get().device().props().limits.maxPushConstantsSize);
  std::array<VkPushConstantRange, 2> pushConstantRanges;
  pushConstantRanges[0].offset = 0;
  pushConstantRanges[0].size = sizeof(glm::mat4);
  pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRanges[1].offset = sizeof(glm::mat4);
  pushConstantRanges[1].size = sizeof(uint32_t);
  pushConstantRanges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkPipelineLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
  layoutInfo.pSetLayouts = vkDescSetLayouts.data();
  layoutInfo.pushConstantRangeCount = pushConstantRanges.size();
  layoutInfo.pPushConstantRanges = pushConstantRanges.data();

  VulkanContext &vctx = VulkanContext::get();
  if (vkCreatePipelineLayout(vctx.device(), &layoutInfo, vctx.allocator(), &m_Layout) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create pipeline layout");
  }
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout &&o)
    : m_Layout(o.m_Layout) {
  o.m_Layout = nullptr;
}
VulkanPipelineLayout &VulkanPipelineLayout::operator=(VulkanPipelineLayout &&o) {
  m_Layout = o.m_Layout;

  o.m_Layout = nullptr;
  return *this;
}

VulkanPipelineLayout::~VulkanPipelineLayout() {
  cleanup();
}

void VulkanPipelineLayout::cleanup() {
  if (m_Layout) {
    VulkanContext &vctx = VulkanContext::get();
    vkDestroyPipelineLayout(vctx.device(), m_Layout, vctx.allocator());
    m_Layout = nullptr;
    FROTH_DEBUG("Destroyed Vulkan Pipeline Layout")
  }
}

} // namespace Froth
