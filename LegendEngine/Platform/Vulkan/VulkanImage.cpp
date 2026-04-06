#include "VulkanImage.hpp"

#include <LE/Common/Assert.hpp>

namespace le
{
    VkFormat VulkanImage::GetFormat(const Format format)
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
