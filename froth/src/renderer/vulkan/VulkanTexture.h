#pragma once

#include "VulkanImage.h"
#include "src/renderer/vulkan/VulkanImageView.h"

namespace Froth {

class VulkanTexture : public VulkanImage {
public:
  VulkanTexture() = default;
  const VulkanImageView &view() const { return m_View; }
  static VulkanTexture create(const VkExtent3D &extent, VkFormat format, const void *data);

private:
  VulkanTexture(const VkExtent3D &extent, VkFormat format);

  VulkanImageView m_View;
};

} // namespace Froth
