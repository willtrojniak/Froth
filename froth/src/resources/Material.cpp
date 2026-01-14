#include "Material.h"

namespace Froth {

Material::Material(ResourceHandle vertShaderModule, ResourceHandle fragShaderModule)
    : m_VertexShaderModule(vertShaderModule), m_FragmentShaderModule(fragShaderModule) {

  // TODO: Customizable descriptor set layout - consider reading from shaders?
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding uboLayoutBindingLights{};
  uboLayoutBindingLights.binding = 1;
  uboLayoutBindingLights.descriptorCount = 1;
  uboLayoutBindingLights.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBindingLights.pImmutableSamplers = VK_NULL_HANDLE;
  uboLayoutBindingLights.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 2;
  samplerLayoutBinding.descriptorCount = 4; // Array size
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  m_DescriptorSetLayouts.emplace_back(std::vector{uboLayoutBinding, uboLayoutBindingLights, samplerLayoutBinding});
}

const std::vector<VulkanDescriptorSetLayout> &Material::descriptorSetLayouts() const {
  return m_DescriptorSetLayouts;
};
} // namespace Froth
