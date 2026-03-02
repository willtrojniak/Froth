#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

namespace Froth {

class VulkanShaderModule {
public:
  enum class ShaderStage {
    VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
    FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT
  };

  VulkanShaderModule() = default;
  VulkanShaderModule(const std::vector<uint32_t> &code);
  VulkanShaderModule(VulkanShaderModule const &) = delete;
  VulkanShaderModule &operator=(VulkanShaderModule const &) = delete;
  VulkanShaderModule &operator=(VulkanShaderModule &&) noexcept;
  ~VulkanShaderModule();

  operator VkShaderModule() const { return m_ShaderModule; }

  VkPipelineShaderStageCreateInfo pipelineStageInfo(VkShaderStageFlagBits stage) const;
  void cleanup();

private:
  VkShaderModule m_ShaderModule = nullptr;
};

} // namespace Froth
