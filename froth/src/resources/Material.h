#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/resources/Resource.h"

namespace Froth {
class Material {
public:
  Material() = default;
  Material(ResourceHandle vertShaderModule, ResourceHandle fragShaderModule);

  ResourceHandle vertexShaderHandle() const noexcept { return m_VertexShaderModule; }
  ResourceHandle fragmentShaderHandle() const noexcept { return m_FragmentShaderModule; }

  const std::vector<VulkanDescriptorSetLayout> &descriptorSetLayouts() const;

private:
  ResourceHandle m_VertexShaderModule;
  ResourceHandle m_FragmentShaderModule;

  // TODO: Move out to global cache for reuse across shaders
  std::vector<VulkanDescriptorSetLayout> m_DescriptorSetLayouts;
};

} // namespace Froth
