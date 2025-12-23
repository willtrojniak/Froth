#include "VulkanSwapchainManager.h"
#include "src/core/logger/Logger.h"
#include "src/platform/window/Window.h"
#include "src/renderer/vulkan/VulkanContext.h"

#include <cstdint>

namespace Froth {
static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

VulkanSwapchainManager::VulkanSwapchainManager(const Window &win)
    : m_Surface(win.createVulkanSurface()),
      m_Swapchain(VulkanSwapchain::create(m_Surface, nullptr)),
      m_DepthImage(VulkanImage::CreateInfo{
          .extent = {.width = m_Swapchain.extent().width, .height = m_Swapchain.extent().height, .depth = 1},
          .format = VK_FORMAT_D32_SFLOAT,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
          .memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      }),
      m_DepthImageView(m_DepthImage.createView(VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT)),
      m_RenderPass(m_Swapchain.format().format, m_DepthImageView.format()) {
  createFramebuffers();
  createFrameData();
}

VulkanSwapchainManager::VulkanSwapchainManager(VulkanSwapchainManager &&o) noexcept
    : m_Surface(std::move(o.m_Surface)),
      m_Swapchain(std::move(o.m_Swapchain)),
      m_DepthImage(std::move(o.m_DepthImage)),
      m_DepthImageView(std::move(o.m_DepthImageView)),
      m_RenderPass(std::move(o.m_RenderPass)),
      m_CurrentFrame(o.m_CurrentFrame),
      m_CurrentImageIndex(o.m_CurrentImageIndex),
      m_Framebuffers(std::move(o.m_Framebuffers)),
      m_ImageAvailableSemaphores(std::move(o.m_ImageAvailableSemaphores)),
      m_RenderCompleteSemaphores(std::move(o.m_RenderCompleteSemaphores)),
      m_FrameInFlightFences(std::move(o.m_FrameInFlightFences)),
      m_CommandPools(std::move(o.m_CommandPools)),
      m_CommandBuffers(std::move(o.m_CommandBuffers)),
      m_ShouldRebuild(o.m_ShouldRebuild) {
  o.m_ShouldRebuild = false;
}

VulkanSwapchainManager &VulkanSwapchainManager::operator=(VulkanSwapchainManager &&o) noexcept {
  m_Surface = std::move(o.m_Surface);
  m_Swapchain = std::move(o.m_Swapchain);
  m_DepthImage = std::move(o.m_DepthImage);
  m_DepthImageView = std::move(o.m_DepthImageView);
  m_RenderPass = std::move(o.m_RenderPass);
  m_CurrentFrame = o.m_CurrentFrame;
  m_CurrentImageIndex = o.m_CurrentImageIndex;
  m_Framebuffers = std::move(o.m_Framebuffers);
  m_ImageAvailableSemaphores = std::move(o.m_ImageAvailableSemaphores);
  m_RenderCompleteSemaphores = std::move(o.m_RenderCompleteSemaphores);
  m_FrameInFlightFences = std::move(o.m_FrameInFlightFences);
  m_CommandPools = std::move(o.m_CommandPools);
  m_CommandBuffers = std::move(o.m_CommandBuffers);
  m_ShouldRebuild = o.m_ShouldRebuild;
  o.m_ShouldRebuild = false;

  return *this;
}

VulkanSwapchainManager::~VulkanSwapchainManager() {
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT && i < m_CommandBuffers.size(); i++) {
    m_CommandBuffers[i].cleanup(m_CommandPools[i]);
    FROTH_DEBUG("Cleaned up swapchain command buffer");
  }
}

void VulkanSwapchainManager::onWindowFramebufferResize(uint32_t width, uint32_t height) {
  m_Surface.resize(width, height);
  rebuild();
}

