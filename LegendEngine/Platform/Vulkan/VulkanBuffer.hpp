#pragma once

#include <vk_mem_alloc.h>
#include "VkDefs.hpp"

namespace le
{
    struct VulkanBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
    };
}