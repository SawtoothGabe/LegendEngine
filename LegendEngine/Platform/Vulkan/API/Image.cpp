#include "API/Image.hpp"

#include <VkDefs.hpp>
#include <LE/Common/Assert.hpp>

namespace le::vk
{
    Image::Image(GraphicsContext& context, const Info& info)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        VkFormat format = GetFormat(info.format);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = info.width;
        imageInfo.extent.height = info.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = info.arrayLayers;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.format = format;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        LE_CHECK_VK(vmaCreateImage(
            m_context.GetAllocator(),
            &imageInfo,
            &allocInfo,
            &m_image,
            &m_allocation,
            nullptr
        ));

        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
        switch (info.type)
        {
            case Type::TYPE_2D: viewType = VK_IMAGE_VIEW_TYPE_2D; break;
            case Type::TYPE_2D_ARRAY: viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY; break;

            default: LE_ASSERT(false, "Unknown image type"); break;
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.format = format;
        viewInfo.viewType = viewType;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = info.arrayLayers;

        LE_CHECK_VK(vkCreateImageView(m_context.GetDevice(), &viewInfo, nullptr, &m_view));
    }

    Image::~Image()
    {
        vmaDestroyImage(m_context.GetAllocator(), m_image, m_allocation);
        vkDestroyImageView(m_context.GetDevice(), m_view, nullptr);
    }

    VkImage Image::GetImage() const
    {
        return m_image;
    }

    VkImageView Image::GetImageView() const
    {
        return m_view;
    }

    VkImageLayout Image::GetImageLayout(const Layout layout)
    {
        switch (layout)
        {
            case Layout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
            case Layout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
            case Layout::COLOR_ATTACHMENT_OPTIMAL: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case Layout::SHADER_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case Layout::TRANSFER_SRC_OPTIMAL: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case Layout::TRANSFER_DST_OPTIMAL: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case Layout::READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            case Layout::PRESENT_SRC: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        LE_ASSERT(false, "Unknown image layout");
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    VkImageAspectFlags Image::GetImageAspectFlags(const Aspect aspect)
    {
        switch (aspect)
        {
            case Aspect::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
            case Aspect::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
            case Aspect::STENCIL: return VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        LE_ASSERT(false, "Unknown image aspect");
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkFormat Image::GetFormat(const Format format)
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
}
