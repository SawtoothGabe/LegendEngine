#include "DescriptorSetLayout.hpp"
#include "VulkanTypes.hpp"

namespace le
{
    DescriptorSetLayout::DescriptorSetLayout(const vk::Device device, std::span<DescriptorSetLayoutBinding> bindings)
    {
        std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
        vkBindings.reserve(bindings.size());

        for (DescriptorSetLayoutBinding& binding : bindings)
        {
            vk::DescriptorSetLayoutBinding vkBinding{};
            vkBinding.binding = binding.binding;
            vkBinding.descriptorCount = binding.descriptorCount;
            vkBinding.descriptorType = VulkanTypes::GetDescriptorType(binding.descriptorType);
            vkBinding.stageFlags = VulkanTypes::GetShaderStageFlags(binding.stageFlags);

            descriptorCounts[vkBinding.descriptorType] += vkBinding.descriptorCount;
            vkBindings.push_back(vkBinding);
        }

        vk::DescriptorSetLayoutCreateInfo info(
            {}, vkBindings.size(), vkBindings.data()
        );

        layout = device.createDescriptorSetLayout(info);
    }
}
