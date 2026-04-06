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
        }

        LE_ASSERT(false, "Unknown image format");
        return VK_FORMAT_UNDEFINED;
    }

    vk::Format VulkanTypes::GetVkFormat(const Format format)
    {
        return static_cast<vk::Format>(GetFormat(format));
    }
}
