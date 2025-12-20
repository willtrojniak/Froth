#include "VulkanTexture.h"
#include <cstring>

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
} // namespace Froth
