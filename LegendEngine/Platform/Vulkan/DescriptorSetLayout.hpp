#pragma once

#include <unordered_map>
#include <LE/Graphics/Types.hpp>

#include "VkDefs.hpp"

namespace le
{
    struct DescriptorSetLayout
    {
        explicit DescriptorSetLayout(vk::Device device, std::span<DescriptorSetLayoutBinding> bindings);

        vk::DescriptorSetLayout layout;
        std::unordered_map<vk::DescriptorType, size_t> descriptorCounts;
    };
}