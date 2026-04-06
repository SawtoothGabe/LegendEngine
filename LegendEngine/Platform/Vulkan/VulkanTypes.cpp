#include "VulkanTypes.hpp"

namespace le
{
    VmaAllocationCreateFlags VulkanTypes::GetVmaFlags(const BufferUsageFlags usage, const bool mapped)
    {
        VmaAllocationCreateFlags flags;
        switch (usage)
        {
            case BufferUsageFlags::TRANSFER_SRC:
            case BufferUsageFlags::UNIFORM_BUFFER:
            case BufferUsageFlags::STORAGE_BUFFER:
            {
                flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
                break;

            default: flags = 0;
        }

        if (mapped)
            flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

        return flags;
    }

    VkFormat VulkanTypes::GetFormat(const Format format)
    {
        switch (format)
        {
            case Format::R8: return VK_FORMAT_R8_SRGB;
            case Format::R8G8: return VK_FORMAT_R8G8_SRGB;
            case Format::R8G8B8: return VK_FORMAT_R8G8B8_SRGB;
            case Format::R8G8B8A8: return VK_FORMAT_R8G8B8A8_SRGB;
            case Format::R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        }

        LE_ASSERT(false, "Unknown image format");
        return VK_FORMAT_UNDEFINED;
    }

    vk::Format VulkanTypes::GetVkFormat(const Format format)
    {
        return static_cast<vk::Format>(GetFormat(format));
    }

    vk::ShaderStageFlagBits VulkanTypes::GetShaderStageFlag(ShaderStageFlags stage)
    {
        switch (stage)
        {
            case ShaderStageFlags::VERTEX: return vk::ShaderStageFlagBits::eVertex;
            case ShaderStageFlags::FRAGMENT: return vk::ShaderStageFlagBits::eFragment;
            case ShaderStageFlags::ALL: return vk::ShaderStageFlagBits::eAll;
        }

        LE_ASSERT(false, "Unknown shader stage");
        return vk::ShaderStageFlagBits::eAll;
    }

    vk::ShaderStageFlagBits VulkanTypes::GetShaderStageFlags(ShaderStageFlags stage)
    {
        return static_cast<vk::ShaderStageFlagBits>(stage);
    }

    vk::VertexInputRate VulkanTypes::GetVertexInputRate(const InputRate inputRate)
    {
        switch (inputRate)
        {
            case InputRate::VERTEX: return vk::VertexInputRate::eVertex;
            case InputRate::INSTANCE: return vk::VertexInputRate::eInstance;
        }

        LE_ASSERT(false, "Unknown input rate");
        return vk::VertexInputRate::eVertex;
    }

    vk::DescriptorType VulkanTypes::GetDescriptorType(DescriptorType type)
    {
        switch (type)
        {
            case DescriptorType::UNIFORM_BUFFER: return vk::DescriptorType::eUniformBuffer;
            case DescriptorType::STORAGE_BUFFER: return vk::DescriptorType::eStorageBuffer;
            case DescriptorType::COMBINED_IMAGE_SAMPLER: return vk::DescriptorType::eCombinedImageSampler;
        }

        LE_ASSERT(false, "Unknown descriptor type");
        return vk::DescriptorType::eUniformBuffer;
    }

    vk::Filter VulkanTypes::GetFilter(Filter filter)
    {
        switch (filter)
        {
            case Filter::NEAREST: return vk::Filter::eNearest;
            case Filter::LINEAR: return vk::Filter::eLinear;
        }

        LE_ASSERT(false, "Unknown sampler filter mode");
        return vk::Filter::eNearest;
    }

    vk::SamplerAddressMode VulkanTypes::GetSamplerAddressMode(const AddressMode addressMode)
    {
        switch (addressMode)
        {
            case AddressMode::REPEAT: return vk::SamplerAddressMode::eRepeat;
            case AddressMode::MIRRORED_REPEAT: return vk::SamplerAddressMode::eMirroredRepeat;
            case AddressMode::CLAMP_TO_EDGE: return vk::SamplerAddressMode::eClampToEdge;
            case AddressMode::CLAMP_TO_BORDER: return vk::SamplerAddressMode::eClampToBorder;
        }

        LE_ASSERT(false, "Unknown sampler addressMode");
        return vk::SamplerAddressMode::eRepeat;
    }

    vk::BorderColor VulkanTypes::GetBorderColor(const BorderColor borderColor)
    {
        switch (borderColor)
        {
            case BorderColor::TRANSPARENT_BLACK: return vk::BorderColor::eIntTransparentBlack; break;
            case BorderColor::OPAQUE_BLACK: return vk::BorderColor::eIntOpaqueBlack; break;
            case BorderColor::OPAQUE_WHITE: return vk::BorderColor::eIntOpaqueWhite; break;
        }

        LE_ASSERT(false, "Unknown border color");
        return vk::BorderColor::eIntOpaqueBlack;
    }
}
