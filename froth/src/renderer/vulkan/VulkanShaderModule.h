#pragma once

#include "src/core/Hash.h"

#include <expected>
#include <vector>
#include <vulkan/vulkan.h>

namespace Froth {

class VulkanShaderModule {
public:
  enum class ShaderStage {
    VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
    FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT
  };

  struct DescriptorSetLayoutData {
    uint32_t setNumber;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bool operator==(const DescriptorSetLayoutData &o) const;
  };

  VulkanShaderModule() = default;
  VulkanShaderModule(const std::vector<uint32_t> &code);
  VulkanShaderModule(VulkanShaderModule const &) = delete;
  VulkanShaderModule &operator=(VulkanShaderModule const &) = delete;
  VulkanShaderModule(VulkanShaderModule &&) noexcept;
  VulkanShaderModule &operator=(VulkanShaderModule &&) noexcept;
  ~VulkanShaderModule();

  operator VkShaderModule() const { return m_ShaderModule; }

  VkPipelineShaderStageCreateInfo pipelineStageInfo(VkShaderStageFlagBits stage) const;
  const std::vector<DescriptorSetLayoutData> &descriptorSetLayouts() const { return m_DescriptorSetLayoutData; }

  void cleanup();
  static std::expected<std::vector<DescriptorSetLayoutData>, bool> mergeDescriptorSetLayouts(const std::vector<DescriptorSetLayoutData> &setA, const std::vector<DescriptorSetLayoutData> &setB);

private:
  VkShaderModule m_ShaderModule = nullptr;
  std::vector<DescriptorSetLayoutData> m_DescriptorSetLayoutData;

  static std::expected<std::vector<DescriptorSetLayoutData>, bool> getShaderDescriptorSetLayouts(const std::vector<uint32_t> &code);
  static std::expected<std::vector<VkDescriptorSetLayoutBinding>, bool> mergeDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindingsA, const std::vector<VkDescriptorSetLayoutBinding> &bindingsB);
  static std::expected<VkDescriptorSetLayoutBinding, bool> mergeDescriptorSetBinding(const VkDescriptorSetLayoutBinding &b0, const VkDescriptorSetLayoutBinding &b1);
};

} // namespace Froth

namespace std {
template <>
struct hash<VkDescriptorSetLayoutBinding> {
  size_t operator()(const VkDescriptorSetLayoutBinding &b) const noexcept {
    size_t h = 0;

    Froth::Hash::HashCombine(h, b.binding);
    Froth::Hash::HashCombine(h, b.descriptorCount);
    Froth::Hash::HashCombine(h, b.descriptorType);
    Froth::Hash::HashCombine(h, b.stageFlags);

    return h;
  }
};

template <>
struct hash<Froth::VulkanShaderModule::DescriptorSetLayoutData> {
  size_t operator()(const Froth::VulkanShaderModule::DescriptorSetLayoutData &b) const noexcept {
    size_t h = 0;

    Froth::Hash::HashCombine(h, b.setNumber);
    Froth::Hash::HashCombine(h, b.bindings.size());
    for (const auto &binding : b.bindings) {
      Froth::Hash::HashCombine(h, std::hash<VkDescriptorSetLayoutBinding>{}(binding));
    }

    return h;
  }
};

} // namespace std
