#include "Shader.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanPipelineBuilder.h"
#include "src/renderer/vulkan/VulkanVertex.h"

namespace Froth {

Shader::Shader(const VulkanShaderModule &vert, const VulkanShaderModule &frag, const VulkanSwapchainManager &swapchainManager) {
  // TODO: Customizable descriptor set layout - consider reading from shaders?
  // VkDescriptorSetLayoutBinding uboLayoutBinding{};
  // uboLayoutBinding.binding = 0;
  // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  // uboLayoutBinding.descriptorCount = 1;
  // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  // uboLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 0;
  samplerLayoutBinding.descriptorCount = 4; // Array size
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
  std::vector<VkDescriptorSetLayoutBinding> bindings = {samplerLayoutBinding};

  m_DescriptorSetLayouts = std::vector<VulkanDescriptorSetLayout>();
  m_DescriptorSetLayouts.emplace_back(bindings);

  // TODO: Bring out push constants to make configurable
  m_PipelineLayout = VulkanPipelineLayout(m_DescriptorSetLayouts);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = swapchainManager.swapchain().extent().width;
  viewport.height = swapchainManager.swapchain().extent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchainManager.swapchain().extent();
  m_Pipeline = VulkanPipelineBuilder()
                   .setVertexInput(Vertex::getInputDescription().getInfo())
                   .setShaders(vert, frag)
                   .setViewport(viewport, scissor)
                   .build(swapchainManager.renderpass(), m_PipelineLayout);

  FROTH_DEBUG("Created Shader");
}

Shader::Shader(Shader &&o) noexcept
    : m_DescriptorSetLayouts(std::move(o.m_DescriptorSetLayouts)),
      m_PipelineLayout(std::move(o.m_PipelineLayout)),
      m_Pipeline(std::move(o.m_Pipeline)) {
}

Shader &Shader::operator=(Shader &&o) noexcept {
  m_DescriptorSetLayouts = std::move(o.m_DescriptorSetLayouts);
  m_PipelineLayout = std::move(o.m_PipelineLayout);
  m_Pipeline = std::move(o.m_Pipeline);

  return *this;
}

} // namespace Froth
