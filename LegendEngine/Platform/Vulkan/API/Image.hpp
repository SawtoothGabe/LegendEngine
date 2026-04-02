#pragma once

#include <LE/Graphics/API/Image.hpp>

#include "GraphicsContext.hpp"

#include <vk_mem_alloc.h>

namespace le::vk
{
    class Image final : public le::Image
    {
    public:
        explicit Image(GraphicsContext& context, const Info& info);
        ~Image() override;

        VkImage GetImage() const;
        VkImageView GetImageView() const;

        static VkImageLayout GetImageLayout(Layout layout);
        static VkImageAspectFlags GetImageAspectFlags(Aspect aspect);
        static VkFormat GetFormat(Format format);
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkImage m_image = nullptr;
        VkImageView m_view = nullptr;
        VmaAllocation m_allocation = nullptr;
    };
}
