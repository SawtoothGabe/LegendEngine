#pragma once

#include <LE/Graphics/API/DescriptorSetLayout.hpp>

#include "GraphicsContext.hpp"

namespace le::vk
{
    class DescriptorSetLayout : public le::DescriptorSetLayout
    {
    public:
        explicit DescriptorSetLayout(GraphicsContext& context, std::span<Binding> bindings);
        ~DescriptorSetLayout() override;

        VkDescriptorSetLayout GetDescriptorSetLayout() const;
        std::unordered_map<VkDescriptorType, size_t> GetDescriptorCounts() const;
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;
        std::unordered_map<VkDescriptorType, size_t> m_descriptorCounts;
    };
}
