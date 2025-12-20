#include "VulkanRenderer.h"
#include "Defines.h"
#include "VulkanIndexBuffer.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanPipelineLayout.h"
#include "VulkanVertex.h"
#include "VulkanVertexBuffer.h"
#include "src/core/events/ApplicationEvent.h"
#include "src/core/events/EventDispatcher.h"
#include "src/renderer/vulkan/VulkanBuffer.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include "src/renderer/vulkan/VulkanDescriptorSet.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include "src/renderer/vulkan/VulkanSampler.h"
#include "src/resources/materials/Material.h"
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
    : m_SwapchainManager(window),
      m_DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>()),
      m_DescriptorPool(MAX_FRAMES_IN_FLIGHT, 0, MAX_FRAMES_IN_FLIGHT * 4),
      m_GraphicsCommandPool(VulkanContext::get().device().getQueueFamilies().graphics.index) {

  std::vector<VkDescriptorSetLayout> descSetLayouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
  m_DescriptorSets = m_DescriptorPool.allocateDescriptorSets(descSetLayouts);

  descSetLayouts = {m_DescriptorSetLayout};
  m_PipelineLayout = std::make_unique<VulkanPipelineLayout>(descSetLayouts);

  const size_t blankImageSize = 4;
  VulkanBuffer textureStagingBuffer(blankImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  uint32_t blankImageData = 0xFFFFFFFF;
  void *dataPtr = textureStagingBuffer.map();
  memcpy(dataPtr, &blankImageData, blankImageSize);
  textureStagingBuffer.unmap();

  VulkanCommandPool &commandPool = m_SwapchainManager.currentCommandPool();
  VulkanCommandBuffer commandBuffer = commandPool.AllocateCommandBuffer();

  m_BlankImage = VulkanImage(VulkanImage::CreateInfo{
      .extent = {1, 1},
      .format = VK_FORMAT_R8G8B8A8_SRGB,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      .memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  });
  m_BlankImage.transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  commandBuffer.reset();
  VulkanBuffer::copyBufferToImage(commandBuffer, textureStagingBuffer, m_BlankImage);
  commandBuffer.reset();
  m_BlankImage.transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  commandBuffer.cleanup(commandPool);

  m_BlankImageView = m_BlankImage.createView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
  m_Sampler = VulkanSampler::Builder().build();

  auto writer = VulkanDescriptorSet::Writer();
  for (auto descriptorSet : m_DescriptorSets) {
    writer.addImageSampler(descriptorSet, 0, m_BlankImageView, m_Sampler, 0);
  }
  writer.Write();
}

VulkanRenderer::~VulkanRenderer() {
  m_DescriptorPool.freeDescriptorSets(m_DescriptorSets);
}

VulkanRenderer::VulkanRenderer(VulkanRenderer &&o)
    : m_SwapchainManager(std::move(o.m_SwapchainManager)),
      m_DescriptorSetLayout(std::move(o.m_DescriptorSetLayout)),
      m_DescriptorPool(std::move(o.m_DescriptorPool)),
      m_DescriptorSets(std::move(o.m_DescriptorSets)),
      m_GraphicsCommandPool(std::move(o.m_GraphicsCommandPool)),
      m_PipelineLayout(std::move(o.m_PipelineLayout)),
      m_Pipeline(std::move(o.m_Pipeline)) {
}

VulkanRenderer &VulkanRenderer::operator=(VulkanRenderer &&o) {
  m_SwapchainManager = std::move(o.m_SwapchainManager);
  m_DescriptorSetLayout = std::move(o.m_DescriptorSetLayout);
  m_DescriptorPool = std::move(o.m_DescriptorPool);
  m_DescriptorSets = std::move(o.m_DescriptorSets);
  m_GraphicsCommandPool = std::move(o.m_GraphicsCommandPool);
  m_PipelineLayout = std::move(o.m_PipelineLayout);
  m_Pipeline = std::move(o.m_Pipeline);

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
}

void VulkanRenderer::endRenderPass() {
  vkCmdEndRenderPass(m_SwapchainManager.currentCommandBuffer());
}

void VulkanRenderer::endFrame() {
  m_SwapchainManager.endFrame();
}

void VulkanRenderer::pushConstants(VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void *pData) const {
  vkCmdPushConstants(m_SwapchainManager.currentCommandBuffer(), *m_PipelineLayout, stage, offset, size, pData);
}

void VulkanRenderer::bindMaterial(const Material &mat) {
  // FIXME: This assumes only one material
  if (!m_Pipeline) {
    m_Pipeline = buildPipeline(mat);
  }
  // Bind pipeline
  vkCmdBindPipeline(m_SwapchainManager.currentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, *m_Pipeline);

  // TODO: Should this move elsewhere?
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

  // TODO: Move
  vkCmdBindDescriptorSets(m_SwapchainManager.currentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, *m_PipelineLayout, 0, 1, &m_DescriptorSets[m_SwapchainManager.currentFrame()], 0, VK_NULL_HANDLE);
}

void VulkanRenderer::setDescriptorTexture(const VulkanSampler &sampler, const VulkanImageView &view) {
  auto writer = VulkanDescriptorSet::Writer();
  for (auto descriptorSet : m_DescriptorSets) {
    writer.addImageSampler(descriptorSet, 0, view, sampler, 1);
  }

  writer.Write();
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

  // TODO: Seperate into another call?
  vkCmdDrawIndexed(m_SwapchainManager.currentCommandBuffer(), indexBuffer.indexCount(), 1, 0, 0, 0);
}

VulkanTexture VulkanRenderer::createTexture(const VkExtent3D &extent, VkFormat format, const void *data) {
  Froth::VulkanCommandPool &commandPool = getGraphicsCommandPool();
  Froth::VulkanCommandBuffer commandBuffer = commandPool.AllocateCommandBuffer();

  VulkanTexture texture(extent, format);

  size_t imageSizeBytes = texture.sizeBytes();
  VulkanBuffer stagingBuffer(imageSizeBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  void *dest = stagingBuffer.map();
  memcpy(dest, data, imageSizeBytes);
  stagingBuffer.unmap();

  texture.transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  commandBuffer.reset();
  VulkanBuffer::copyBufferToImage(commandBuffer, stagingBuffer, texture);
  commandBuffer.reset();
  texture.transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  commandBuffer.cleanup(commandPool);
  return texture;
}

// TODO: Remove temporary
VulkanCommandPool &VulkanRenderer::getCurrentCommandPool() {
  return m_SwapchainManager.currentCommandPool();
}

std::unique_ptr<VulkanPipeline> VulkanRenderer::buildPipeline(const Material &mat) {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = m_SwapchainManager.swapchain().extent().width;
  viewport.height = m_SwapchainManager.swapchain().extent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_SwapchainManager.swapchain().extent();

  return VulkanPipelineBuilder()
      .setVertexInput(Vertex::getInputDescription().getInfo())
      .setShaders(mat.frag(), mat.vert())
      .setViewport(viewport, scissor)
      .build(m_SwapchainManager.renderpass(), *m_PipelineLayout);
}

} // namespace Froth
