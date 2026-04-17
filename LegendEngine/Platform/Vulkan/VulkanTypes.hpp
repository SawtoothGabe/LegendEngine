#pragma once

#include <vk_mem_alloc.h>
#include <LE/Graphics/ShaderInfo.hpp>
#include <LE/Graphics/Types.hpp>
#include "VkDefs.hpp"

namespace le::VulkanTypes
{
    VmaAllocationCreateFlags GetVmaFlags(BufferUsageFlags usage, bool mapped);
    VkFormat GetFormat(Format format);
    vk::Format GetVkFormat(Format format);
    vk::ShaderStageFlagBits GetShaderStageFlag(ShaderStageFlagBits stage);
    vk::ShaderStageFlagBits GetShaderStageFlags(ShaderStageFlagBits stage);
    vk::VertexInputRate GetVertexInputRate(InputRate inputRate);
    vk::DescriptorType GetDescriptorType(DescriptorType type);
    vk::Filter GetFilter(Filter filter);
    vk::SamplerAddressMode GetSamplerAddressMode(AddressMode addressMode);
    vk::BorderColor GetBorderColor(BorderColor borderColor);
    vk::PipelineStageFlags GetPipelineStage(PipelineStage stage);
    vk::ImageAspectFlags GetImageAspectFlags(ImageAspect aspect);
    vk::ImageLayout GetImageLayout(ImageLayout layout);
    vk::PipelineBindPoint GetPipelineBindPoint(PipelineBindPoint bindPoint);
    vk::CullModeFlags GetCullModeFlags(CullMode cullMode);
    vk::AccessFlags GetAccessFlags(AccessFlags accessFlags);
    vk::ShaderStageFlagBits GetShaderStageFlag(ShaderStage stage);

    Format ToFormat(vk::Format format);
}