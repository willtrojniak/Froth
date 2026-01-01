#pragma once

#include "src/core/UUID.h"

#include <string_view>

namespace Froth {

using ResourceHandle = UUID;

enum class ResourceType : uint16_t {
  None = 0,
  Mesh,
  ShaderModule,
  Texture2D,
};

class Resource {
public:
  ResourceHandle handle() const { return m_Handle; }
  virtual ResourceType type() const noexcept = 0;

  static std::string_view ResourceTypeToString(ResourceType type);

private:
  ResourceHandle m_Handle; // Auto generates on creation
};

} // namespace Froth
