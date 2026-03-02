#pragma once

#include "ShaderCompiler.h"

namespace Froth {
class ShadercShaderCompiler : public ShaderCompiler {

public:
  virtual std::expected<std::vector<uint32_t>, bool> compileGLSLToSpirV(const std::string &source, VulkanShaderModule::ShaderStage stage) override;
};

} // namespace Froth
