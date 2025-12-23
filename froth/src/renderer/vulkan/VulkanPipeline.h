#pragma once

#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"

namespace Froth {

class VulkanPipeline {
public:
  VulkanPipeline() = default;
  VulkanPipeline(const VulkanPipelineLayout &pipelineLayout,
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
                 const VkPipelineDynamicStateCreateInfo &dynamicState);
  ~VulkanPipeline();

  VulkanPipeline(VulkanPipeline const &) = delete;
  VulkanPipeline &operator=(VulkanPipeline const &) = delete;
  VulkanPipeline(VulkanPipeline &&) noexcept;
  VulkanPipeline &operator=(VulkanPipeline &&) noexcept;

  operator VkPipeline() const { return m_Pipeline; }

private:
  VkPipeline m_Pipeline = nullptr;

  void cleanup();
};

} // namespace Froth
