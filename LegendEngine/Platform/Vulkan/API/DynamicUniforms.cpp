#include "API/DynamicUniforms.hpp"

#include <API/Image.hpp>

#include "VkDefs.hpp"
#include "API/Buffer.hpp"
#include "API/Sampler.hpp"
#include "API/DescriptorSetLayout.hpp"

#include <LE/Application.hpp>

namespace le::vk
{
    DynamicUniforms::DynamicUniforms(GraphicsContext& context, const UpdateFrequency frequency, le::DescriptorSetLayout& layout)
        :
        m_context(context.GetTetherGraphicsContext()),
        m_updateFrequency(frequency),
        m_framesUntilValid(Application::FRAMES_IN_FLIGHT)
    {
        auto& vkLayout = static_cast<DescriptorSetLayout&>(layout);
        const size_t setCount = frequency == UpdateFrequency::PER_FRAME ? Application::FRAMES_IN_FLIGHT : 1;

        CreateDescriptorPool(setCount, vkLayout);

        const std::vector layouts(setCount, vkLayout.GetDescriptorSetLayout());
        m_descriptorSets.resize(setCount);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = m_descriptorSets.size();
        allocInfo.pSetLayouts = layouts.data();

        LE_CHECK_VK(vkAllocateDescriptorSets(m_context.GetDevice(), &allocInfo, m_descriptorSets.data()));
    }

    DynamicUniforms::~DynamicUniforms()
    {
        vkDestroyDescriptorPool(m_context.GetDevice(), m_descriptorPool, nullptr);
    }

    void DynamicUniforms::UpdateUniformBuffer(le::Buffer& buffer, const uint32_t binding)
    {
        if (IsValid())
            return;

        auto& vkBuffer = static_cast<Buffer&>(buffer);
        const auto [bufferObject, size] = vkBuffer.GetDesc();

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = bufferObject;
        bufferInfo.range = size;

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;

        WriteSet(binding, &write);
    }

    void DynamicUniforms::UpdateStorageBuffer(le::Buffer& buffer, uint32_t binding)
    {
        if (IsValid())
            return;

        auto& vkBuffer = static_cast<Buffer&>(buffer);
        const auto [bufferObject, size] = vkBuffer.GetDesc();

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = bufferObject;
        bufferInfo.range = size;

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.pBufferInfo = &bufferInfo;

        WriteSet(binding, &write);
    }

    void DynamicUniforms::UpdateSampledImage(le::Image& image, const uint32_t binding)
    {
        if (IsValid())
            return;

        const auto& vkImage = static_cast<Image&>(image);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vkImage.GetImageView();

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.pImageInfo = &imageInfo;

        WriteSet(binding, &write);
    }

    void DynamicUniforms::UpdateSampler(le::Sampler& sampler, uint32_t binding)
    {
        if (IsValid())
            return;

        const auto& vkSampler = static_cast<Sampler&>(sampler);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = vkSampler.GetSampler();

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write.pImageInfo = &imageInfo;

        WriteSet(binding, &write);
    }

    void DynamicUniforms::UpdateCombinedImageSampler(le::Image& image, le::Sampler& sampler, uint32_t binding)
    {
        if (IsValid())
            return;

        const auto& vkImage = static_cast<Image&>(image);
        const auto& vkSampler = static_cast<Sampler&>(sampler);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = vkSampler.GetSampler();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vkImage.GetImageView();

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;

        WriteSet(binding, &write);
    }

    void DynamicUniforms::Invalidate()
    {
        m_framesUntilValid = m_descriptorSets.size() + 1;
    }

    void DynamicUniforms::CreateDescriptorPool(size_t setCount, DescriptorSetLayout& layout)
    {
        std::unordered_map<VkDescriptorType, size_t> layoutSetCounts = layout.GetDescriptorCounts();
        size_t maxSets = 0;

        std::vector<VkDescriptorPoolSize> poolSizeStructs;
        poolSizeStructs.reserve(layoutSetCounts.size());
        for (const auto [type, size] : layoutSetCounts)
        {
            VkDescriptorPoolSize poolSize;
            poolSize.type = type;
            poolSize.descriptorCount = size * setCount;

            maxSets += poolSize.descriptorCount;
            poolSizeStructs.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pNext = nullptr;
        poolInfo.maxSets = maxSets;
        poolInfo.poolSizeCount = poolSizeStructs.size();
        poolInfo.pPoolSizes = poolSizeStructs.data();

        LE_CHECK_VK(vkCreateDescriptorPool(m_context.GetDevice(), &poolInfo, nullptr, &m_descriptorPool));
    }

    VkDescriptorType DynamicUniforms::GetDescriptorType(const DescriptorType type)
    {
        switch (type)
        {
            case DescriptorType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case DescriptorType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case DescriptorType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        LE_ASSERT(false, "Unknown descriptor type");
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    void DynamicUniforms::Validate()
    {
        if (m_lastUpdatedFrame == Application::Get().GetCurrentFrame())
            return;

        m_framesUntilValid = std::max<size_t>(m_framesUntilValid - 1, 0);
        m_lastUpdatedFrame = Application::Get().GetCurrentFrame();
    }

    bool DynamicUniforms::IsValid() const
    {
        return m_framesUntilValid == 0 && m_updateFrequency == UpdateFrequency::PER_FRAME;
    }

    void DynamicUniforms::WriteSet(const uint32_t binding, VkWriteDescriptorSet* pWrite)
    {
        pWrite->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        pWrite->dstBinding = binding;
        pWrite->dstSet = GetDescriptorSet();
        pWrite->descriptorCount = 1;

        // Occasional is something like a sampler that only updates when the user changes
        // graphics settings. A set can't be updated when it is in use. For the occasional
        // update where there isn't a dedicated set per frame in flight, we'll wait for
        // the frames to render. After it isn't in use, update it.
        if (m_updateFrequency == UpdateFrequency::OCCASIONAL)
            vkDeviceWaitIdle(m_context.GetDevice());

        vkUpdateDescriptorSets(m_context.GetDevice(), 1, pWrite, 0, nullptr);
        Validate();
    }

    VkDescriptorSet DynamicUniforms::GetDescriptorSet() const
    {
        return m_descriptorSets[Application::Get().GetCurrentFrame()];
    }
}
