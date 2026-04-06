#pragma once

#include <vk_mem_alloc.h>

#include <LE/Graphics/Types.hpp>
#include "VkDefs.hpp"

namespace le
{
    struct VulkanBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;

        static VmaAllocationCreateFlags ToVmaFlags(BufferUsageFlags usage, bool mapped);
    };
}