#pragma once

#include "src/core/events/ApplicationEvent.h"
#include "src/platform/window/Window.h"
#include "src/renderer/Renderer.h"
#include "src/renderer/vulkan/VulkanCommandBuffer.h"
#include "src/renderer/vulkan/VulkanCommandPool.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include "src/renderer/vulkan/VulkanDevice.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include "src/renderer/vulkan/VulkanIndexBuffer.h"
#include "src/renderer/vulkan/VulkanShaderPipelineManager.h"
#include "src/renderer/vulkan/VulkanSurface.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/Mesh.h"
#include <memory>
#include <vector>

namespace Froth {
class VulkanRenderer : public Renderer {
  friend class Renderer;

public:
  VulkanRenderer() = default;
  VulkanRenderer(const Window &window);
  static void shutdown();

  ~VulkanRenderer() override;
  VulkanRenderer(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer const &) = delete;
  VulkanRenderer(VulkanRenderer &&);
  VulkanRenderer &operator=(VulkanRenderer &&);

  virtual bool onEvent(const Event &e) override;
  bool onFramebufferResize(FramebufferResizeEvent &e);

  void registerMaterial(const Material &mat);

  uint32_t currentFrame() const { return m_SwapchainManager.currentFrame(); }
  virtual bool beginFrame() override;
  virtual void beginRenderPass() override;
  virtual void endRenderPass() override;
  virtual void endFrame() override;

  VulkanCommandPool &getGraphicsCommandPool() { return m_GraphicsCommandPool; };

  void bindMaterial(const Material &mat);
  void bindMesh(const Mesh &mesh);
  void pushConstants(const Material &mat, VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void *pData);
  void bindDescriptorSets(const Material &mat, uint32_t start, const std::vector<VkDescriptorSet> &sets);
  void bindDescriptorSets(const Material &mat, uint32_t start, const std::vector<VkDescriptorSet> &sets, const std::vector<uint32_t> &offsets);
  void bindVertexBuffer(const VulkanVertexBuffer &buffer);
  void bindIndexBuffer(const VulkanIndexBuffer &buffer);

protected:
  /* Creates a Vulkan Renderer backend
   *
   * @returns Vulkan Renderer Backend
   * @throws std::runtime_error if Window Surface cannot be created
   */
  static std::unique_ptr<VulkanRenderer> create(const Window &window);
  static void init(const Window &window) { VulkanContext::get().init(window); };

private:
  VulkanCommandPool m_GraphicsCommandPool;
  VulkanSwapchainManager m_SwapchainManager;
  VulkanShaderPipelineManager m_ShaderManager;
};
} // namespace Froth
