#include "VulkanBuffer.hpp"

namespace le
{
    VmaAllocationCreateFlags VulkanBuffer::ToVmaFlags(BufferUsageFlags usage, bool mapped)
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
}
