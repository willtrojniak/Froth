#include "VulkanShaderModuleManager.h"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/resources/ShaderSource.h"

namespace Froth {

VulkanShaderModuleManager::VulkanShaderModuleManager(std::unique_ptr<ShaderCompiler> compiler)
    : m_Compiler(std::move(compiler)) {
}

const VulkanShaderModule &VulkanShaderModuleManager::getOrCreateShaderModule(const ResourceHandle &handle) {
  if (!m_ShaderModules.contains(handle)) {
    auto shaderSrc = Application::getInstance().resourceManager().getResource<ShaderSource>(handle);
    auto metaData = Application::getInstance().resourceManager().getMetadata(handle);

    // TODO: Move compilation logic to its own class
    const std::string source = std::string(shaderSrc->data().begin(), shaderSrc->data().end());
    FROTH_DEBUG("Compiling:\n%s\n", source.c_str());

    VulkanShaderModule::ShaderStage stage;
    if (metaData.FilePath.extension() == ".vert")
      stage = VulkanShaderModule::ShaderStage::VERTEX;
    else if (metaData.FilePath.extension() == ".frag")
      stage = VulkanShaderModule::ShaderStage::FRAGMENT;

    auto result = m_Compiler->compileGLSLToSpirV(source, stage);
    if (!result) {
      // TODO: Propogate expected return type
      throw std::runtime_error("Failed to compile shader");
    }

    m_ShaderModules[handle] = VulkanShaderModule(result.value());
    FROTH_DEBUG("Vulkan Shader Module: Compiled Shader");
  }

  return m_ShaderModules[handle];
}

} // namespace Froth
