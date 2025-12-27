#include "InputController.h"
#include "glm/ext/vector_float3.hpp"
#include "src/core/Entrypoint.h"

#include "src/core/events/EventDispatcher.h"
#include "src/core/events/MouseEvent.h"
#include "src/modules/camera/Camera.h"
#include "src/modules/light/Light.h"
#include "src/platform/filesystem/Filesystem.h"
#include "src/platform/keys/Keycodes.h"
#include "src/renderer/vulkan/VulkanDescriptorPool.h"
#include "src/renderer/vulkan/VulkanDescriptorSet.h"
#include "src/renderer/vulkan/VulkanImage.h"
#include "src/renderer/vulkan/VulkanIndexBuffer.h"
#include "src/renderer/vulkan/VulkanUniformBuffer.h"
#include "src/renderer/vulkan/VulkanVertexBuffer.h"
#include "src/resources/Mesh.h"
#include "src/resources/ResourceManager.h"
#include "src/resources/Texture2D.h"
#include <vector>

#include "Object.h"

#include <functional>
#include <string.h>

const std::string MODEL_PATH = "../playground/models/viking_room.obj";
const std::string CUBE_MODEL_PATH = "../playground/models/cube.obj";
const std::string TEXTURE_PATH = "../playground/textures/viking_room.png";
const std::string TEXTURE_JPG_PATH = "../playground/textures/texture.jpg";

class TestLayer : public Froth::Layer {
public:
  struct ViewProjUbo {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 cameraPos;
  };

  static const uint32_t MAX_LIGHTS = 3;
  struct LightUBO {
    Froth::SpotLight lights[MAX_LIGHTS];
  };

  TestLayer(Froth::VulkanRenderer &renderer)
      : m_Renderer(renderer),
        m_DescriptorPool(2, 4, 8),
        m_Camera(glm::vec3(0.0f, -5.0f, 1.0f), 90.f, 0.f) {

    Froth::ResourceHandle vikingMeshHandle = m_ResourceManager.importResource<Froth::Mesh>(MODEL_PATH.c_str())->handle();
    Froth::ResourceHandle cubeMeshHandle = m_ResourceManager.importResource<Froth::Mesh>(CUBE_MODEL_PATH.c_str())->handle();

    m_VikingObject = Object(glm::translate(glm::vec3(0.f, 0.f, 0.107647f)), vikingMeshHandle);
    m_Cubes.emplace_back(glm::translate(glm::vec3(-5.0f, -5.0f, -0.1f)) * glm::scale(glm::vec3(10.f, 10.f, 0.1f)), cubeMeshHandle);
    m_Cubes.emplace_back(glm::translate(glm::vec3(2.f, 1.f, -0.01f)), cubeMeshHandle);
    m_Cubes.emplace_back(glm::translate(glm::vec3(-2.f, -2.f, -0.01f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)), cubeMeshHandle);

    std::vector<char> vertShaderCode = Froth::Filesystem::readFile("../playground/shaders/vert.spv");
    std::vector<char> fragShaderCode = Froth::Filesystem::readFile("../playground/shaders/frag.spv");

    m_Vert = Froth::VulkanShaderModule(vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
    m_Frag = Froth::VulkanShaderModule(fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);
    m_Shader = m_Renderer.createShader(m_Vert, m_Frag);

    LightUBO lightUBO{};
    lightUBO.lights[0].pos = glm::vec4(-1.75f, -1.75f, 0.5f, 1.0);
    lightUBO.lights[0].color = glm::vec4(0.6f, 0.1f, 0.9f, 1.0);
    lightUBO.lights[0].params = glm::vec4(0.0f, 0.0f, 3.0f, 1.0);

    lightUBO.lights[1].pos = glm::vec4(-1.25f, -1.25f, 0.5f, 1.0);
    lightUBO.lights[1].color = glm::vec4(0.0f, 0.7f, 0.2f, 1.0);
    lightUBO.lights[1].params = glm::vec4(0.0f, 0.0f, 1.0f, 1.0);

    lightUBO.lights[2].pos = glm::vec4(0.1f, 0.1f, 0.5f, 1.0);
    lightUBO.lights[2].color = glm::vec4(0.8f, 0.3f, 0.1f, 1.0);
    lightUBO.lights[2].params = glm::vec4(0.0f, 0.0f, 0.3f, 1.0);

    m_DescriptorSets = m_DescriptorPool.allocateDescriptorSets(std::vector<VkDescriptorSetLayout>(2, m_Shader.descriptorSets()[0]));
    m_ViewProjUbos.reserve(2);
    m_ViewProjUbos.emplace_back(sizeof(ViewProjUbo));
    m_ViewProjUbos.emplace_back(sizeof(ViewProjUbo));

    m_LightUbos.reserve(2);
    m_LightUbos.emplace_back(sizeof(LightUBO));
    m_LightUbos.emplace_back(sizeof(LightUBO));

    m_LightUbos[0].write(sizeof(lightUBO), &lightUBO);
    m_LightUbos[1].write(sizeof(lightUBO), &lightUBO);

    // Blank Texture
    uint32_t blankImageData = 0xFFFFFFFF;
    Froth::Texture2D m_BlankTexture2D(Froth::Extent2D{.width = 1, .height = 1}, &blankImageData);

    // Viking Texture
    std::shared_ptr<Froth::Texture2D> texture = m_ResourceManager.importResource<Froth::Texture2D>(TEXTURE_PATH);
    m_Sampler = Froth::VulkanSampler::Builder().build();

    auto writer = Froth::VulkanDescriptorSet::Writer();
    for (size_t i = 0; i < m_DescriptorSets.size(); i++) {
      auto descriptorSet = m_DescriptorSets[i];
      writer.addUniform(descriptorSet, 0, m_ViewProjUbos[i], 0);
      writer.addUniform(descriptorSet, 1, m_LightUbos[i], 0);
      writer.addImageSampler(descriptorSet, 2, m_BlankTexture2D.view(), m_Sampler, 0);
      writer.addImageSampler(descriptorSet, 2, texture->view(), m_Sampler, 1);
    }
    writer.Write();
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
  }

  void onDraw(uint32_t frame) override {
    glm::mat4 view = m_Camera.view();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), m_Width / (float)m_Height, 0.1f, 100.0f);
    proj[1][1] *= -1;

