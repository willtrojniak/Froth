#include "InputController.h"
#include "src/core/events/EventDispatcher.h"
#include "src/core/events/KeyEvent.h"
#include <functional>

bool InputController::onEvent(const Froth::Event &e) {
  Froth::EventDispatcher d(e);
  d.dispatch<Froth::KeyPressedEvent>(std::bind(&InputController::onKeyPress, this, std::placeholders::_1));
  d.dispatch<Froth::KeyReleasedEvent>(std::bind(&InputController::onKeyRelease, this, std::placeholders::_1));

  return d.isHandled();
}

bool InputController::isPressed(Froth::KeyCode key) const {
  if (!m_Keys.contains(key))
    return false;

  return m_Keys.at(key);
}

bool InputController::onKeyPress(Froth::KeyPressedEvent &e) {
  m_Keys[e.keyCode()] = true;
  return false;
}

bool InputController::onKeyRelease(Froth::KeyReleasedEvent &e) {
  m_Keys[e.keyCode()] = false;
  return false;
}
