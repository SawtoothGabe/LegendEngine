#pragma once

#include <vk_mem_alloc.h>
#include <LE/Graphics/Types.hpp>

namespace le
{
    struct VulkanImage
    {
        VkImage image;
        VmaAllocation allocation;

        static VkFormat GetFormat(Format format);
    };
}
