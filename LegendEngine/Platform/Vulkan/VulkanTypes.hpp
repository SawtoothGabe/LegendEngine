#pragma once

#include <vk_mem_alloc.h>
#include <LE/Graphics/Types.hpp>
#include "VkDefs.hpp"

namespace le::VulkanTypes
{
    static VmaAllocationCreateFlags GetVmaFlags(BufferUsageFlags usage, bool mapped);
    static VkFormat GetFormat(Format format);
    static vk::Format GetVkFormat(Format format);
    static vk::ShaderStageFlagBits GetShaderStageFlag(ShaderStageFlags stage);
    static vk::VertexInputRate GetVertexInputRate(InputRate inputRate);
}