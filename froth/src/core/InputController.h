#pragma once

#include "src/core/events/Event.h"
#include "src/core/events/KeyEvent.h"
#include "src/platform/keys/Keycodes.h"
#include <unordered_map>

class InputController {
public:
  bool onEvent(const Froth::Event &e);
  bool isPressed(Froth::KeyCode key) const;

private:
  std::unordered_map<Froth::KeyCode, bool> m_Keys;

  bool onKeyPress(Froth::KeyPressedEvent &e);
  bool onKeyRelease(Froth::KeyReleasedEvent &e);
};
