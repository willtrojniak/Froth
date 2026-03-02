#pragma once

#include "src/core/UUID.h"

#include <string_view>

namespace Froth {
enum class ResourceType : uint16_t {
  None = 0,
  Mesh,
  ShaderSource,
  Texture2D,
};

class Resource {
public:
  virtual ResourceType type() const noexcept = 0;

  static std::string_view ResourceTypeToString(ResourceType type);
};

template <std::derived_from<Resource> T>
using ResourceHandle = UUID;

} // namespace Froth
