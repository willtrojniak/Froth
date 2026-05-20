#include "VulkanContext.h"
#include "Defines.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanDevice.h"
#include "src/renderer/vulkan/VulkanInstance.h"
#include "src/renderer/vulkan/VulkanSurface.h"

namespace Froth {

void VulkanContext::cleanup() {
  m_Device.cleanup();
  m_Instance.cleanup(m_Allocator);
  m_Allocator = nullptr;
}

void VulkanContext::init(const Window &window) {
  m_Allocator = nullptr; // TODO: Configurable
  m_Instance = VulkanInstance(m_Allocator);

  // Physical Device Selection & Logical Device Creation
  {
    VulkanSurface surface = window.createVulkanSurface();

    // Physical Device
    VulkanDevice::PhysicalDeviceProperties requirements{};
    requirements.graphics = true;
    requirements.present = true;
    requirements.compute = true;
    requirements.transfer = true;

    requirements.extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#if FROTH_PLATFORM_MACOS
    FROTH_DEBUG("Vulkan Device Enabling MacOS Portability Subset Extension");
    requirements.extensions.emplace_back("VK_KHR_portability_subset");
#endif // FROTH_PLATFORM_MACOS

#if FROTH_BUILD_DEBUG
    FROTH_DEBUG("Vulkan Device Enabling Validation Extension");
    requirements.layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif // FROTH_BUILD_DEBUG

    requirements.samplerAnisotropy = true; // TODO: This can probably be made optional?
    // Device Requirements

    VkPhysicalDevice physicalDevice = VulkanDevice::pickPhysicalDevice(m_Instance, surface, requirements);
    if (physicalDevice == nullptr) {
      FROTH_ERROR("Failed to find suitable Vulkan phyiscal device")
    }

    // Logical Device Creation
    VulkanDevice::QueueFamilies queueFamilies = VulkanDevice::getPhysicalDeviceQueueFamilies(physicalDevice, surface);
    m_Device = VulkanDevice(m_Allocator, physicalDevice, queueFamilies, requirements);
    FROTH_INFO("Initialized Vulkan Context")
  }
}

} // namespace Froth
