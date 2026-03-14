#include "VulkanShaderModule.h"
#include "VulkanDevice.h"
#include "spirv_reflect.h"
#include "src/core/logger/Logger.h"
#include "src/renderer/vulkan/VulkanContext.h"
#include <algorithm>

namespace Froth {

static bool bindingIsEqual(const VkDescriptorSetLayoutBinding &a, const VkDescriptorSetLayoutBinding &b) {
  return a.binding == b.binding && a.descriptorCount == b.descriptorCount && a.descriptorType == b.descriptorType &&
         a.pImmutableSamplers == b.pImmutableSamplers && a.stageFlags == b.stageFlags;
}

bool VulkanShaderModule::DescriptorSetLayoutData::operator==(const DescriptorSetLayoutData &o) const {
  if (bindings.size() != o.bindings.size() || setNumber != o.setNumber) {
    return false;
  }

  for (size_t i = 0; i < bindings.size(); i++) {
    if (!bindingIsEqual(bindings[i], o.bindings[i]))
      return false;
  }
  return true;
}

VulkanShaderModule::VulkanShaderModule(const std::vector<uint32_t> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  VulkanContext &vctx = VulkanContext::get();
  if (vkCreateShaderModule(vctx.device(), &createInfo, vctx.allocator(), &m_ShaderModule) != VK_SUCCESS) {
    FROTH_ERROR("Failed to create shader module");
  }
  auto reflectBindingsResult = getShaderDescriptorSetLayouts(code);
  if (!reflectBindingsResult) {
    cleanup();
    FROTH_WARN("Failed to reflect binding descriptor sets")
  }
  m_DescriptorSetLayoutData = reflectBindingsResult.value();
  FROTH_DEBUG("Created Vulkan Shader Module, Set Count: %u", m_DescriptorSetLayoutData.size())
}

VulkanShaderModule::VulkanShaderModule(VulkanShaderModule &&o) noexcept
    : m_ShaderModule(o.m_ShaderModule), m_DescriptorSetLayoutData(std::move(o.m_DescriptorSetLayoutData)) {
  o.m_ShaderModule = nullptr;
}

VulkanShaderModule &VulkanShaderModule::operator=(VulkanShaderModule &&o) noexcept {
  m_ShaderModule = o.m_ShaderModule;
  m_DescriptorSetLayoutData = std::move(o.m_DescriptorSetLayoutData);

  o.m_ShaderModule = nullptr;
  return *this;
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::pipelineStageInfo(VkShaderStageFlagBits stage) const {
  VkPipelineShaderStageCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  createInfo.stage = stage;
  createInfo.module = m_ShaderModule;
  createInfo.pName = "main"; // TODO: Customizable?

  return createInfo;
}

VulkanShaderModule::~VulkanShaderModule() {
  cleanup();
}

void VulkanShaderModule::cleanup() {
  if (m_ShaderModule) {
    VulkanContext &vctx = VulkanContext::get();
    vkDestroyShaderModule(vctx.device(), m_ShaderModule, vctx.allocator());
    m_ShaderModule = nullptr;
    FROTH_DEBUG("Destroyed Vulkan Shader Module")
  }
}

std::expected<std::vector<VulkanShaderModule::DescriptorSetLayoutData>, bool> VulkanShaderModule::getShaderDescriptorSetLayouts(const std::vector<uint32_t> &code) {
  SpvReflectShaderModule module{};
  SpvReflectResult result = spvReflectCreateShaderModule(sizeof(uint32_t) * code.size(), code.data(), &module);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    FROTH_WARN("Vulkan Shader Module: Failed to create reflect module");
    return std::unexpected(false);
  }

  uint32_t descSetCount = 0;
  result = spvReflectEnumerateDescriptorSets(&module, &descSetCount, NULL);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    FROTH_WARN("Vulkan Shader Module: Failed to enumerate descriptor sets");
    return std::unexpected(false);
  }

  std::vector<SpvReflectDescriptorSet *> sets(descSetCount);
  result = spvReflectEnumerateDescriptorSets(&module, &descSetCount, sets.data());
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    FROTH_WARN("Vulkan Shader Module: Failed to enumerate descriptor sets");
    return std::unexpected(false);
  }

  std::vector<DescriptorSetLayoutData> setLayoutData(descSetCount);
  for (size_t i_set = 0; i_set < sets.size(); ++i_set) {
    const SpvReflectDescriptorSet &refl_set = *(sets[i_set]);
    setLayoutData[i_set].setNumber = refl_set.set;
    setLayoutData[i_set].bindings.resize(refl_set.binding_count);
    for (size_t i_binding = 0; i_binding < refl_set.binding_count; ++i_binding) {
      const SpvReflectDescriptorBinding &refl_binding = *(refl_set.bindings[i_binding]);
      setLayoutData[i_set].bindings[i_binding].binding = refl_binding.binding;
      setLayoutData[i_set].bindings[i_binding].descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
      setLayoutData[i_set].bindings[i_binding].descriptorCount = 1;
      for (size_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
        setLayoutData[i_set].bindings[i_binding].descriptorCount *= refl_binding.array.dims[i_dim];
      }
      setLayoutData[i_set].bindings[i_binding].stageFlags = static_cast<VkShaderStageFlagBits>(module.shader_stage);
    }
    std::sort(setLayoutData[i_set].bindings.begin(), setLayoutData[i_set].bindings.end(), [](VkDescriptorSetLayoutBinding &a, VkDescriptorSetLayoutBinding &b) {
      return a.binding < b.binding;
    });
  }

  std::sort(setLayoutData.begin(), setLayoutData.end(), [](DescriptorSetLayoutData &a, DescriptorSetLayoutData &b) {
    return a.setNumber < b.setNumber;
  });

  spvReflectDestroyShaderModule(&module);
  return setLayoutData;
}