    ViewProjUbo vp{
        .view = view,
        .proj = proj,
        .cameraPos = glm::vec4(m_Camera.pos(), 1.0f),
    };

    m_ViewProjUbos[frame].write(sizeof(ViewProjUbo), &vp);

    uint32_t texIndex = 1;
    m_Renderer.bindShader(m_Shader);
    m_Renderer.bindDescriptorSets(m_Shader, 0, std::vector{m_DescriptorSets[frame]});

    m_Renderer.pushConstants(m_Shader, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::mat4), sizeof(texIndex), &texIndex);
    m_Renderer.pushConstants(m_Shader, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(m_VikingObject.transform()), &m_VikingObject.transform());
    m_Renderer.bindMesh(*m_ResourceManager.getResource<Froth::Mesh>(m_VikingObject.meshHandle()));

    texIndex = 0;
    m_Renderer.pushConstants(m_Shader, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::mat4), sizeof(texIndex), &texIndex);
    for (Object &cube : m_Cubes) {
      m_Renderer.pushConstants(m_Shader, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(cube.transform()), &cube.transform());
      m_Renderer.bindMesh(*m_ResourceManager.getResource<Froth::Mesh>(cube.meshHandle()));
    }
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

  ~TestLayer() {
    m_DescriptorPool.freeDescriptorSets(m_DescriptorSets);
  }

private:
  Froth::VulkanRenderer &m_Renderer;
  Object m_VikingObject;
  std::vector<Object> m_Cubes;
  Froth::ResourceManager m_ResourceManager;
  Froth::Shader m_Shader;
  Froth::Texture2D m_BlankTexture2D;
  Froth::VulkanDescriptorPool m_DescriptorPool;
  std::vector<VkDescriptorSet> m_DescriptorSets;
  std::vector<Froth::VulkanUniformBuffer> m_ViewProjUbos;
  std::vector<Froth::VulkanUniformBuffer> m_LightUbos;
  Froth::VulkanSampler m_Sampler;
  Froth::VulkanShaderModule m_Vert;
  Froth::VulkanShaderModule m_Frag;
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
