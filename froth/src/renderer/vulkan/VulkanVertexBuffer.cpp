#include "VulkanVertexBuffer.h"
#include "VulkanBuffer.h"
#include <algorithm>
#include <string.h>
#include <utility>

namespace Froth {

VulkanVertexBuffer::VulkanVertexBuffer(const VkDeviceSize &size)
    : VulkanBuffer(size,
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      m_StagingBuffer(size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanVertexBuffer &&o) noexcept
    : VulkanBuffer(std::move(o)),
      m_StagingBuffer(std::move(o.m_StagingBuffer)) {
}
VulkanVertexBuffer &VulkanVertexBuffer::operator=(VulkanVertexBuffer &&o) noexcept {
  VulkanBuffer::operator=(std::move(o));
  m_StagingBuffer = std::move(o.m_StagingBuffer);

  return *this;
}

bool VulkanVertexBuffer::write(const VulkanCommandBuffer &commandBuffer, size_t vertexDataSize, const void *vertexData) {
  vertexDataSize = std::min(vertexDataSize, static_cast<size_t>(size()));

  void *data = m_StagingBuffer.map();
  memcpy(data, vertexData, vertexDataSize);
  m_StagingBuffer.unmap();

  return VulkanBuffer::copyBuffer(m_StagingBuffer, *this, commandBuffer);
}

void VulkanVertexBuffer::cleanup() {
  m_StagingBuffer.cleanup();
  VulkanBuffer::cleanup();
}

} // namespace Froth
