#pragma once

#include "VulkanCommandBuffer.h"

namespace Froth {
class VulkanImageView;

class VulkanImage {
  friend class VulkanRenderer;

public:
  struct CreateInfo;

  VulkanImage() = default;
  VulkanImage(const CreateInfo &opts);
  VulkanImage(VulkanImage const &) = delete;
  VulkanImage &operator=(VulkanImage const &) = delete;
  VulkanImage(VulkanImage &&);
  VulkanImage &operator=(VulkanImage &&);
  ~VulkanImage();

  operator VkImage() const { return m_Image; }
  const VkExtent3D &extent() const { return m_Extent; }
  size_t sizeBytes() const {
    return m_Extent.width * m_Extent.height * m_Extent.depth * 4; // FIXME: Channels
  }
  void cleanup();
  VulkanImageView createView(VkFormat format, VkImageAspectFlags aspect) const;

  bool transitionLayout(VulkanCommandBuffer &commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

  struct CreateInfo {
    VkExtent3D extent;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags memPropFlags;
  };

private:
  VkImage m_Image = nullptr;
  VkExtent3D m_Extent{};
  VkDeviceMemory m_Memory = nullptr;
};

} // namespace Froth