void VulkanSwapchainManager::rebuild() {
  FROTH_DEBUG("Rebuilding Swap Chain");
  vkDeviceWaitIdle(VulkanContext::get().device());

  m_Framebuffers.clear();
  m_DepthImageView.cleanup();
  m_DepthImage.cleanup();

  VulkanSwapchain oldSwapchain = std::move(m_Swapchain);
  m_Swapchain = VulkanSwapchain::create(m_Surface, &oldSwapchain);
  m_DepthImage = VulkanImage(VulkanImage::CreateInfo{
      .extent = {.width = m_Swapchain.extent().width, .height = m_Swapchain.extent().height, .depth = 1},
      .format = VK_FORMAT_D32_SFLOAT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .memPropFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  });

  m_DepthImageView = m_DepthImage.createView(VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

  // TODO: Rebuild render pass if dynamic viewport is not on

  createFramebuffers();
  m_ShouldRebuild = false;
  FROTH_DEBUG("Rebuilt Swap Chain");
}

void VulkanSwapchainManager::createFramebuffers() {

  std::vector<VkImageView> framebufferAttachments(2);
  framebufferAttachments[1] = m_DepthImageView;
  m_Framebuffers.reserve(m_Swapchain.views().size());

  for (size_t i = 0; i < m_Swapchain.views().size(); i++) {
    // Framebuffer
    framebufferAttachments[0] = m_Swapchain.views()[i];
    m_Framebuffers.emplace_back(m_RenderPass, m_Swapchain.extent(), framebufferAttachments);
  }
}

void VulkanSwapchainManager::createFrameData() {
  VulkanContext &vctx = VulkanContext::get();

  m_ImageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
  m_RenderCompleteSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
  m_FrameInFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
  m_CommandPools.reserve(MAX_FRAMES_IN_FLIGHT);
  m_CommandBuffers.reserve(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    // Semaphores + Fences
    m_ImageAvailableSemaphores.emplace_back();
    m_RenderCompleteSemaphores.emplace_back();
    m_FrameInFlightFences.emplace_back(true);

    // TODO: Seperate into graphics + present + etc...
    m_CommandPools.emplace_back(vctx.device().getQueueFamilies().graphics.index);

    // Command Buffer
    m_CommandBuffers.push_back(m_CommandPools[i].AllocateCommandBuffer());
  }
}

bool VulkanSwapchainManager::beginFrame() {
  VulkanContext &vctx = VulkanContext::get();

  VkFence inFlightFence[] = {m_FrameInFlightFences[m_CurrentFrame]};
  vkWaitForFences(vctx.device(), 1, inFlightFence, VK_TRUE, UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(vctx.device(), m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    rebuild();
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    FROTH_ERROR("Failed to acquire swapchain image");
  }

  vkResetFences(vctx.device(), 1, inFlightFence);
  vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

  // Record command buffer
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;
  if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void VulkanSwapchainManager::endFrame() {
  if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS) {
    FROTH_ERROR("Failed to record command buffer");
  }
  VulkanContext &vctx = VulkanContext::get();

  // Submit draw command buffer
  VkSubmitInfo submitInfo{};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore imageAvailableSemaphore[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
  VkSemaphore renderFinishedSemaphore[] = {m_RenderCompleteSemaphores[m_CurrentFrame]};
  VkCommandBuffer commandBuffer[] = {m_CommandBuffers[m_CurrentFrame]};

  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = imageAvailableSemaphore;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = commandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = renderFinishedSemaphore;
  if (vkQueueSubmit(vctx.device().getQueueFamilies().graphics.queue, 1, &submitInfo, m_FrameInFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
    FROTH_ERROR("Failed to submit draw command buffer");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = renderFinishedSemaphore;

  VkSwapchainKHR swapchains[] = {m_Swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &m_CurrentImageIndex;
  presentInfo.pResults = nullptr;

  VkResult result = vkQueuePresentKHR(vctx.device().getQueueFamilies().present.queue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    rebuild();
  } else if (result != VK_SUCCESS) {
    FROTH_ERROR("Failed to present swap chain image");
  }

  m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace Froth
