#include "VulkanShaderModuleManager.h"
#include "shaderc/shaderc.hpp"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/resources/ShaderSource.h"

namespace Froth {

const VulkanShaderModule &VulkanShaderModuleManager::getOrCreateShaderModule(const ResourceHandle &handle) {
  if (!m_ShaderModules.contains(handle)) {
    auto shaderSrc = Application::getInstance().resourceManager().getResource<ShaderSource>(handle);
    auto metaData = Application::getInstance().resourceManager().getMetadata(handle);

    // TODO: Move compilation logic to its own class
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    const std::string source = std::string(shaderSrc->data().begin(), shaderSrc->data().end());
    FROTH_DEBUG("Compiling:\n%s\n", source.c_str());

    shaderc_shader_kind kind;
    if (metaData.FilePath.extension() == ".vert")
      kind = shaderc_glsl_vertex_shader;
    else if (metaData.FilePath.extension() == ".frag")
      kind = shaderc_fragment_shader;

    auto result = compiler.CompileGlslToSpv(source, kind, metaData.FilePath.filename().c_str());
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
      throw std::runtime_error("Failed to compile shader");
    }

    const std::vector<uint32_t> data(result.cbegin(), result.cend());

    m_ShaderModules[handle] = VulkanShaderModule(data);
    FROTH_DEBUG("Vulkan Shader Module: Compiled Shader");
  }

  return m_ShaderModules[handle];
}

} // namespace Froth
