
#include "VulkanUniformBuffer.h"
#include <algorithm>
#include <utility>

namespace Froth {
VulkanUniformBuffer::VulkanUniformBuffer(const VkDeviceSize &size)
    : VulkanBuffer(size,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      m_StagingBuffer(size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBuffer &&o) noexcept
    : VulkanBuffer(std::move(o)), m_StagingBuffer(std::move(o.m_StagingBuffer)) {}

VulkanUniformBuffer &VulkanUniformBuffer::operator=(VulkanUniformBuffer &&o) noexcept {
  VulkanBuffer::operator=(std::move(o));
  m_StagingBuffer = std::move(o.m_StagingBuffer);

  return *this;
}

bool VulkanUniformBuffer::write(const VulkanCommandBuffer &commandBuffer, size_t sizeBytes, const void *data) {
  sizeBytes = std::min(sizeBytes, static_cast<size_t>(size()));

  void *ptr = m_StagingBuffer.map();
  memcpy(ptr, data, sizeBytes);
  m_StagingBuffer.unmap();

  return VulkanBuffer::copyBuffer(m_StagingBuffer, *this, commandBuffer);
}

void VulkanUniformBuffer::cleanup() {
  m_StagingBuffer.cleanup();
  VulkanBuffer::cleanup();
}

} // namespace Froth
