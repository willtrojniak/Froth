#include "VulkanTexture.h"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanCommandPool.h"
#include "src/renderer/vulkan/VulkanRenderer.h"

namespace Froth {

VulkanTexture::VulkanTexture(const VkExtent3D &extent, VkFormat format)
    : VulkanImage(CreateInfo{
          .extent = extent,
          .format = format,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          .memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}),
      m_View(createView(format, VK_IMAGE_ASPECT_COLOR_BIT)) {
}

VulkanTexture VulkanTexture::create(const VkExtent3D &extent, VkFormat format, const void *data) {
  VulkanRenderer &renderer = Application::getInstance().renderer();
  VulkanCommandPool &commandPool = renderer.getGraphicsCommandPool();
  VulkanCommandBuffer commandBuffer = commandPool.AllocateCommandBuffer();

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
  FROTH_DEBUG("Created texture");
  return texture;
}
} // namespace Froth
