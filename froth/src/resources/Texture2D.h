#pragma once

#include "Resource.h"
#include "src/core/Types.h"
#include "src/renderer/vulkan/VulkanTexture.h"

namespace Froth {

class Texture2D : public Resource {
public:
  Texture2D() = default;
  Texture2D(const Extent2D &extent, const void *data);
  virtual ResourceType type() const noexcept override final { return ResourceType::Texture2D; }

  // TODO: Abstract
  const VulkanImageView &view() const { return m_Texture.view(); }

private:
  // TODO: Abstract to interface
  VulkanTexture m_Texture;
};

} // namespace Froth
