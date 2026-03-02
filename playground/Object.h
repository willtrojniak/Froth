#pragma once

#include "src/resources/Mesh.h"
#include "src/resources/Resource.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Object {
public:
  Object() = default;
  Object(const glm::mat4 &transform, const Froth::ResourceHandle<Froth::Mesh> &meshHandle)
      : m_Transform(transform), m_MeshHandle(meshHandle) {}

  const glm::mat4 &transform() const { return m_Transform; }
  const Froth::ResourceHandle<Froth::Mesh> &meshHandle() const { return m_MeshHandle; }

private:
  glm::mat4 m_Transform;
  Froth::ResourceHandle<Froth::Mesh> m_MeshHandle;
};
