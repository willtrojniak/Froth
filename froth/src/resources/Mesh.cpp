#include "Mesh.h"
#include "src/core/Application.h"

namespace Froth {

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    : m_VertexBuffer(sizeof(Vertex) * vertices.size()),
      m_IndexBuffer(indices.size()) {

  VulkanRenderer &renderer = Application::getInstance().renderer();
  Froth::VulkanCommandPool &commandPool = renderer.getGraphicsCommandPool();
  Froth::VulkanCommandBuffer commandBuffer = commandPool.AllocateCommandBuffer();

  // FIXME: Error handling
  m_VertexBuffer.write(commandBuffer, sizeof(Vertex) * vertices.size(), vertices.data());
  commandBuffer.reset();

  // FIXME: Error handling
  m_IndexBuffer.write(commandBuffer, indices.size(), indices.data());
  commandBuffer.reset();

  commandBuffer.cleanup(commandPool);
}

} // namespace Froth
