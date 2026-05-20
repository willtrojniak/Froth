#pragma once

#include "events/Event.h"
#include <cstdint>

namespace Froth {
class Layer {
public:
  Layer() = default;
  virtual ~Layer() = default;

  virtual void onAttach() {}
  virtual void onDetatch() {}
  virtual void onUpdate(double ts) {}
  virtual void onDraw(uint32_t frame) {}
  virtual bool onEvent(const Event &e) { return false; }
};
} // namespace Froth
