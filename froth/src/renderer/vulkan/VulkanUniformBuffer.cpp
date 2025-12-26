
#include "VulkanUniformBuffer.h"
#include <algorithm>
#include <utility>

namespace Froth {
VulkanUniformBuffer::VulkanUniformBuffer(const VkDeviceSize &size)
    : VulkanBuffer(size,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
  m_DataPtr = VulkanBuffer::map();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
  cleanup();
}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBuffer &&o) noexcept
    : VulkanBuffer(std::move(o)) {}

VulkanUniformBuffer &VulkanUniformBuffer::operator=(VulkanUniformBuffer &&o) noexcept {
  VulkanBuffer::operator=(std::move(o));

  return *this;
}

void VulkanUniformBuffer::write(size_t sizeBytes, const void *data) {
  sizeBytes = std::min(sizeBytes, static_cast<size_t>(size()));

  memcpy(m_DataPtr, data, sizeBytes);
}

void VulkanUniformBuffer::cleanup() {
  VulkanBuffer::unmap();
  VulkanBuffer::cleanup();
}

} // namespace Froth
