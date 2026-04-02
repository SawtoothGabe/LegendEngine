#pragma once

#include <LE/Graphics/API/DynamicUniforms.hpp>

#include "GraphicsContext.hpp"
#include "API/DescriptorSetLayout.hpp"

namespace le::vk
{
    class DynamicUniforms final : public le::DynamicUniforms
    {
    public:
        explicit DynamicUniforms(GraphicsContext& context, UpdateFrequency frequency, le::DescriptorSetLayout& layout);
        ~DynamicUniforms() override;

        void UpdateUniformBuffer(le::Buffer& buffer, uint32_t binding) override;
        void UpdateStorageBuffer(le::Buffer& buffer, uint32_t binding) override;
        void UpdateSampledImage(le::Image& image, uint32_t binding) override;
        void UpdateSampler(le::Sampler& sampler, uint32_t binding) override;
        void UpdateCombinedImageSampler(le::Image& image, le::Sampler& sampler, uint32_t binding) override;

        void Invalidate() override;

        VkDescriptorSet GetDescriptorSet() const;
        static VkDescriptorType GetDescriptorType(DescriptorType type);
    private:
        void CreateDescriptorPool(size_t setCount, DescriptorSetLayout& layout);
        void Validate();
        bool IsValid() const;
        void WriteSet(uint32_t binding, VkWriteDescriptorSet* pWrite);

        TetherVulkan::GraphicsContext& m_context;

        VkDescriptorPool m_descriptorPool = nullptr;

        UpdateFrequency m_updateFrequency;

        size_t m_framesUntilValid = 0;
        size_t m_lastUpdatedFrame = std::numeric_limits<size_t>::max();
        std::vector<VkDescriptorSet> m_descriptorSets;
    };
}
