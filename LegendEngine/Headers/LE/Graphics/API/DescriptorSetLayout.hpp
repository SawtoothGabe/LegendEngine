#pragma once

#include <LE/Graphics/API/Types.hpp>

namespace le
{
    class DescriptorSetLayout
    {
    public:
        struct Binding
        {
            std::size_t binding = 0;
            std::size_t descriptorCount = 0;
            DescriptorType descriptorType = DescriptorType::UNIFORM_BUFFER;
            ShaderStageFlags stageFlags = ShaderStageFlags::ALL;
        };

        virtual ~DescriptorSetLayout() = default;
    };
}
