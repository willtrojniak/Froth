#include "InputController.h"
#include "glm/ext/vector_float3.hpp"
#include "src/core/Entrypoint.h"

#include "src/core/events/EventDispatcher.h"
#include "src/core/events/MouseEvent.h"
#include "src/modules/camera/Camera.h"
#include "src/platform/filesystem/Filesystem.h"
#include "src/platform/keys/Keycodes.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include "src/renderer/vulkan/VulkanIndexBuffer.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/ResourceManager.h"
#include "src/resources/Texture2D.h"
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <functional>
#include <string.h>

const std::string MODEL_PATH = "../playground/models/viking_room.obj";
const std::string CUBE_MODEL_PATH = "../playground/models/cube.obj";
const std::string TEXTURE_PATH = "../playground/textures/viking_room.png";
const std::string TEXTURE_JPG_PATH = "../playground/textures/texture.jpg";

class TestLayer : public Froth::Layer {
public:
  TestLayer(Froth::VulkanRenderer &renderer)
      : m_Renderer(renderer) {

    std::vector<Froth::Vertex> vertices;
    std::vector<uint32_t> indices;
    if (!Froth::Filesystem::loadObj(MODEL_PATH.c_str(), vertices, indices)) {
    }
    Froth::VulkanCommandPool &commandPool = m_Renderer.getCurrentCommandPool();
    Froth::VulkanCommandBuffer commandBuffer = commandPool.AllocateCommandBuffer();

    m_VertexBuffer = Froth::VulkanVertexBuffer(sizeof(Froth::Vertex) * vertices.size());
    m_VertexBuffer.write(commandBuffer, sizeof(Froth::Vertex) * vertices.size(), vertices.data());

    commandBuffer.reset();

    m_IndexBuffer = Froth::VulkanIndexBuffer(indices.size());
    m_IndexBuffer.write(commandBuffer, indices.size(), indices.data());
    commandBuffer.reset();

    const float groundSize = 4.0f;
    std::vector<Froth::Vertex> plane_vertices = {
        {{-groundSize, -groundSize, 0.f}, {.3f, .3f, .3f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
        {{groundSize, -groundSize, 0.f}, {.3f, .3f, .3f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
        {{groundSize, groundSize, 0.f}, {.3f, .3f, .3f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
        {{-groundSize, groundSize, 0.f}, {.3f, .3f, .3f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
    };
    std::vector<uint32_t> plane_indices = {0, 1, 2, 2, 3, 0};

    std::vector<Froth::Vertex> cube_vertices;
    std::vector<uint32_t> cube_indices;
    if (!Froth::Filesystem::loadObj(CUBE_MODEL_PATH.c_str(), cube_vertices, cube_indices)) {
    }

    m_VertexBuffer2 = Froth::VulkanVertexBuffer(sizeof(Froth::Vertex) * plane_vertices.size());
    m_VertexBuffer2.write(commandBuffer, sizeof(Froth::Vertex) * plane_vertices.size(), plane_vertices.data());
    commandBuffer.reset();
    m_IndexBuffer2 = Froth::VulkanIndexBuffer(plane_indices.size());
    m_IndexBuffer2.write(commandBuffer, plane_indices.size(), plane_indices.data());
    commandBuffer.reset();

    m_VertexBuffer3 = Froth::VulkanVertexBuffer(sizeof(Froth::Vertex) * cube_vertices.size());
    m_VertexBuffer3.write(commandBuffer, sizeof(Froth::Vertex) * cube_vertices.size(), cube_vertices.data());
    commandBuffer.reset();
    m_IndexBuffer3 = Froth::VulkanIndexBuffer(cube_indices.size());
    m_IndexBuffer3.write(commandBuffer, cube_indices.size(), cube_indices.data());
    commandBuffer.reset();

    std::vector<char> vertShaderCode = Froth::Filesystem::readFile("../playground/shaders/vert.spv");
    std::vector<char> fragShaderCode = Froth::Filesystem::readFile("../playground/shaders/frag.spv");

    std::shared_ptr<Froth::VulkanShaderModule> vertShaderModule = std::make_shared<Froth::VulkanShaderModule>(vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
    std::shared_ptr<Froth::VulkanShaderModule> fragShaderModule = std::make_shared<Froth::VulkanShaderModule>(fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);

    m_Material = Froth::Material(vertShaderModule, fragShaderModule);

    // Texture
    std::shared_ptr<Froth::Texture2D> texture = m_ResourceManager.importResource<Froth::Texture2D>(TEXTURE_PATH);

    m_Sampler = Froth::VulkanSampler::Builder().build();
    m_Renderer.setDescriptorTexture(m_Sampler, texture->view());

    commandBuffer.cleanup(commandPool);
    m_Camera = Froth::Camera(glm::vec3(0.0f, -5.0f, 1.0f), 90.f, 0.f);
  }

  void onUpdate(double ts) override {
    const float moveSpeed = 2.f;

    if (m_InputController.isPressed(Froth::Key::W)) {
      m_Camera.moveForward(ts * moveSpeed);
    }
    if (m_InputController.isPressed(Froth::Key::S)) {
      m_Camera.moveForward(ts * -moveSpeed);
    }
    if (m_InputController.isPressed(Froth::Key::A)) {
      m_Camera.strafe(ts * -moveSpeed);
    }
    if (m_InputController.isPressed(Froth::Key::D)) {
      m_Camera.strafe(ts * moveSpeed);
    }
    if (m_InputController.isPressed(Froth::Key::Q)) {
      m_Camera.moveUp(ts * -moveSpeed);
    }
    if (m_InputController.isPressed(Froth::Key::E)) {
      m_Camera.moveUp(ts * moveSpeed);
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, 0.107647f));
    glm::mat4 view = m_Camera.view();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), m_Width / (float)m_Height, 0.1f, 100.0f);
    proj[1][1] *= -1;

    glm::mat4 mvp = proj * view * model;
    uint32_t texIndex = 1;

    m_Renderer.bindMaterial(m_Material);

    m_Renderer.pushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp), &mvp);
    m_Renderer.pushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mvp), sizeof(texIndex), &texIndex);
    m_Renderer.bindVertexBuffer(m_VertexBuffer);
    m_Renderer.bindIndexBuffer(m_IndexBuffer);

    mvp = proj * view * glm::translate(glm::vec3(-5.0f, -5.0f, -0.1f)) * glm::scale(glm::vec3(10.f, 10.f, 0.1f));
    texIndex = 0;
    m_Renderer.pushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp), &mvp);
    m_Renderer.pushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mvp), sizeof(texIndex), &texIndex);
    m_Renderer.bindVertexBuffer(m_VertexBuffer3);
    m_Renderer.bindIndexBuffer(m_IndexBuffer3);

    mvp = proj * view * glm::translate(glm::vec3(2.f, 1.f, -0.01f));
    m_Renderer.pushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp), &mvp);
    m_Renderer.pushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mvp), sizeof(texIndex), &texIndex);
    m_Renderer.bindVertexBuffer(m_VertexBuffer3);
    m_Renderer.bindIndexBuffer(m_IndexBuffer3);

    mvp = proj * view * glm::translate(glm::vec3(-2.f, -2.f, -0.01f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
    m_Renderer.pushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp), &mvp);
    m_Renderer.pushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mvp), sizeof(texIndex), &texIndex);
    m_Renderer.bindVertexBuffer(m_VertexBuffer3);
    m_Renderer.bindIndexBuffer(m_IndexBuffer3);
  }

  bool onWindowResize(Froth::WindowResizeEvent &e) {
    m_Height = e.height();
    m_Width = e.width();
    return false;
  }

  bool onMouseMove(const Froth::MouseMoveEvent &e) {
    m_Camera.rotate((m_CursorX - e.x()) / 10, (m_CursorY - e.y()) / 10);
    m_CursorX = e.x();
    m_CursorY = e.y();

    return false;
  }

  virtual bool onEvent(const Froth::Event &e) override {
    Froth::EventDispatcher d(e);
    d.dispatch<Froth::WindowResizeEvent>(std::bind(&TestLayer::onWindowResize, this, std::placeholders::_1));
    d.dispatch<Froth::MouseMoveEvent>(std::bind(&TestLayer::onMouseMove, this, std::placeholders::_1));

    m_InputController.onEvent(e);

    return d.isHandled();
  }

  virtual void onDetatch() override final {
  };

private:
  Froth::ResourceManager m_ResourceManager;
  Froth::VulkanRenderer &m_Renderer;
  Froth::VulkanIndexBuffer m_IndexBuffer;
  Froth::VulkanVertexBuffer m_VertexBuffer;
  Froth::VulkanIndexBuffer m_IndexBuffer2;
  Froth::VulkanVertexBuffer m_VertexBuffer2;
  Froth::VulkanIndexBuffer m_IndexBuffer3;
  Froth::VulkanVertexBuffer m_VertexBuffer3;
  Froth::Material m_Material;
  Froth::VulkanSampler m_Sampler;
  Froth::Camera m_Camera;
  InputController m_InputController;
  uint32_t m_Width = 600;
  uint32_t m_Height = 400;
  double m_CursorX;
  double m_CursorY;
};

class Playground : public Froth::Application {
public:
  Playground() {
    pushLayer(std::make_unique<TestLayer>(renderer()));
  }
};

Froth::Application *Froth::CreateApplication() { return new Playground(); }
