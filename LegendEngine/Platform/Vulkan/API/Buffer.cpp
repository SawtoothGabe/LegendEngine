#include "API/Buffer.hpp"

namespace le::vk
{
    VkBufferUsageFlags Buffer::ToVulkanUsageFlags(const UsageFlags usage)
    {
        return static_cast<uint32_t>(usage);
    }

    VmaAllocationCreateFlags Buffer::ToVmaAllocationCreateFlags(const UsageFlags usage, bool mapped)
    {
        VmaAllocationCreateFlags flags;
        switch (usage)
        {
            case UsageFlags::TRANSFER_SRC:
            case UsageFlags::UNIFORM_BUFFER:
            case UsageFlags::STORAGE_BUFFER:
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
}
