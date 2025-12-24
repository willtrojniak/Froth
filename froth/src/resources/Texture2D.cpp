#include "Texture2D.h"

namespace Froth {
Texture2D::Texture2D(const Extent2D &extent, const void *data)
    : m_Texture(VulkanTexture::create(VkExtent3D{
                                          .width = extent.width,
                                          .height = extent.height,
                                          .depth = 1,
                                      },
                                      VK_FORMAT_R8G8B8A8_SRGB, data)) {
}

} // namespace Froth
