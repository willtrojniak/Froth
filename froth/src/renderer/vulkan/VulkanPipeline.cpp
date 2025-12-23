#include "VulkanPipeline.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include <vector>

namespace Froth {

VulkanPipeline::VulkanPipeline(const VulkanPipelineLayout &pipelineLayout,
                               const VulkanRenderPass &renderPass,
                               const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
                               const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
                               const VkPipelineInputAssemblyStateCreateInfo &inputAssemblyInfo,
                               const VkViewport &viewport,
                               const VkRect2D &scissor,
                               const VkPipelineRasterizationStateCreateInfo &rasterizerInfo,
                               const VkPipelineMultisampleStateCreateInfo &multisampleInfo,
                               const VkPipelineDepthStencilStateCreateInfo &depthStencilInfo,
                               const VkPipelineColorBlendStateCreateInfo &colorBlendInfo,
                               const VkPipelineDynamicStateCreateInfo &dynamicState) {

  // TODO: Support multiple viewports and scissors
  VkPipelineViewportStateCreateInfo viewportStateInfo{};
  viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateInfo.viewportCount = 1;
  viewportStateInfo.pViewports = &viewport;
  viewportStateInfo.scissorCount = 1;
  viewportStateInfo.pScissors = &scissor;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineInfo.pStages = shaderStages.data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportStateInfo;
  pipelineInfo.pRasterizationState = &rasterizerInfo;
  pipelineInfo.pMultisampleState = &multisampleInfo;
  pipelineInfo.pDepthStencilState = &depthStencilInfo;
  pipelineInfo.pColorBlendState = &colorBlendInfo;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  // TODO: Support pipeline cache
  // TODO: Support multiple pipelines?
  VulkanContext &vctx = VulkanContext::get();
  if (vkCreateGraphicsPipelines(vctx.device(), VK_NULL_HANDLE, 1, &pipelineInfo, vctx.allocator(), &m_Pipeline) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create Vulkan Pipeline")
  }
}

VulkanPipeline::VulkanPipeline(VulkanPipeline &&o) noexcept
    : m_Pipeline(o.m_Pipeline) {
  o.m_Pipeline = nullptr;
}

VulkanPipeline &VulkanPipeline::operator=(VulkanPipeline &&o) noexcept {
  m_Pipeline = o.m_Pipeline;

  o.m_Pipeline = nullptr;
  return *this;
}

VulkanPipeline::~VulkanPipeline() {
  cleanup();
}

void VulkanPipeline::cleanup() {
  VulkanContext &vctx = VulkanContext::get();
  if (m_Pipeline) {
    vkDestroyPipeline(vctx.device(), m_Pipeline, vctx.allocator());
    m_Pipeline = nullptr;
    FROTH_DEBUG("Destroyed Vulkan Pipeline")
  }
}

} // namespace Froth
