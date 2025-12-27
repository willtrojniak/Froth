#pragma once

#include "src/platform/window/Window.h"
#include "src/resources/Shader.h"
#include <memory>

namespace Froth {

class Renderer {
protected:
  Renderer() {};

public:
  virtual ~Renderer() = default;

  virtual bool onEvent(const Event &e) = 0;

  virtual Shader createShader(const std::vector<char> &vert, const std::vector<char> &frag) = 0;

  virtual bool beginFrame() = 0;
  virtual void beginRenderPass() = 0;
  virtual void endRenderPass() = 0;
  virtual void endFrame() = 0;

  /* Creates an abstract Renderer backend
   *
   * @returns  Renderer Backend
   * @throws std::runtime_error if Window Surface cannot be created
   */
  static std::unique_ptr<Renderer> create(const Window &window);
  static void init(const Window &window);
  static void shutdown();
};

} // namespace Froth
