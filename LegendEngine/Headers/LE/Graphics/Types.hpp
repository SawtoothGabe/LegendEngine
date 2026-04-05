#pragma once

#include <cstdint>
#include <functional>

namespace le
{
    struct Handle
    {
        Handle(const uint64_t id, const std::function<void(uint64_t)>& free)
            : id(id), m_free(free)
        {}

        ~Handle()
        {
            m_free(id);
        }

        uint64_t id = 0;
    private:
        std::function<void(uint64_t)> m_free = nullptr;
    };

    struct MaterialHandle : Handle {};
    struct MeshHandle : Handle {};
    struct ShaderHandle : Handle {};
    struct Texture2DHandle : Handle {};
    struct Texture2DArrayHandle : Handle {};
    struct RenderTargetHandle : Handle {};

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

    LE_GRAPHICS_RESOURCE_ID(Buffer);
    LE_GRAPHICS_RESOURCE_ID(CommandBuffer);
    LE_GRAPHICS_RESOURCE_ID(DescriptorSet);
    LE_GRAPHICS_RESOURCE_ID(CommandPool);
    LE_GRAPHICS_RESOURCE_ID(Fence);
    LE_GRAPHICS_RESOURCE_ID(Image);
    LE_GRAPHICS_RESOURCE_ID(ImageView);
    LE_GRAPHICS_RESOURCE_ID(Pipeline);
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
}