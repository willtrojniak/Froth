#pragma once

#include "Resource.h"
#include "ResourceMetadata.h"
#include "src/core/events/Event.h"

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Froth {

using ResourceRegistry = std::unordered_map<ResourceHandle<Resource>, ResourceMetadata>;
using ResourceMap = std::unordered_map<ResourceHandle<Resource>, std::shared_ptr<Resource>>;

class ResourceManager {
public:
  ResourceManager() = default;

  void setEventCallbackFunction(const std::function<void(const Event &e)> &);
  void onUpdate(float deltaT);
  void pollFiles();

  template <std::derived_from<Resource> T>
  std::shared_ptr<T> getResource(ResourceHandle<T> handle) {
    auto resource = getResource(handle);
    return std::static_pointer_cast<T>(resource);
  }

  ResourceMetadata getMetadata(ResourceHandle<Resource> handle) const;
  ResourceType getResourceType(ResourceHandle<Resource> handle) const;

  bool isHandleValid(ResourceHandle<Resource> handle) const;
  bool isHandleLoaded(ResourceHandle<Resource> handle) const;

  template <std::derived_from<Resource> T>
  ResourceHandle<T> importResource(const std::filesystem::path &filepath) {
    auto handle = importResource(filepath);
    return (ResourceHandle<T>)(handle);
  }

private:
  ResourceRegistry m_ResourceRegistry;
  ResourceMap m_LoadedResources;
  float m_SecondsSinceLastPoll = 0.f;
  std::function<void(const Event &)> m_EventCallbackFn = nullptr;

  static ResourceType getResourceTypeFromExtension(const std::filesystem::path &ext) noexcept;
  std::shared_ptr<Resource> loadResource(ResourceHandle<Resource> handle);
  std::shared_ptr<Resource> getResource(ResourceHandle<Resource> handle) const;
  ResourceHandle<Resource> importResource(const std::filesystem::path &filepath);
};

} // namespace Froth
