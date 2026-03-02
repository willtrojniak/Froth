#include "ShadercShaderCompiler.h"
#include "shaderc/shaderc.hpp"
#include "src/core/logger/Logger.h"

namespace Froth {

static shaderc_shader_kind getShadeKindFromStage(VulkanShaderModule::ShaderStage stage) {
  switch (stage) {
  case VulkanShaderModule::ShaderStage::VERTEX:
    return shaderc_vertex_shader;
  case VulkanShaderModule::ShaderStage::FRAGMENT:
    return shaderc_fragment_shader;
  }
}

std::expected<std::vector<uint32_t>, bool> ShadercShaderCompiler::compileGLSLToSpirV(const std::string &source, VulkanShaderModule::ShaderStage stage) {
  shaderc::Compiler compiler;
  shaderc_shader_kind shaderKind = getShadeKindFromStage(stage);
  const char *shaderName = "Shader";

  // TODO: Support compilation options
  shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, shaderKind, shaderName);
  if (result.GetCompilationStatus() == shaderc_compilation_status_success) {
    FROTH_DEBUG("ShadercShaderCompiler: Compiled Shader");
    return std::vector<uint32_t>{result.cbegin(), result.cend()};
  }

  return std::unexpected(false);
}

} // namespace Froth
