#pragma once

#include "events/Event.h"
namespace Froth {
class Layer {
public:
  Layer() = default;
  virtual ~Layer() = default;

  virtual void onAttach() {}
  virtual void onDetatch() {}
  virtual void onUpdate(double ts) {}
  virtual void onDraw(double ts) {}
  virtual bool onEvent(const Event &e) { return false; }
};
} // namespace Froth
