#include "VulkanRenderer.h"
#include "Defines.h"
#include "VulkanIndexBuffer.h"
#include "VulkanVertexBuffer.h"
#include "src/core/events/ApplicationEvent.h"
#include "src/core/events/EventDispatcher.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace Froth {

#define BIND_FUNC(x) std::bind(&VulkanRenderer::x, this, std::placeholders::_1)

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

bool hasExtensions(const std::vector<const char *> &extensions) noexcept;
bool getRequiredExtensions(std::vector<const char *> &extensions) noexcept;
bool hasLayers(const std::vector<const char *> &layers) noexcept;

VulkanRenderer::VulkanRenderer(const Window &window)
    : m_SwapchainManager(window, MAX_FRAMES_IN_FLIGHT),
      m_GraphicsCommandPool(VulkanContext::get().device().getQueueFamilies().graphics.index) {
}

VulkanRenderer::~VulkanRenderer() {
}

VulkanRenderer::VulkanRenderer(VulkanRenderer &&o)
    : m_SwapchainManager(std::move(o.m_SwapchainManager)),
      m_GraphicsCommandPool(std::move(o.m_GraphicsCommandPool)) {
}

VulkanRenderer &VulkanRenderer::operator=(VulkanRenderer &&o) {
  m_SwapchainManager = std::move(o.m_SwapchainManager);
  m_GraphicsCommandPool = std::move(o.m_GraphicsCommandPool);

  return *this;
}

std::unique_ptr<VulkanRenderer> VulkanRenderer::create(const Window &window) {
  return std::make_unique<VulkanRenderer>(window);
}

void VulkanRenderer::shutdown() {
  VulkanContext &vctx = VulkanContext::get();
  vkDeviceWaitIdle(vctx.device());
}

bool VulkanRenderer::onEvent(const Event &e) {
  EventDispatcher dispatcher = EventDispatcher(e);
  dispatcher.dispatch<FramebufferResizeEvent>(BIND_FUNC(onFramebufferResize));
  return dispatcher.isHandled();
}

bool VulkanRenderer::onFramebufferResize(FramebufferResizeEvent &e) {
  m_SwapchainManager.onWindowFramebufferResize(e.width(), e.height());
  return false;
}

bool VulkanRenderer::beginFrame() {
  return m_SwapchainManager.beginFrame();
}

void VulkanRenderer::beginRenderPass() {
  // Render Pass Begin
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0] = {0.0f, 0.0f, 0.0f, 1.0f};
  clearValues[1] = {1.0f, 0};

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = m_SwapchainManager.renderpass();
  renderPassInfo.framebuffer = m_SwapchainManager.currentFramebuffer();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = m_SwapchainManager.swapchain().extent();
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
  vkCmdBeginRenderPass(m_SwapchainManager.currentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // Viewport
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = m_SwapchainManager.swapchain().extent().width;
  viewport.height = m_SwapchainManager.swapchain().extent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_SwapchainManager.currentCommandBuffer(), 0, 1, &viewport);

  // Scissor
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_SwapchainManager.swapchain().extent();
  vkCmdSetScissor(m_SwapchainManager.currentCommandBuffer(), 0, 1, &scissor);
}

void VulkanRenderer::endRenderPass() {
  vkCmdEndRenderPass(m_SwapchainManager.currentCommandBuffer());
}

void VulkanRenderer::endFrame() {
  m_SwapchainManager.endFrame();
}

void VulkanRenderer::pushConstants(const Shader &shader, VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void *pData) const {
  vkCmdPushConstants(m_SwapchainManager.currentCommandBuffer(), shader.pipelineLayout(), stage, offset, size, pData);
}

void VulkanRenderer::bindShader(const Shader &shader) const {
  // Bind pipeline
  vkCmdBindPipeline(m_SwapchainManager.currentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, shader.pipeline());
}

void VulkanRenderer::bindDescriptorSets(const Shader &shader, uint32_t start, const std::vector<VkDescriptorSet> &sets) const {
  vkCmdBindDescriptorSets(m_SwapchainManager.currentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          shader.pipelineLayout(),
                          start, sets.size(),
                          sets.data(),
                          0, VK_NULL_HANDLE);
}

void VulkanRenderer::bindDescriptorSets(const Shader &shader, uint32_t start, const std::vector<VkDescriptorSet> &sets, const std::vector<uint32_t> &offsets) const {
  // FIXME: Validate that dynamic offsets are the right size
  vkCmdBindDescriptorSets(m_SwapchainManager.currentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          shader.pipelineLayout(),
                          start, sets.size(),
                          sets.data(),
                          offsets.size(), offsets.data());
}

void VulkanRenderer::bindMesh(const Mesh &mesh) const {
  bindVertexBuffer(mesh.vertexBuffer());
  bindIndexBuffer(mesh.indexBuffer());
}

void VulkanRenderer::bindVertexBuffer(const VulkanVertexBuffer &vertexBuffer) const {
  // TODO: Handle dynamic offsets
  VkDeviceSize offsets[] = {0};
  VkBuffer vertexBuffers[] = {vertexBuffer};
  vkCmdBindVertexBuffers(m_SwapchainManager.currentCommandBuffer(), 0, 1, vertexBuffers, offsets);
}

void VulkanRenderer::bindIndexBuffer(const VulkanIndexBuffer &indexBuffer) const {
  // TODO: Handle offsets
  vkCmdBindIndexBuffer(m_SwapchainManager.currentCommandBuffer(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);

  // PERF: Seperate into another call?
  vkCmdDrawIndexed(m_SwapchainManager.currentCommandBuffer(), indexBuffer.indexCount(), 1, 0, 0, 0);
}

Shader VulkanRenderer::createShader(const Material &mat) {
  return Shader(mat, m_SwapchainManager);
}

} // namespace Froth
