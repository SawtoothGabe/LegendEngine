#pragma once

#include <LE/Graphics/API/Buffer.hpp>

#include <vk_mem_alloc.h>

namespace le::vk
{
    class Buffer : public le::Buffer
    {
    public:
        struct Desc
        {
            VkBuffer buffer;
            size_t size;
        };

        virtual Desc GetDesc() = 0;
    protected:
        static VkBufferUsageFlags ToVulkanUsageFlags(UsageFlags usage);
        static VmaAllocationCreateFlags ToVmaAllocationCreateFlags(UsageFlags usage, bool mapped);
    };
}
