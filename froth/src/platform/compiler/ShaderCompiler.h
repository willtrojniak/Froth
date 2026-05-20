#pragma once

#include "src/renderer/vulkan/VulkanShaderModule.h"

#include <expected>
#include <string>
#include <vector>

namespace Froth {
class ShaderCompiler {
public:
  virtual ~ShaderCompiler() = default;
  virtual std::expected<std::vector<uint32_t>, bool> compileGLSLToSpirV(const std::string &source, VulkanShaderModule::ShaderStage stage) = 0;
};
} // namespace Froth
