#pragma once

#include "src/renderer/vulkan/VulkanShaderModule.h"
#include "src/resources/Resource.h"

namespace Froth {

class ShaderModule : public Resource {
public:
  enum class ShaderStage : uint16_t {
    FRAGMENT,
    VERTEX,
  };

  virtual ResourceType type() const noexcept { return ResourceType::ShaderModule; }
  ShaderModule() = default;
  ShaderModule(const std::vector<char> &code);
  const VulkanShaderModule &module() const noexcept { return m_Module; }

private:
  // TODO: Abstract implementation
  VulkanShaderModule m_Module;
};

} // namespace Froth
