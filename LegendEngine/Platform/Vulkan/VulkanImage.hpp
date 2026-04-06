#pragma once

#include <vk_mem_alloc.h>

namespace le
{
    struct VulkanImage
    {
        VkImage image;
        VmaAllocation allocation;
    };
}
