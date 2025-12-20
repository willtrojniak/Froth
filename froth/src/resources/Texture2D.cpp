#include "Texture2D.h"
#include "src/core/Application.h"
#include "src/renderer/vulkan/VulkanRenderer.h"

namespace Froth {
Texture2D::Texture2D(const Extent2D &extent, const void *data) {
  VulkanRenderer &renderer = Application::getInstance().renderer();

  m_Texture = renderer.createTexture(VkExtent3D{
                                         .width = extent.width,
                                         .height = extent.height,
                                         .depth = 1,
                                     },
                                     VK_FORMAT_R8G8B8A8_SRGB, data);
}

} // namespace Froth
