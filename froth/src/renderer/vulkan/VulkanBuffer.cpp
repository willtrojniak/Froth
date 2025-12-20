
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "src/core/logger/Logger.h"

namespace Froth {

VulkanBuffer::VulkanBuffer(const VkDeviceSize &size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProperties) {
  VulkanContext &vctx = VulkanContext::get();

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vctx.device(), &bufferInfo, vctx.allocator(), &m_Buffer) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create vertex buffer");
  }
  m_Size = size;

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vctx.device(), m_Buffer, &memRequirements);
  m_Memory = vctx.device().allocateMemory(memRequirements, memProperties);

  if (vkBindBufferMemory(vctx.device(), m_Buffer, m_Memory, 0) != VK_SUCCESS) {
    FROTH_ERROR("Failed to bind buffer memory");
    // FIXME: Improper cleanup
  }
}

VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
    : m_Buffer(other.m_Buffer), m_Size(other.m_Size), m_Memory(other.m_Memory) {
  other.m_Buffer = nullptr;
  other.m_Size = 0;
  other.m_Memory = nullptr;
}

VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept {
  m_Buffer = other.m_Buffer;
  m_Size = other.m_Size;
  m_Memory = other.m_Memory;
  other.m_Buffer = nullptr;
  other.m_Size = 0;
  other.m_Memory = nullptr;
  return *this;
}

// FIXME: ERROR CHECK
void *VulkanBuffer::map() const {
  void *data;
  vkMapMemory(VulkanContext::get().device(), m_Memory, 0, m_Size, 0, &data);
  return data;
}

void VulkanBuffer::unmap() const {
  vkUnmapMemory(VulkanContext::get().device(), m_Memory);
}

VulkanBuffer::~VulkanBuffer() {
  cleanup();
}

void VulkanBuffer::cleanup() {
  VulkanContext &vctx = VulkanContext::get();
  if (m_Memory) {
    vkFreeMemory(vctx.device(), m_Memory, vctx.allocator());
    m_Memory = nullptr;
    FROTH_DEBUG("Freed Vulkan Buffer Memory");
  }

  if (m_Buffer) {
    vkDestroyBuffer(vctx.device(), m_Buffer, vctx.allocator());
    m_Buffer = nullptr;
    FROTH_DEBUG("Destroyed Vulkan Buffer");
  }
}

bool VulkanBuffer::copyBuffer(const VulkanBuffer &src, const VulkanBuffer &dest, const VulkanCommandBuffer &commandBuffer) {
  VulkanContext &vctx = VulkanContext::get();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    FROTH_WARN("Failed to begin command buffer");
    return false;
  }

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = std::min(src.size(), dest.size());
  vkCmdCopyBuffer(commandBuffer, src, dest, 1, &copyRegion);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    FROTH_WARN("Failed to end command buffer");
    return false;
  }

  VkSubmitInfo submitInfo{};
  VkCommandBuffer b = commandBuffer;
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &b;
  if (vkQueueSubmit(vctx.device().getQueueFamilies().graphics.queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    FROTH_WARN("Failed to submit to queue");
    return false;
  }

  // PERF: Requires copies are done sequentially
  if (vkQueueWaitIdle(vctx.device().getQueueFamilies().graphics.queue) != VK_SUCCESS) {
    FROTH_WARN("Failed to wait for queue idle");
    return false;
  }
  return true;
}

bool VulkanBuffer::copyBufferToImage(VulkanCommandBuffer &commandBuffer, const VulkanBuffer &src, const VulkanImage &dst) {
  if (!commandBuffer.beginSingleTime())
    return false;

  VkBufferImageCopy copyInfo{};
  copyInfo.bufferOffset = 0;
  copyInfo.bufferRowLength = 0;
  copyInfo.bufferImageHeight = 0;
  copyInfo.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  copyInfo.imageSubresource.mipLevel = 0;
  copyInfo.imageSubresource.baseArrayLayer = 0;
  copyInfo.imageSubresource.layerCount = 1;
  copyInfo.imageOffset = {0, 0, 0};
  copyInfo.imageExtent = dst.extent();
  vkCmdCopyBufferToImage(commandBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

  if (!commandBuffer.end())
    return false;

  VulkanContext &vctx = VulkanContext::get();
  VkSubmitInfo submitInfo{};
  VkCommandBuffer b = commandBuffer;
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &b;
  if (vkQueueSubmit(vctx.device().getQueueFamilies().graphics.queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    FROTH_WARN("Failed to submit to queue");
    return false;
  }

  // FIXME: Requires copies are done sequentially
  if (vkQueueWaitIdle(vctx.device().getQueueFamilies().graphics.queue) != VK_SUCCESS) {
    FROTH_WARN("Failed to wait for queue idle");
    return false;
  }

  return true;
}

} // namespace Froth
