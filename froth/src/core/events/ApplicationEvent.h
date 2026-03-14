#pragma once
#include "Event.h"
#include "src/resources/Resource.h"
#include "src/resources/ResourceMetadata.h"
#include <cstddef>
#include <sstream>

namespace Froth {
class WindowCloseEvent : public Event {
public:
  WindowCloseEvent() {}
  static inline EventType staticEventType() { return EventType::WindowClose; }
  virtual inline EventType eventType() const override {
    return staticEventType();
  }
  virtual inline int categoryFlags() const override {
    return EventCategoryApplication;
  }
  virtual std::string ToString() const override {
    std::stringstream ss;
    ss << "WindowCloseEvent";
    return ss.str();
  }
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(size_t width, size_t height)
      : m_Width(width), m_Height(height) {}
  static inline EventType staticEventType() { return EventType::WindowResize; }
  virtual inline EventType eventType() const override {
    return staticEventType();
  }
  virtual inline int categoryFlags() const override {
    return EventCategoryApplication;
  }
  virtual std::string ToString() const override {
    std::stringstream ss;
    ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
    return ss.str();
  }

  size_t width() { return m_Width; }
  size_t height() { return m_Height; }

private:
  size_t m_Width, m_Height;
};

class FramebufferResizeEvent : public Event {
public:
  FramebufferResizeEvent(size_t width, size_t height)
      : m_Width(width), m_Height(height) {}
  static inline EventType staticEventType() { return EventType::FramebufferResize; }
  virtual inline EventType eventType() const override {
    return staticEventType();
  }
  virtual inline int categoryFlags() const override {
    return EventCategoryApplication;
  }
  virtual std::string ToString() const override {
    std::stringstream ss;
    ss << "FramebufferResizeEvent: " << m_Width << ", " << m_Height;
    return ss.str();
  }

  size_t width() { return m_Width; }
  size_t height() { return m_Height; }

private:
  size_t m_Width, m_Height;
};

class ResourceLoadedEvent : public Event {
public:
  ResourceLoadedEvent(ResourceHandle<Resource> handle, const ResourceMetadata &metadata) : m_Handle(handle), m_Metadata(metadata) {}
  static inline EventType staticEventType() { return EventType::ResourceLoaded; }
  virtual inline EventType eventType() const override {
    return staticEventType();
  }

  virtual inline int categoryFlags() const override {
    return EventCategoryApplication;
  }

  virtual std::string ToString() const override {
    std::stringstream ss;
    ss << "Resource Loaded Event: " << m_Handle;
    return ss.str();
  }

  ResourceHandle<Resource> handle() const { return m_Handle; }
  const ResourceMetadata &metadata() const { return m_Metadata; }

private:
  ResourceHandle<Resource> m_Handle;
  ResourceMetadata m_Metadata;
};

} // namespace Froth
