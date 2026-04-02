#include "API/DescriptorSetLayout.hpp"

#include "API/DynamicUniforms.hpp"
#include "VkDefs.hpp"

namespace le::vk
{
    DescriptorSetLayout::DescriptorSetLayout(GraphicsContext& context, std::span<Binding> bindings)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        std::vector<VkDescriptorSetLayoutBinding> vkBindings;
        vkBindings.reserve(bindings.size());
        for (Binding& binding : bindings)
        {
            VkDescriptorSetLayoutBinding vkBinding{};
            vkBinding.binding = binding.binding;
            vkBinding.descriptorCount = binding.descriptorCount;
            vkBinding.descriptorType = DynamicUniforms::GetDescriptorType(binding.descriptorType);
            vkBinding.stageFlags = static_cast<VkShaderStageFlags>(binding.stageFlags);

            m_descriptorCounts[vkBinding.descriptorType] += vkBinding.descriptorCount;
            vkBindings.push_back(vkBinding);
        }

        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(vkBindings.size());
        info.pBindings = vkBindings.data();

        LE_CHECK_VK(vkCreateDescriptorSetLayout(m_context.GetDevice(), &info,
            nullptr, &m_descriptorSetLayout));
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(m_context.GetDevice(), m_descriptorSetLayout, nullptr);
    }

    VkDescriptorSetLayout DescriptorSetLayout::GetDescriptorSetLayout() const
    {
        return m_descriptorSetLayout;
    }

    std::unordered_map<VkDescriptorType, size_t> DescriptorSetLayout::GetDescriptorCounts() const
    {
        return m_descriptorCounts;
    }
}
