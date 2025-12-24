#pragma once
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanFence.h"
#include "VulkanFramebuffer.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"
#include "VulkanSemaphore.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include <cstdint>
#include <vector>

namespace Froth {

class VulkanSwapchainManager {
public:
  VulkanSwapchainManager() = default;
  VulkanSwapchainManager(const Window &window, uint32_t maxFramesInFlight);
  VulkanSwapchainManager(const VulkanSwapchainManager &) = delete;
  VulkanSwapchainManager &operator=(const VulkanSwapchainManager &) = delete;
  VulkanSwapchainManager(VulkanSwapchainManager &&) noexcept;
  VulkanSwapchainManager &operator=(VulkanSwapchainManager &&) noexcept;

  ~VulkanSwapchainManager();

  void setShouldRebuild() { m_ShouldRebuild = true; }
  void onWindowFramebufferResize(uint32_t width, uint32_t height);
  void rebuild();

  const VulkanSurface &surface() const { return m_Surface; }
  const VulkanSwapchain &swapchain() const { return m_Swapchain; }
  const VulkanRenderPass &renderpass() const { return m_RenderPass; }
  const std::vector<VulkanFramebuffer> &framebuffers() const { return m_Framebuffers; }

  bool beginFrame();
  void endFrame();

  uint32_t currentFrame() const { return m_CurrentFrame; }
  VulkanCommandPool &currentCommandPool() { return m_CommandPools[m_CurrentFrame]; }
  const VulkanCommandBuffer &currentCommandBuffer() const { return m_CommandBuffers[m_CurrentFrame]; }
  const VulkanFramebuffer &currentFramebuffer() const { return m_Framebuffers[m_CurrentImageIndex]; }
  uint32_t maxFramesInFlight() const { return m_MaxFramesInFlight; }
  void presentImage();

private:
  VulkanSurface m_Surface;
  VulkanSwapchain m_Swapchain;
  VulkanImage m_DepthImage;
  VulkanImageView m_DepthImageView;
  VulkanRenderPass m_RenderPass;

  uint32_t m_CurrentFrame = 0;      // [0, MAX_FRAMES_IN_FLIGHT)
  uint32_t m_CurrentImageIndex = 0; // [0, Swapchain Image Count)

  // Frame data
  std::vector<VulkanFramebuffer> m_Framebuffers;
  std::vector<VulkanSemaphore> m_ImageAvailableSemaphores;
  std::vector<VulkanSemaphore> m_RenderCompleteSemaphores;
  std::vector<VulkanFence> m_FrameInFlightFences;
  std::vector<VulkanCommandPool> m_CommandPools;
  std::vector<VulkanCommandBuffer> m_CommandBuffers;

  bool m_ShouldRebuild = false;
  void createFrameData();
  void createFramebuffers();
  uint32_t m_MaxFramesInFlight;
};

} // namespace Froth
