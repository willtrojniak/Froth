#pragma once

#include "Resource.h"
#include "ResourceMetadata.h"

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Froth {

using ResourceRegistry = std::unordered_map<ResourceHandle, ResourceMetadata>;
using ResourceMap = std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>;

class ResourceManager {
public:
  ResourceManager() = default;

  template <std::derived_from<Resource> T>
  std::shared_ptr<T> getResource(ResourceHandle handle) {
    auto resource = getResource(handle);
    return std::static_pointer_cast<T>(resource);
  }

  std::shared_ptr<Resource> getResource(ResourceHandle handle);
  ResourceMetadata getMetadata(ResourceHandle handle);
  ResourceType getResourceType(ResourceHandle handle) const;

  bool isHandleValid(ResourceHandle handle) const;
  bool isHandleLoaded(ResourceHandle handle) const;

  template <std::derived_from<Resource> T>
  std::shared_ptr<T> importResource(const std::filesystem::path &filepath) {
    auto resource = importResource(filepath);
    return std::static_pointer_cast<T>(resource);
  }

  std::shared_ptr<Resource> importResource(const std::filesystem::path &filepath);

private:
  ResourceRegistry m_ResourceRegistry;
  ResourceMap m_LoadedResources;

  static ResourceType getResourceTypeFromExtension(const std::filesystem::path &ext) noexcept;
};

} // namespace Froth
