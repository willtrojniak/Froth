#include "VulkanInstance.h"
#include "Defines.h"
#include "src/core/logger/Logger.h"
#include "src/platform/window/Window.h"
#include "src/renderer/vulkan/VulkanContext.h"

#include <set>
#include <vector>

namespace Froth {

static const char *VK_LAYER_KHRONOS_validation = "VK_LAYER_KHRONOS_validation";

bool hasExtensions(const std::vector<const char *> &extensions) noexcept;
bool getRequiredExtensions(std::vector<const char *> &extensions) noexcept;
bool hasLayers(const std::vector<const char *> &layers) noexcept;

VulkanInstance::VulkanInstance(const VkAllocationCallbacks *allocator) {
  VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.pApplicationName = "App Name";                 // TODO: Make configurable
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0); // TODO: Make configurable
  appInfo.pEngineName = "Froth Game Engine";             // TODO: Make constant
  appInfo.engineVersion = VK_MAKE_VERSION(FROTH_VERSION_MAJOR, FROTH_VERSION_MINOR, FROTH_VERSION_PATCH);
  appInfo.apiVersion = VK_API_VERSION_1_3; // TODO: Setup in build settings

  // Extensions
  std::vector<const char *> extensions;

  if (!getRequiredExtensions(extensions)) {
    FROTH_ERROR("Failed to retreive required Vulkan extensions")
  }

  if (!hasExtensions(extensions)) {
    FROTH_ERROR("Vulkan does not support required extensions");
  }

  // Validation Layers
  std::vector<const char *> validationLayers{};
#ifdef FROTH_DEBUG
  validationLayers.emplace_back(VK_LAYER_KHRONOS_validation);
#endif // FROTH_DEBUG
  if (!hasLayers(validationLayers)) {
    FROTH_ERROR("Vulkan does not support required layers");
  }

  VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();
  // TODO: Only on MacOS
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  createInfo.ppEnabledLayerNames = validationLayers.data();

  if (vkCreateInstance(&createInfo, allocator, &m_Instance) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create Vulkan instance")
  };
}

VulkanInstance::VulkanInstance(VulkanInstance &&o) noexcept
    : m_Instance(o.m_Instance) {
  o.m_Instance = nullptr;
}
VulkanInstance &VulkanInstance::operator=(VulkanInstance &&o) noexcept {
  m_Instance = o.m_Instance;
  o.m_Instance = nullptr;
  return *this;
}

void VulkanInstance::cleanup(const VkAllocationCallbacks *allocator) {
  vkDestroyInstance(m_Instance, allocator);
  m_Instance = nullptr;
  FROTH_DEBUG("Cleaned up Vulkan Instance");
}

VulkanInstance::~VulkanInstance() {
  if (m_Instance != nullptr) {
    FROTH_WARN("Vulkan Instance was never cleaned up");
  }
}

bool getRequiredExtensions(std::vector<const char *> &extensions) noexcept {
  uint32_t requiredWindowExtensionsCount;
  const char **requiredWindowExtensions = Window::requiredVulkanExtensions(requiredWindowExtensionsCount);
  if (requiredWindowExtensions == nullptr) {
    return false;
  }

  extensions.resize(requiredWindowExtensionsCount);
  for (uint32_t i = 0; i < requiredWindowExtensionsCount; i++) {
    extensions[i] = requiredWindowExtensions[i];
  }

#ifdef FROTH_PLATFORM_MACOS
  FROTH_DEBUG("Requiring MacOS Vulkan Extensions");
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif // FROTH_PLATFORM_MACOS

  return true;
}

bool hasExtensions(const std::vector<const char *> &extensions) noexcept {
  uint32_t availableExtensionsCount;
  if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr) != VK_SUCCESS) {
    return false;
  }

  std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
  if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data()) != VK_SUCCESS) {
    return false;
  };

  std::set<std::string> extensionsSet(extensions.begin(), extensions.end());
  for (const auto &e : availableExtensions) {
    extensionsSet.erase(e.extensionName);
  }

  return extensionsSet.empty();
}

bool hasLayers(const std::vector<const char *> &layers) noexcept {
  uint32_t availableLayerCount;
  if (vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr) != VK_SUCCESS) {
    return false;
  }

  std::vector<VkLayerProperties> availableLayers(availableLayerCount);
  if (vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()) != VK_SUCCESS) {
    return false;
  };

  std::set<std::string> layerSet(layers.begin(), layers.end());
  for (const auto &l : availableLayers) {
    layerSet.erase(l.layerName);
  }

  return layerSet.empty();
}

} // namespace Froth
