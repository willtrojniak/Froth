#include "Material.h"

namespace Froth {

Material::Material(ResourceHandle vertShaderModule, ResourceHandle fragShaderModule)
    : m_VertexShaderModule(vertShaderModule), m_FragmentShaderModule(fragShaderModule) {
}
} // namespace Froth
