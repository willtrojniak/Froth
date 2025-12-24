#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
namespace Froth {

class Camera {
public:
  Camera() = default;
  Camera(const glm::vec3 &pos, float yaw, float pitch);

  glm::vec3 direction() const;

  // TODO: Move controls to controller class
  void rotate(float yaw, float pitch);
  void strafe(float speed);
  void moveForward(float speed);
  void moveUp(float speed);

  glm::mat4 view() const;
  glm::vec3 pos() const { return m_Pos; }

private:
  glm::vec3 m_Pos;
  glm::vec3 m_Up = glm::vec3(0.0f, 0.0f, 1.0f);
  float m_Yaw = 0.0f;
  float m_Pitch = 0.0f;
};

} // namespace Froth
