#pragma once

#include "src/core/events/ApplicationEvent.h"
#include "src/platform/window/Window.h"
#include "src/renderer/Renderer.h"
#include "src/renderer/vulkan/VulkanCommandBuffer.h"
#include "src/renderer/vulkan/VulkanCommandPool.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include "src/renderer/vulkan/VulkanDescriptorPool.h"
#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/renderer/vulkan/VulkanDevice.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include "src/renderer/vulkan/VulkanImageView.h"
#include "src/renderer/vulkan/VulkanIndexBuffer.h"
#include "src/renderer/vulkan/VulkanPipeline.h"
#include "src/renderer/vulkan/VulkanPipelineLayout.h"
#include "src/renderer/vulkan/VulkanSampler.h"
#include "src/renderer/vulkan/VulkanSurface.h"
#include "src/renderer/vulkan/VulkanSwapchainManager.h"
#include "src/renderer/vulkan/VulkanTexture.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/materials/Material.h"
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

  virtual bool beginFrame() override;
  virtual void beginRenderPass() override;
  virtual void endRenderPass() override;
  virtual void endFrame() override;

  void pushConstants(VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void *pData) const;
  virtual void bindMaterial(const Material &mat) override;

  VulkanCommandPool &getCurrentCommandPool();
  VulkanCommandPool &getGraphicsCommandPool() { return m_GraphicsCommandPool; };
  VulkanDescriptorPool &getDescriptorPool() { return m_DescriptorPool; }
  void setDescriptorTexture(const VulkanSampler &sampler, const VulkanImageView &view);

  void bindVertexBuffer(const VulkanVertexBuffer &buffer) const;
  void bindIndexBuffer(const VulkanIndexBuffer &buffer) const;

  VulkanTexture createTexture(const VkExtent3D &extent, VkFormat format, const void *data);

protected:
  /* Creates a Vulkan Renderer backend
   *
   * @returns Vulkan Renderer Backend
   * @throws std::runtime_error if Window Surface cannot be created
   */
  static std::unique_ptr<VulkanRenderer> create(const Window &window);
  static void init(const Window &window) { VulkanContext::get().init(window); };

private:
  VulkanDescriptorSetLayout m_DescriptorSetLayout;
  VulkanCommandPool m_GraphicsCommandPool;
  VulkanSwapchainManager m_SwapchainManager;
  VulkanDescriptorPool m_DescriptorPool;
  std::vector<VkDescriptorSet> m_DescriptorSets;
  VulkanImage m_BlankImage;
  VulkanImageView m_BlankImageView;
  VulkanSampler m_Sampler;
  std::unique_ptr<VulkanPipelineLayout> m_PipelineLayout = nullptr;
  std::unique_ptr<VulkanPipeline> m_Pipeline = nullptr;

  std::unique_ptr<VulkanPipeline> buildPipeline(const Material &mat);
};
} // namespace Froth