std::expected<std::vector<VulkanShaderModule::DescriptorSetLayoutData>, bool> VulkanShaderModule::mergeDescriptorSetLayouts(const std::vector<DescriptorSetLayoutData> &setA, const std::vector<DescriptorSetLayoutData> &setB) {
  uint32_t i_setA = 0;
  uint32_t i_setB = 0;
  FROTH_DEBUG("Merging layouts: %u, %u", setA.size(), setB.size());

  std::vector<DescriptorSetLayoutData> mergedSets;
  while (i_setA < setA.size() && i_setB < setB.size()) {
    FROTH_DEBUG("Merging layouts %u, %u ", i_setA, i_setB);
    const auto &layoutA = setA[i_setA];
    const auto &layoutB = setB[i_setB];

    if (layoutA.setNumber < layoutB.setNumber) {
      mergedSets.push_back(layoutA);
      ++i_setA;
    } else if (layoutB.setNumber < layoutA.setNumber) {
      mergedSets.push_back(layoutB);
      ++i_setB;
    } else {
      FROTH_DEBUG("Merging layout conflict on set %u ", layoutA.setNumber);
      auto mergedBindingsResult = mergeDescriptorSetLayout(layoutA.bindings, layoutB.bindings);
      if (!mergedBindingsResult) {
        FROTH_WARN("Failed to merge descriptor set %u", layoutA.setNumber);
        return std::unexpected(false);
      }
      mergedSets.push_back(DescriptorSetLayoutData{
          .setNumber = layoutA.setNumber,
          .bindings = mergedBindingsResult.value(),
      });

      ++i_setA;
      ++i_setB;
    }
  }

  for (; i_setA < setA.size(); ++i_setA) {
    mergedSets.push_back(setA[i_setA]);
  }

  for (; i_setB < setB.size(); ++i_setB) {
    mergedSets.push_back(setB[i_setB]);
  }

  return mergedSets;
}

std::expected<std::vector<VkDescriptorSetLayoutBinding>, bool> VulkanShaderModule::mergeDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindingsA, const std::vector<VkDescriptorSetLayoutBinding> &bindingsB) {
  uint32_t i_bindingA = 0;
  uint32_t i_bindingB = 0;
  std::vector<VkDescriptorSetLayoutBinding> mergedBindings;

  while (i_bindingA < bindingsA.size() && i_bindingB < bindingsB.size()) {
    const auto &bindingA = bindingsA[i_bindingA];
    const auto &bindingB = bindingsB[i_bindingB];

    if (bindingA.binding < bindingB.binding) {
      mergedBindings.push_back(bindingA);
      ++i_bindingA;
    } else if (bindingB.binding < bindingA.binding) {
      mergedBindings.push_back(bindingB);
      ++i_bindingB;
    } else {
      const auto mergedBindgingResult = mergeDescriptorSetBinding(bindingA, bindingB);

      if (!mergedBindgingResult) {
        FROTH_WARN("Failed to merge binding %u", bindingA.binding);
        return std::unexpected(false);
      }

      mergedBindings.push_back(mergedBindgingResult.value());
      ++i_bindingA;
      ++i_bindingB;
    }
  }

  for (; i_bindingA < bindingsA.size(); ++i_bindingA) {
    mergedBindings.push_back(bindingsA[i_bindingA]);
  }

  for (; i_bindingB < bindingsB.size(); ++i_bindingB) {
    mergedBindings.push_back(bindingsB[i_bindingB]);
  }

  return mergedBindings;
}

std::expected<VkDescriptorSetLayoutBinding, bool> VulkanShaderModule::mergeDescriptorSetBinding(const VkDescriptorSetLayoutBinding &b0, const VkDescriptorSetLayoutBinding &b1) {
  VkDescriptorSetLayoutBinding mergedBinding{};

  if (b0.binding != b1.binding || b0.descriptorCount != b1.descriptorCount || b0.descriptorType != b1.descriptorType || b0.pImmutableSamplers != b1.pImmutableSamplers)
    return std::unexpected(false);

  mergedBinding.binding = b0.binding;
  mergedBinding.descriptorType = b0.descriptorType;
  mergedBinding.descriptorCount = b0.descriptorCount;
  mergedBinding.pImmutableSamplers = b0.pImmutableSamplers;
  mergedBinding.stageFlags = (b0.stageFlags | b1.stageFlags);
  return mergedBinding;
}

} // namespace Froth
