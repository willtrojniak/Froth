#pragma once

#include <string>
namespace Froth {
enum class EventType {
  None = 0,
  WindowClose,
  WindowResize,
  WindowMove,
  WindowFocus,
  WindowLoseFocus,
  FramebufferResize,
  ResourceLoaded,
  KeyPressed,
  KeyReleased,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMove,
  MouseScroll
};

enum EventCategory {
  None = 0,
  EventCategoryApplication = 1,
  EventCategoryInput = 1 << 1,
  EventCategoryKeyboard = 1 << 2,
  EventCategoryMouse = 1 << 3,
  EventCategoryMouseButton = 1 << 4
};

class Event {
public:
  bool isHandled = false;

  virtual ~Event() = default;
  virtual EventType eventType() const = 0;
  virtual int categoryFlags() const = 0;
  virtual std::string ToString() const = 0;

  inline bool isOfCategory(EventCategory category) const {
    return categoryFlags() & category;
  }
};
} // namespace Froth
