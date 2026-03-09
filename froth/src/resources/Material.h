#pragma once

#include "src/resources/Resource.h"
#include "src/resources/ShaderSource.h"

namespace Froth {
class Material {
public:
  Material() = default;
  Material(ResourceHandle<ShaderSource> vertShaderModule, ResourceHandle<ShaderSource> fragShaderModule);

  ResourceHandle<ShaderSource> vertexShaderHandle() const noexcept { return m_VertexShaderModule; }
  ResourceHandle<ShaderSource> fragmentShaderHandle() const noexcept { return m_FragmentShaderModule; }

private:
  ResourceHandle<ShaderSource> m_VertexShaderModule;
  ResourceHandle<ShaderSource> m_FragmentShaderModule;
};

} // namespace Froth
