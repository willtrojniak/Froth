#pragma once

#include "glm/ext/vector_float4.hpp"

namespace Froth {
struct alignas(16) SpotLight {
  glm::vec4 pos;
  glm::vec4 color;
  glm::vec4 params;
};
} // namespace Froth
