#include "src/renderer/vulkan/VulkanShaderPipeline.h"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanPipelineBuilder.h"
#include "src/renderer/vulkan/VulkanVertex.h"
#include "src/resources/ShaderModule.h"

namespace Froth {

VulkanShaderPipeline::VulkanShaderPipeline(const Material &mat, const VulkanSwapchainManager &swapchainManager) {
  auto vertexShader = Application::getInstance().resourceManager().getResource<ShaderModule>(mat.vertexShaderHandle());
  auto fragmentShader = Application::getInstance().resourceManager().getResource<ShaderModule>(mat.fragmentShaderHandle());

  // TODO: Bring out push constants to make configurable
  m_PipelineLayout = VulkanPipelineLayout(mat.descriptorSetLayouts());

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

VulkanShaderPipeline::VulkanShaderPipeline(VulkanShaderPipeline &&o) noexcept
    : m_PipelineLayout(std::move(o.m_PipelineLayout)),
      m_Pipeline(std::move(o.m_Pipeline)) {
}

VulkanShaderPipeline &VulkanShaderPipeline::operator=(VulkanShaderPipeline &&o) noexcept {
  m_PipelineLayout = std::move(o.m_PipelineLayout);
  m_Pipeline = std::move(o.m_Pipeline);

  return *this;
}

} // namespace Froth
