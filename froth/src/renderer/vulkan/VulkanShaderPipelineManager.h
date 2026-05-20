#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/renderer/vulkan/VulkanShaderModuleManager.h"
#include "src/renderer/vulkan/VulkanShaderPipeline.h"
#include "src/resources/Material.h"
#include "src/resources/Resource.h"

#include <map>

namespace Froth {

class VulkanShaderPipelineManager {
public:
  struct ShaderPipelineKey {
    ResourceHandle<ShaderSource> vertexShaderHandle;
    ResourceHandle<ShaderSource> fragmentShaderHandle;

    bool operator<(const ShaderPipelineKey &o) const {
      return vertexShaderHandle < o.vertexShaderHandle || (vertexShaderHandle == o.vertexShaderHandle && fragmentShaderHandle < o.fragmentShaderHandle);
    };
  };

  VulkanShaderPipelineManager() = default;
  const VulkanShaderPipeline &getOrCreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                  const Material &mat,
                                                  const VulkanSwapchainManager &swapchain);
  const std::optional<VulkanShaderPipeline> recreatePipeline(VulkanShaderModuleManager &shaderModuleManager,
                                                             const Material &mat,
                                                             const VulkanSwapchainManager &swapchain);
  void onShaderLoaded(VulkanShaderModuleManager &shaderModuleManager,
                      ResourceHandle<ShaderSource> handle,
                      const VulkanSwapchainManager &swapchain);
  const std::expected<std::vector<VkDescriptorSetLayout>, bool> getMaterialDescSetLayouts(const Material &mat);

private:
  struct PipelineData {
    VulkanShaderPipeline pipeline;
    std::vector<VulkanShaderModule::DescriptorSetLayoutData> descSets;
  };

  static ShaderPipelineKey getPipelineKey(const Material &mat);
  PipelineData createPipeline(VulkanShaderModuleManager &shaderModuleManager,
                              const Material &mat,
                              const VulkanSwapchainManager &swapchain);

  std::unordered_map<VulkanShaderModule::DescriptorSetLayoutData, VulkanDescriptorSetLayout> m_LayoutCache;
  std::map<ResourceHandle<ShaderSource>, std::vector<Material>> m_ShaderDependents;
  std::map<ShaderPipelineKey, PipelineData> m_PipelineCache;
};

} // namespace Froth

namespace std {
template <>
struct hash<Froth::VulkanShaderPipelineManager::ShaderPipelineKey> {
  std::size_t operator()(const Froth::VulkanShaderPipelineManager::ShaderPipelineKey &key) const noexcept {
    // FIXME: Better hash?
    return hash<uint64_t>()(static_cast<uint64_t>(key.fragmentShaderHandle + key.vertexShaderHandle));
  }
};
} // namespace std
