#include "Material.h"

namespace Froth {

Material::Material(ResourceHandle<ShaderSource> vertShaderModule, ResourceHandle<ShaderSource> fragShaderModule)
    : m_VertexShaderModule(vertShaderModule), m_FragmentShaderModule(fragShaderModule) {
}
} // namespace Froth
