#pragma once

#include "Defines.h"
#include "Layer.h"
#include "LayerStack.h"
#include "events/ApplicationEvent.h"
#include "events/Event.h"
#include "src/platform/window/Window.h"
#include "src/renderer/vulkan/VulkanRenderer.h"
#include <memory>

namespace Froth {

class Application {

public:
  Application();
  ~Application();
  void Run();
  static Application &getInstance() { return *s_Application; }

  void pushLayer(std::shared_ptr<Layer> layer);
  void pushOverlay(std::shared_ptr<Layer> overlay);

  void onEvent(const Event &e);
  bool onWindowClose(WindowCloseEvent &e);
  bool onWindowResize(WindowResizeEvent &e);

  // TODO: Remove
  const Window &window() { return *m_Window; }
  VulkanRenderer &renderer() { return m_Renderer; }

private:
  bool m_Running = true;
  std::unique_ptr<Window> m_Window;
  LayerStack m_LayerStack;
  VulkanRenderer m_Renderer;
  static Application *s_Application;
};
Application *CreateApplication();
} // namespace Froth
