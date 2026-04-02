#include "API/Sampler.hpp"

#include "VkDefs.hpp"

namespace le::vk
{
    Sampler::Sampler(GraphicsContext& context, const Info& info)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        VkFilter filter = VK_FILTER_NEAREST;
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        switch (info.filter)
        {
            case Filter::NEAREST: filter = VK_FILTER_NEAREST; break;
            case Filter::LINEAR: filter = VK_FILTER_LINEAR; break;

            default: LE_ASSERT(false, "Unknown sampler filter mode"); break;
        }

        switch (info.addressMode)
        {
            case AddressMode::REPEAT: addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
            case AddressMode::MIRRORED_REPEAT: addressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; break;
            case AddressMode::CLAMP_TO_EDGE: addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
            case AddressMode::CLAMP_TO_BORDER: addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; break;

            default: LE_ASSERT(false, "Unknown sampler addressMode"); break;
        }

        switch (info.borderColor)
        {
            case BorderColor::TRANSPARENT_BLACK: borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; break;
            case BorderColor::OPAQUE_BLACK: borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; break;
            case BorderColor::OPAQUE_WHITE: borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE; break;

            default: LE_ASSERT(false, "Unknown border color"); break;
        }

        VkSamplerCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.minFilter = filter;
        createInfo.magFilter = filter;
        createInfo.addressModeU = addressMode;
        createInfo.addressModeV = addressMode;
        createInfo.addressModeW = addressMode;
        createInfo.anisotropyEnable = false;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_context.GetPhysicalDevice(), &properties);

        createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        createInfo.borderColor = borderColor;
        createInfo.unnormalizedCoordinates = false;
        createInfo.compareEnable = false;
        createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.mipLodBias = 0.0f;
        createInfo.minLod = 0.0f;
        createInfo.maxLod = 0.0f;

        LE_CHECK_VK(vkCreateSampler(m_context.GetDevice(), &createInfo, nullptr, &m_sampler));
    }

    Sampler::~Sampler()
    {
        vkDestroySampler(m_context.GetDevice(), m_sampler, nullptr);
    }

    VkSampler Sampler::GetSampler() const
    {
        return m_sampler;
    }
}
