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
#include "src/renderer/vulkan/VulkanSurface.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/Mesh.h"
#include "src/resources/Shader.h"
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

  uint32_t currentFrame() const { return m_SwapchainManager.currentFrame(); }
  virtual bool beginFrame() override;
  virtual void beginRenderPass() override;
  virtual void endRenderPass() override;
  virtual void endFrame() override;

  VulkanCommandPool &getGraphicsCommandPool() { return m_GraphicsCommandPool; };

  void bindShader(const Shader &shader) const;
  void bindMesh(const Mesh &mesh) const;
  void pushConstants(const Shader &shader, VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void *pData) const;
  void bindDescriptorSets(const Shader &shader, uint32_t start, const std::vector<VkDescriptorSet> &sets) const;
  void bindDescriptorSets(const Shader &shader, uint32_t start, const std::vector<VkDescriptorSet> &sets, const std::vector<uint32_t> &offsets) const;
  void bindVertexBuffer(const VulkanVertexBuffer &buffer) const;
  void bindIndexBuffer(const VulkanIndexBuffer &buffer) const;

  Shader createShader(const VulkanShaderModule &vert, const VulkanShaderModule &frag);

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
};
} // namespace Froth
