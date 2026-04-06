#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace le
{
#define LE_GRAPHICS_RESOURCE_ID(name) \
    struct name##ID \
    { \
        uint64_t id = 0; \
        name##ID() = default; \
        explicit name##ID(uint64_t id) : id(id) {} \
        explicit name##ID(void* ptr) : id(reinterpret_cast<uint64_t>(ptr)) {} \
        name##ID(const name##ID& other) : id(other.id) {} \
        explicit operator bool() const \
        { \
            return id != 0; \
        } \
        name##ID& operator=(const name##ID& other) \
        { \
            id = other.id; \
            return *this; \
        } \
        bool operator==(const name##ID& other) \
        { \
            return id == other.id; \
        }\
        bool operator!=(const name##ID& other) \
        { \
            return id != other.id; \
        } \
        \
    }

    // Graphics Resources
    LE_GRAPHICS_RESOURCE_ID(Material);
    LE_GRAPHICS_RESOURCE_ID(Mesh);
    LE_GRAPHICS_RESOURCE_ID(Shader);
    LE_GRAPHICS_RESOURCE_ID(Texture2D);
    LE_GRAPHICS_RESOURCE_ID(Texture2DArray);
    LE_GRAPHICS_RESOURCE_ID(RenderTarget);

    // GPU primitives
    LE_GRAPHICS_RESOURCE_ID(Buffer);
    LE_GRAPHICS_RESOURCE_ID(CommandBuffer);
    LE_GRAPHICS_RESOURCE_ID(DescriptorSet);
    LE_GRAPHICS_RESOURCE_ID(CommandPool);
    LE_GRAPHICS_RESOURCE_ID(Fence);
    LE_GRAPHICS_RESOURCE_ID(Image);
    LE_GRAPHICS_RESOURCE_ID(ImageView);
    LE_GRAPHICS_RESOURCE_ID(Pipeline);
    LE_GRAPHICS_RESOURCE_ID(PipelineLayout);
    LE_GRAPHICS_RESOURCE_ID(Semaphore);
    LE_GRAPHICS_RESOURCE_ID(Swapchain);
    LE_GRAPHICS_RESOURCE_ID(Surface);
    LE_GRAPHICS_RESOURCE_ID(ShaderModule);
    LE_GRAPHICS_RESOURCE_ID(DescriptorSetLayout);
    LE_GRAPHICS_RESOURCE_ID(Sampler);

    struct Offset3D
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    struct Extent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    enum class Format
    {
        R8,
        R8G8,
        R8G8B8,
        R8G8B8A8,
        R32G32B32_FLOAT
    };

    enum class PipelineStage
    {
        TOP_OF_PIPE,
        TRANSFER,
        FRAGMENT_SHADER,
    };

    enum class AccessFlagBits
    {
        TRANSFER_WRITE_BIT = 0x00001000,
        SHADER_READ_BIT = 0x00000020,
    };

    enum class DescriptorType
    {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        COMBINED_IMAGE_SAMPLER
    };

    enum class ShaderStageFlags : uint32_t
    {
        VERTEX   = 0x00000001,
        FRAGMENT = 0x00000010,
        ALL      = 0x7FFFFFFF,
    };

    enum class BufferUsageFlags : uint32_t
    {
        TRANSFER_SRC   = 0x00000001,
        TRANSFER_DST   = 0x00000002,
        UNIFORM_BUFFER = 0x00000010,
        STORAGE_BUFFER = 0x00000020,
        INDEX_BUFFER   = 0x00000040,
        VERTEX_BUFFER  = 0x00000080,
    };

    enum class QueueFamily
    {
        GRAPHICS,
        COMPUTE,
        TRANSFER,
    };

    enum class ImageViewType
    {
        TYPE_2D,
        TYPE_2D_ARRAY,
    };

    struct ImageInfo
    {
        std::size_t width = 0;
        std::size_t height = 0;
        std::size_t arrayLayers = 1;
        Format format = Format::R8G8B8A8;
    };

    struct PushConstantRange
    {
        std::size_t size = 0;
        std::size_t offset = 0;
        ShaderStageFlags stage = ShaderStageFlags::VERTEX;
    };

    struct StageInfo
    {
        std::string name;
        ShaderModuleID module;
        ShaderStageFlags stage;
    };

    enum class InputRate
    {
        VERTEX,
        INSTANCE,
    };

    struct VertexBinding
    {
        size_t binding = 0;
        size_t stride = 0;
        InputRate inputRate;
    };

    struct VertexAttribute
    {
        size_t location = 0;
        size_t binding = 0;
        size_t offset = 0;
        Format format = Format::R32G32B32_FLOAT;
    };

    struct PipelineInfo
    {
        PipelineLayoutID layout;
        std::span<Format> colorAttachmentFormats;
        Format depthFormat;
        std::span<StageInfo> stages;
        std::span<VertexBinding> vertexBindings;
        std::span<VertexAttribute> vertexAttributes;
    };
}
