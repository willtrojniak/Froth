#pragma once

#include "VulkanBuffer.h"

namespace Froth {
class VulkanUniformBuffer : public VulkanBuffer {
public:
  VulkanUniformBuffer() = default;
  VulkanUniformBuffer(const VkDeviceSize &size);
  ~VulkanUniformBuffer();
  VulkanUniformBuffer(VulkanUniformBuffer const &) = delete;
  VulkanUniformBuffer &operator=(VulkanUniformBuffer const &) = delete;
  VulkanUniformBuffer(VulkanUniformBuffer &&) noexcept;
  VulkanUniformBuffer &operator=(VulkanUniformBuffer &&) noexcept;

  void write(size_t sizeBytes, const void *data);

  virtual void cleanup() override final;

private:
  void *m_DataPtr = nullptr;
};
} // namespace Froth
