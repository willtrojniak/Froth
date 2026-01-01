#include "Resource.h"

namespace Froth {

std::string_view Resource::ResourceTypeToString(ResourceType type) {
  switch (type) {
  case ResourceType::None:
    return "ResourceType::None";
  case ResourceType::Mesh:
    return "ResourceType::Mesh";
  case ResourceType::ShaderModule:
    return "ResourceType::ShaderModule";
  case ResourceType::Texture2D:
    return "ResourceType::Texture2D";
  default:
    return "ResourceType::<Invalid>";
  }
}
} // namespace Froth
