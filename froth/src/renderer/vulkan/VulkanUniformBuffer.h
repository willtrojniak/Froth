#pragma once

#include "VulkanBuffer.h"

namespace Froth {
class VulkanUniformBuffer : public VulkanBuffer {
public:
  VulkanUniformBuffer() = default;
  VulkanUniformBuffer(const VkDeviceSize &size);
  VulkanUniformBuffer(VulkanUniformBuffer const &) = delete;
  VulkanUniformBuffer &operator=(VulkanUniformBuffer const &) = delete;
  VulkanUniformBuffer(VulkanUniformBuffer &&) noexcept;
  VulkanUniformBuffer &operator=(VulkanUniformBuffer &&) noexcept;

  bool write(const VulkanCommandBuffer &commandBuffer, size_t sizeBytes, const void *data);

  virtual void cleanup() override final;

private:
  VulkanBuffer m_StagingBuffer;
};
} // namespace Froth
