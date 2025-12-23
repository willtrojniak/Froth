#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

namespace Froth {

class VulkanShaderModule {

public:
  VulkanShaderModule() = default;
  VulkanShaderModule(const std::vector<char> &code, VkShaderStageFlagBits stage);
  VulkanShaderModule(VulkanShaderModule const &) = delete;
  VulkanShaderModule &operator=(VulkanShaderModule const &) = delete;
  VulkanShaderModule &operator=(VulkanShaderModule &&) noexcept;
  ~VulkanShaderModule();

  operator VkShaderModule() const { return m_ShaderModule; }

  VkPipelineShaderStageCreateInfo pipelineStageInfo() const;
  void cleanup();

private:
  VkShaderModule m_ShaderModule = nullptr;
  VkShaderStageFlagBits m_Stage;
};

} // namespace Froth
