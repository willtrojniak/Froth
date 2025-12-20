#pragma once

#include "src/renderer/vulkan/VulkanIndexBuffer.h"
#include "src/renderer/vulkan/VulkanVertex.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/Resource.h"

namespace Froth {
class Mesh : public Resource {
public:
  Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
  virtual ResourceType type() const noexcept override final { return ResourceType::Mesh; }
  const VulkanVertexBuffer &vertexBuffer() const noexcept { return m_VertexBuffer; }
  const VulkanIndexBuffer &indexBuffer() const noexcept { return m_IndexBuffer; }

private:
  VulkanVertexBuffer m_VertexBuffer;
  VulkanIndexBuffer m_IndexBuffer;
};

} // namespace Froth
