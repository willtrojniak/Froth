#pragma once

#include "src/renderer/vulkan/VulkanShaderPipeline.h"
#include "src/resources/Resource.h"

#include <map>

namespace Froth {

class VulkanShaderPipelineManager {
public:
  struct ShaderPipelineKey {
    ResourceHandle vertexShaderHandle;
    ResourceHandle fragmentShaderHandle;

    bool operator<(const ShaderPipelineKey &o) const {
      return vertexShaderHandle < o.vertexShaderHandle || (vertexShaderHandle == o.vertexShaderHandle && fragmentShaderHandle < o.fragmentShaderHandle);
    };
  };

  VulkanShaderPipelineManager() = default;
  const VulkanShaderPipeline &registerMaterial(const Material &mat, const VulkanSwapchainManager &swapchain);
  const VulkanShaderPipeline &getShader(const Material &mat, const VulkanSwapchainManager &swapchain);

private:
  static ShaderPipelineKey getPipelineKey(const Material &mat);

  std::map<ShaderPipelineKey, VulkanShaderPipeline> m_Cache;
};

} // namespace Froth

namespace std {
template <>
struct std::hash<Froth::VulkanShaderPipelineManager::ShaderPipelineKey> {
  std::size_t operator()(const Froth::VulkanShaderPipelineManager::ShaderPipelineKey &key) const noexcept {
    // FIXME: Better hash?
    return hash<uint64_t>()(static_cast<uint64_t>(key.fragmentShaderHandle + key.vertexShaderHandle));
  }
};
} // namespace std
