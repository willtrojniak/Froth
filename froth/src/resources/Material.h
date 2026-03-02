#pragma once

#include "src/renderer/vulkan/VulkanDescriptorSetLayout.h"
#include "src/resources/Resource.h"
#include "src/resources/ShaderSource.h"

namespace Froth {
class Material {
public:
  Material() = default;
  Material(ResourceHandle<ShaderSource> vertShaderModule, ResourceHandle<ShaderSource> fragShaderModule);

  ResourceHandle<ShaderSource> vertexShaderHandle() const noexcept { return m_VertexShaderModule; }
  ResourceHandle<ShaderSource> fragmentShaderHandle() const noexcept { return m_FragmentShaderModule; }

  const std::vector<VulkanDescriptorSetLayout> &descriptorSetLayouts() const;

private:
  ResourceHandle<ShaderSource> m_VertexShaderModule;
  ResourceHandle<ShaderSource> m_FragmentShaderModule;

  // TODO: Move out to global cache for reuse across shaders
  std::vector<VulkanDescriptorSetLayout> m_DescriptorSetLayouts;
};

} // namespace Froth
