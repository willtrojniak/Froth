#include "Shader.h"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanPipelineBuilder.h"
#include "src/renderer/vulkan/VulkanVertex.h"
#include "src/resources/ShaderModule.h"

namespace Froth {

Shader::Shader(const Material &material, const VulkanSwapchainManager &swapchainManager) {
  auto vertexShader = Application::getInstance().resourceManager().getResource<ShaderModule>(material.vertexShaderHandle());
  auto fragmentShader = Application::getInstance().resourceManager().getResource<ShaderModule>(material.fragmentShaderHandle());

  // TODO: Customizable descriptor set layout - consider reading from shaders?
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding uboLayoutBindingLights{};
  uboLayoutBindingLights.binding = 1;
  uboLayoutBindingLights.descriptorCount = 1;
  uboLayoutBindingLights.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBindingLights.pImmutableSamplers = VK_NULL_HANDLE;
  uboLayoutBindingLights.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 2;
  samplerLayoutBinding.descriptorCount = 4; // Array size
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  m_DescriptorSetLayouts.emplace_back(std::vector{uboLayoutBinding, uboLayoutBindingLights, samplerLayoutBinding});

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
                   .setShaders(vertexShader->module(), fragmentShader->module())
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
