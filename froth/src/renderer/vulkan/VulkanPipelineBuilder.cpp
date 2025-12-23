#include "VulkanPipelineBuilder.h"

namespace Froth {

VulkanPipelineBuilder::VulkanPipelineBuilder() noexcept {

  // Default: triangle list
  m_InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  m_InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  m_InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  // Default: empty vertex input
  m_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  m_VertexInputInfo.vertexBindingDescriptionCount = 0;
  m_VertexInputInfo.vertexAttributeDescriptionCount = 0;

  // Must be set by user
  m_Viewport = {};
  m_Scissor = {};

  // Default rasterizer
  m_RasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  m_RasterizerInfo.depthClampEnable = VK_FALSE;
  m_RasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
  m_RasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
  m_RasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  m_RasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  m_RasterizerInfo.depthBiasEnable = VK_FALSE;
  m_RasterizerInfo.depthBiasConstantFactor = 0.0f;
  m_RasterizerInfo.depthBiasClamp = VK_FALSE;
  m_RasterizerInfo.depthBiasSlopeFactor = 0.0f;
  m_RasterizerInfo.lineWidth = 1.0f;

  // Default multisampling
  m_MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  m_MultisampleInfo.sampleShadingEnable = VK_FALSE;
  m_MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  m_MultisampleInfo.minSampleShading = 1.0f;
  m_MultisampleInfo.pSampleMask = nullptr;
  m_MultisampleInfo.alphaToCoverageEnable = VK_FALSE;
  m_MultisampleInfo.alphaToOneEnable = VK_FALSE;

  // Default color blending: no blending just write color
  m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  m_ColorBlendAttachment.blendEnable = VK_FALSE;

  // Default dynamic states
  m_DynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  // Default depth stencil
  m_DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  m_DepthStencilInfo.depthTestEnable = VK_TRUE;
  m_DepthStencilInfo.depthWriteEnable = VK_TRUE;
  m_DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  m_DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  m_DepthStencilInfo.minDepthBounds = 0.0f;
  m_DepthStencilInfo.maxDepthBounds = 1.0f;
  m_DepthStencilInfo.stencilTestEnable = VK_FALSE;
  m_DepthStencilInfo.front = {};
  m_DepthStencilInfo.back = {};
}

VulkanPipeline VulkanPipelineBuilder::build(const VulkanRenderPass &renderPass, const VulkanPipelineLayout &pipelineLayout) {

  // Dynamic State
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
  dynamicState.pDynamicStates = m_DynamicStates.data();

  // Colorblending
  VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
  colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendInfo.logicOpEnable = VK_FALSE;
  colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendInfo.attachmentCount = 1;
  colorBlendInfo.pAttachments = &m_ColorBlendAttachment;
  colorBlendInfo.blendConstants[0] = 0.0f;
  colorBlendInfo.blendConstants[1] = 0.0f;
  colorBlendInfo.blendConstants[2] = 0.0f;
  colorBlendInfo.blendConstants[3] = 0.0f;

  return VulkanPipeline(pipelineLayout, renderPass,
                        m_ShaderStages, m_VertexInputInfo, m_InputAssemblyInfo,
                        m_Viewport, m_Scissor, m_RasterizerInfo, m_MultisampleInfo,
                        m_DepthStencilInfo, colorBlendInfo, dynamicState);
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setShaders(const VulkanShaderModule &vertexShader, const VulkanShaderModule &fragmentShader) {
  m_ShaderStages = {vertexShader.pipelineStageInfo(), fragmentShader.pipelineStageInfo()};

  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setVertexInput(const VkPipelineVertexInputStateCreateInfo &vertexInput) {
  m_VertexInputInfo = vertexInput;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setInputAssembly(const VkPipelineInputAssemblyStateCreateInfo &assemblyInfo) {
  m_InputAssemblyInfo = assemblyInfo;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setViewport(const VkViewport &viewport, const VkRect2D &scissor) {
  m_Viewport = viewport;
  m_Scissor = scissor;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setRasterizer(const VkPipelineRasterizationStateCreateInfo &rasterizerInfo) {
  m_RasterizerInfo = rasterizerInfo;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setMultisampling(const VkPipelineMultisampleStateCreateInfo &multisamplingInfo) {
  m_MultisampleInfo = multisamplingInfo;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setColorBlend(const VkPipelineColorBlendAttachmentState &colorBlend) {
  m_ColorBlendAttachment = colorBlend;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setDepthStencil(const VkPipelineDepthStencilStateCreateInfo &depthStencilInfo) {
  m_DepthStencilInfo = depthStencilInfo;
  return *this;
}

VulkanPipelineBuilder &VulkanPipelineBuilder::setDynamicStates(const std::vector<VkDynamicState> &states) {
  m_DynamicStates = states;
  return *this;
}

} // namespace Froth
