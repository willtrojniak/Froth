#pragma once

#include "VulkanPipeline.h"
#include "VulkanShaderModule.h"
#include <vector>

namespace Froth {

class VulkanPipelineBuilder {
public:
  std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
  VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
  VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyInfo{};
  VkViewport m_Viewport{};
  VkRect2D m_Scissor{};
  VkPipelineRasterizationStateCreateInfo m_RasterizerInfo{};
  VkPipelineMultisampleStateCreateInfo m_MultisampleInfo{};
  VkPipelineColorBlendAttachmentState m_ColorBlendAttachment{};
  VkPipelineDepthStencilStateCreateInfo m_DepthStencilInfo{};
  std::vector<VkDynamicState> m_DynamicStates;

  VulkanPipelineBuilder() noexcept;

  VulkanPipelineBuilder &setShaders(const VulkanShaderModule &vertexShader, const VulkanShaderModule &fragmentShader);
  VulkanPipelineBuilder &setVertexInput(const VkPipelineVertexInputStateCreateInfo &vertexInput);
  VulkanPipelineBuilder &setInputAssembly(const VkPipelineInputAssemblyStateCreateInfo &assemblyInfo);
  VulkanPipelineBuilder &setViewport(const VkViewport &viewport, const VkRect2D &scissor);
  VulkanPipelineBuilder &setRasterizer(const VkPipelineRasterizationStateCreateInfo &rasterizerInfo);
  VulkanPipelineBuilder &setMultisampling(const VkPipelineMultisampleStateCreateInfo &multisamplingInfo);
  VulkanPipelineBuilder &setColorBlend(const VkPipelineColorBlendAttachmentState &colorBlend);
  VulkanPipelineBuilder &setDepthStencil(const VkPipelineDepthStencilStateCreateInfo &depthStencilInfo);
  VulkanPipelineBuilder &setDynamicStates(const std::vector<VkDynamicState> &states);

  VulkanPipeline build(const VulkanRenderPass &renderPass, const VulkanPipelineLayout &pipelineLayout);
};

} // namespace Froth
