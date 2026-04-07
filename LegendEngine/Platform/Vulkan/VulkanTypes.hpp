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
    static vk::ShaderStageFlagBits GetShaderStageFlags(ShaderStageFlags stage);
    static vk::VertexInputRate GetVertexInputRate(InputRate inputRate);
    static vk::DescriptorType GetDescriptorType(DescriptorType type);
    static vk::Filter GetFilter(Filter filter);
    static vk::SamplerAddressMode GetSamplerAddressMode(AddressMode addressMode);
    static vk::BorderColor GetBorderColor(BorderColor borderColor);
    static vk::PipelineStageFlags GetPipelineStage(PipelineStage stage);
    static vk::ImageAspectFlags GetImageAspectFlags(ImageAspect aspect);
    static vk::ImageLayout GetImageLayout(ImageLayout layout);
    static vk::PipelineBindPoint GetPipelineBindPoint(PipelineBindPoint bindPoint);
    static vk::CullModeFlags GetCullModeFlags(CullMode cullMode);
}