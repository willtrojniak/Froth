#pragma once

#include "src/resources/Resource.h"
namespace Froth {
class Material {
public:
  Material() = default;
  Material(ResourceHandle vertShaderModule, ResourceHandle fragShaderModule);

  ResourceHandle vertexShaderHandle() const noexcept { return m_VertexShaderModule; }
  ResourceHandle fragmentShaderHandle() const noexcept { return m_FragmentShaderModule; }

private:
  ResourceHandle m_VertexShaderModule;
  ResourceHandle m_FragmentShaderModule;
};

} // namespace Froth
