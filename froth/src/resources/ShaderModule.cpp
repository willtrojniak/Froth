#include "ShaderModule.h"

namespace Froth {
ShaderModule::ShaderModule(const std::vector<char> &code)
    : m_Module(code) {
}

} // namespace Froth
