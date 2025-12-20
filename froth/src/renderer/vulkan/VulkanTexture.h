#pragma once

#include "VulkanImage.h"
#include "src/renderer/vulkan/VulkanImageView.h"

namespace Froth {

class VulkanTexture : public VulkanImage {
public:
  friend class VulkanRenderer;
  VulkanTexture() = default;
  const VulkanImageView &view() const { return m_View; }

protected:
  VulkanTexture(const VkExtent3D &extent, VkFormat format);

  VulkanImageView m_View;
};

} // namespace Froth
