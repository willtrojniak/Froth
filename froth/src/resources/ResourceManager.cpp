#include "ResourceManager.h"
#include "src/core/logger/Logger.h"
#include "src/resources/ResourceImporter.h"

#include <map>

namespace Froth {

static std::map<std::filesystem::path, ResourceType> s_ResourceExtensionMap = {
    {".png", ResourceType::Texture2D},
    {".jpg", ResourceType::Texture2D},
    {".jpeg", ResourceType::Texture2D},
};

ResourceType ResourceManager::getResourceTypeFromExtension(const std::filesystem::path &extension) noexcept {
  if (s_ResourceExtensionMap.find(extension) == s_ResourceExtensionMap.end()) {
    FROTH_WARN("Could not find ResourceType for %s", extension.c_str());
    return ResourceType::None;
  }
  return s_ResourceExtensionMap.at(extension);
}

std::shared_ptr<Resource> ResourceManager::getResource(ResourceHandle handle) {
  if (!isHandleValid(handle))
    return nullptr;

  std::shared_ptr<Resource> pResource;
  if (isHandleLoaded(handle)) {
    pResource = m_LoadedResources.at(handle);
  } else {
    const ResourceMetadata &metadata = getMetadata(handle);
    // asset = TODO: Finish, right now all resources are loaded
  }

  return pResource;
}

ResourceMetadata ResourceManager::getMetadata(ResourceHandle handle) {
  static ResourceMetadata s_NullMetadata{};
  auto it = m_ResourceRegistry.find(handle);
  if (it == m_ResourceRegistry.end()) {
    return s_NullMetadata;
  }

  return it->second;
}

ResourceType ResourceManager::getResourceType(ResourceHandle handle) const {
  if (!isHandleValid(handle))
    return ResourceType::None;

  return m_ResourceRegistry.at(handle).Type;
}

bool ResourceManager::isHandleValid(ResourceHandle handle) const {
  return handle != 0 && m_ResourceRegistry.find(handle) != m_ResourceRegistry.end();
}

bool ResourceManager::isHandleLoaded(ResourceHandle handle) const {
  return m_LoadedResources.find(handle) != m_LoadedResources.end();
}

std::shared_ptr<Resource> ResourceManager::importResource(const std::filesystem::path &filepath) {
  ResourceMetadata metadata{
      .Type = getResourceTypeFromExtension(filepath.extension()),
      .FilePath = filepath,
  };

  if (metadata.Type == ResourceType::None) {
    FROTH_ERROR("ResourceManager could not import resource");
  }

  std::shared_ptr<Resource> resource = ResourceImporter::ImportResource(metadata);
  if (!resource) {
    FROTH_WARN("Failed to import resource from %s", filepath.c_str());
    return nullptr;
  }

  m_ResourceRegistry[resource->handle()] = metadata;
  // TODO: Consider lazy loading resources
  m_LoadedResources[resource->handle()] = resource;

  // TODO: Persist resource registry to disk

  return resource;
}

} // namespace Froth
