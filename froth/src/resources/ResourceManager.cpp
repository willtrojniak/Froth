#include "ResourceManager.h"
#include "src/core/logger/Logger.h"
#include "src/resources/ResourceImporter.h"

#include <map>

namespace Froth {

static std::map<std::filesystem::path, ResourceType> s_ResourceExtensionMap = {
    {".obj", ResourceType::Mesh},
    {".spv", ResourceType::ShaderSource},
    {".vert", ResourceType::ShaderSource},
    {".frag", ResourceType::ShaderSource},
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

void ResourceManager::update() {
  for (auto &registryEntry : m_ResourceRegistry) {
  }
}

std::shared_ptr<Resource> ResourceManager::getResource(ResourceHandle<Resource> handle) const {
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

ResourceMetadata ResourceManager::getMetadata(ResourceHandle<Resource> handle) const {
  static ResourceMetadata s_NullMetadata{};
  auto it = m_ResourceRegistry.find(handle);
  if (it == m_ResourceRegistry.end()) {
    return s_NullMetadata;
  }

  return it->second;
}

ResourceType ResourceManager::getResourceType(ResourceHandle<Resource> handle) const {
  if (!isHandleValid(handle))
    return ResourceType::None;

  return m_ResourceRegistry.at(handle).Type;
}

bool ResourceManager::isHandleValid(ResourceHandle<Resource> handle) const {
  return handle != 0 && m_ResourceRegistry.find(handle) != m_ResourceRegistry.end();
}

bool ResourceManager::isHandleLoaded(ResourceHandle<Resource> handle) const {
  return m_LoadedResources.find(handle) != m_LoadedResources.end();
}

ResourceHandle<Resource> ResourceManager::importResource(const std::filesystem::path &filepath) {
  ResourceMetadata metadata{
      .Type = getResourceTypeFromExtension(filepath.extension()),
      .FilePath = filepath,
      .IsDirty = false,
  };

  if (metadata.Type == ResourceType::None) {
    FROTH_ERROR("ResourceManager could not import resource");
  }

  std::shared_ptr<Resource> resource = ResourceImporter::ImportResource(metadata);
  if (!resource) {
    FROTH_WARN("Failed to import resource from %s", filepath.c_str());
    return 0;
  }

  ResourceHandle<Resource> handle{};
  m_ResourceRegistry[handle] = metadata;
  // TODO: Consider lazy loading resources
  m_LoadedResources[handle] = resource;

  // TODO: Persist resource registry to disk

  FROTH_DEBUG("Resource Manager: Imported resource at %s", filepath.c_str());

  return handle;
}

} // namespace Froth
