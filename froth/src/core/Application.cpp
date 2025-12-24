#include "Application.h"
#include "Defines.h"
#include "src/core/events/ApplicationEvent.h"
#include "src/core/events/Event.h"
#include "src/core/events/EventDispatcher.h"
#include "src/core/logger/Logger.h"
#include "src/platform/window/Window.h"
#include "src/renderer/Renderer.h"
#include "src/renderer/vulkan/VulkanRenderer.h"
#include <chrono>
#include <functional>
#include <memory>

namespace Froth {

#define BIND_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

Application *Application::s_Application = nullptr;

Application::Application() {
  if (s_Application) {
    FROTH_ERROR("Attempted to create second application");
  }
  s_Application = this;

  FROTH_INFO("Creating Application with Froth v%d.%d.%d", FROTH_VERSION_MAJOR, FROTH_VERSION_MINOR, FROTH_VERSION_PATCH)
  m_Window = Window::createWindow(640, 480, "Hello World");
  if (!m_Window) {
    m_Running = false;
    return;
  }
  m_Window->setEventCallbackFunction(BIND_FUNC(onEvent));
  m_Window->setCursorMode(Window::CursorMode::DISABLED);

  Renderer::init(*m_Window);
  FROTH_INFO("RENDERER INITIALIZED")

  m_Renderer = VulkanRenderer(*m_Window);
  FROTH_INFO("RENDERER INSTANTIATED")
}
Application::~Application() {
  Renderer::shutdown();
}

void Application::Run() {
  auto timer = std::chrono::high_resolution_clock();
  auto tBegin = timer.now();
  float deltaT = 1.f / 30.f;

  while (m_Running) {
    Window::pollEvents();

    // Update loop
    for (std::shared_ptr<Layer> layer : m_LayerStack) {
      layer->onUpdate(deltaT);
    }

    // Draw loop
    m_Renderer.beginFrame();
    m_Renderer.beginRenderPass();
    for (std::shared_ptr<Layer> layer : m_LayerStack) {
      layer->onDraw(m_Renderer.currentFrame());
    }
    m_Renderer.endRenderPass();
    m_Renderer.endFrame();

    auto tEnd = timer.now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
    deltaT = elapsed.count() / 1000.f;
    tBegin = tEnd;
  }
}

void Application::onEvent(const Event &e) {
  for (std::shared_ptr<Layer> layer : m_LayerStack) {
    if (layer->onEvent(e))
      return;
  }

  EventDispatcher dispatcher = EventDispatcher(e);
  dispatcher.dispatch<WindowCloseEvent>(BIND_FUNC(onWindowClose));
  dispatcher.dispatch<WindowResizeEvent>(BIND_FUNC(onWindowResize));
}

void Application::pushLayer(std::shared_ptr<Layer> layer) {
  layer->onAttach();
  m_LayerStack.pushLayer(std::move(layer));
}

void Application::pushOverlay(std::shared_ptr<Layer> overlay) {
  overlay->onAttach();
  m_LayerStack.pushOverlay(std::move(overlay));
}

bool Application::onWindowClose(WindowCloseEvent &e) {
  m_Running = false;
  return true;
}

bool Application::onWindowResize(WindowResizeEvent &e) {
  return false;
}

} // namespace Froth
