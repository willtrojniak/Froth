#include "VulkanShaderModuleManager.h"
#include "src/core/Application.h"
#include "src/core/logger/Logger.h"
#include "src/resources/ShaderSource.h"

namespace Froth {

VulkanShaderModuleManager::VulkanShaderModuleManager(std::unique_ptr<ShaderCompiler> compiler)
    : m_Compiler(std::move(compiler)) {
}

const VulkanShaderModule &VulkanShaderModuleManager::getOrCreateShaderModule(const ResourceHandle<ShaderSource> &handle) {
  if (!m_ShaderModules.contains(handle)) {
    m_ShaderModules[handle] = createShaderModule(handle);
  }

  return m_ShaderModules[handle];
}

std::optional<VulkanShaderModule> VulkanShaderModuleManager::recreateShaderModule(const ResourceHandle<ShaderSource> &handle) {
  auto it = m_ShaderModules.find(handle);
  std::optional<VulkanShaderModule> oldModule;
  if (it != m_ShaderModules.end()) {
    oldModule = std::move(it->second);
  } else {
    oldModule = std::nullopt;
  }

  m_ShaderModules[handle] = createShaderModule(handle);
  return oldModule;
}

VulkanShaderModule VulkanShaderModuleManager::createShaderModule(ResourceHandle<ShaderSource> handle) const {
  auto shaderSrc = Application::getInstance().resourceManager().getResource<ShaderSource>(handle);
  auto metaData = Application::getInstance().resourceManager().getMetadata(handle);

  const std::string source = std::string(shaderSrc->data().begin(), shaderSrc->data().end());

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

  FROTH_DEBUG("Vulkan Shader Module: Compiled Shader");
  return VulkanShaderModule(result.value());
}

} // namespace Froth
