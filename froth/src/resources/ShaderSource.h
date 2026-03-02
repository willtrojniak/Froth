#pragma once

#include "Resource.h"

namespace Froth {
class ShaderSource : public Resource {
public:
  ShaderSource() = default;
  ShaderSource(const std::vector<char> code);

  virtual ResourceType type() const noexcept { return ResourceType::ShaderSource; }

  const std::vector<char> &data() const noexcept { return m_GlslCode; }

private:
  const std::vector<char> m_GlslCode;
};
} // namespace Froth
