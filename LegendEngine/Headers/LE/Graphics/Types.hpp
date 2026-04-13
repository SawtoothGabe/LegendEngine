#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <LE/Common/Enums.hpp>
#include <LE/Math/Types.hpp>
#include <lesh/Shader.hpp>

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
    LE_GRAPHICS_RESOURCE_ID(PoolManager);

    // GPU primitives
    LE_GRAPHICS_RESOURCE_ID(Buffer);
    LE_GRAPHICS_RESOURCE_ID(CommandBuffer);
    LE_GRAPHICS_RESOURCE_ID(DescriptorSet);
    LE_GRAPHICS_RESOURCE_ID(DescriptorPool);
    LE_GRAPHICS_RESOURCE_ID(CommandPool);
    LE_GRAPHICS_RESOURCE_ID(Fence);
    LE_GRAPHICS_RESOURCE_ID(Framebuffer);
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
    LE_GRAPHICS_RESOURCE_ID(Queue);

    static_assert(sizeof(void*) == sizeof(BufferID));

    struct Offset3D
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    struct Offset2D
    {
        int32_t x;
        int32_t y;
    };

    struct Extent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    struct Extent2D
    {
        uint32_t width;
        uint32_t height;
    };

    enum class Format
    {
        UNDEFINED = 0,
        R4G4_UNORM_PACK8 = 1,
        R4G4B4A4_UNORM_PACK16 = 2,
        B4G4R4A4_UNORM_PACK16 = 3,
        R5G6B5_UNORM_PACK16 = 4,
        B5G6R5_UNORM_PACK16 = 5,
        R5G5B5A1_UNORM_PACK16 = 6,
        B5G5R5A1_UNORM_PACK16 = 7,
        A1R5G5B5_UNORM_PACK16 = 8,
        R8_UNORM = 9,
        R8_SNORM = 10,
        R8_USCALED = 11,
        R8_SSCALED = 12,
        R8_UINT = 13,
        R8_SINT = 14,
        R8_SRGB = 15,
        R8G8_UNORM = 16,
        R8G8_SNORM = 17,
        R8G8_USCALED = 18,
        R8G8_SSCALED = 19,
        R8G8_UINT = 20,
        R8G8_SINT = 21,
        R8G8_SRGB = 22,
        R8G8B8_UNORM = 23,
        R8G8B8_SNORM = 24,
        R8G8B8_USCALED = 25,
        R8G8B8_SSCALED = 26,
        R8G8B8_UINT = 27,
        R8G8B8_SINT = 28,
        R8G8B8_SRGB = 29,
        B8G8R8_UNORM = 30,
        B8G8R8_SNORM = 31,
        B8G8R8_USCALED = 32,
        B8G8R8_SSCALED = 33,
        B8G8R8_UINT = 34,
        B8G8R8_SINT = 35,
        B8G8R8_SRGB = 36,
        R8G8B8A8_UNORM = 37,
        R8G8B8A8_SNORM = 38,
        R8G8B8A8_USCALED = 39,
        R8G8B8A8_SSCALED = 40,
        R8G8B8A8_UINT = 41,
        R8G8B8A8_SINT = 42,
        R8G8B8A8_SRGB = 43,
        B8G8R8A8_UNORM = 44,
        B8G8R8A8_SNORM = 45,
        B8G8R8A8_USCALED = 46,
        B8G8R8A8_SSCALED = 47,
        B8G8R8A8_UINT = 48,
        B8G8R8A8_SINT = 49,
        B8G8R8A8_SRGB = 50,
        A8B8G8R8_UNORM_PACK32 = 51,
        A8B8G8R8_SNORM_PACK32 = 52,
        A8B8G8R8_USCALED_PACK32 = 53,
        A8B8G8R8_SSCALED_PACK32 = 54,
        A8B8G8R8_UINT_PACK32 = 55,
        A8B8G8R8_SINT_PACK32 = 56,
        A8B8G8R8_SRGB_PACK32 = 57,
        A2R10G10B10_UNORM_PACK32 = 58,
        A2R10G10B10_SNORM_PACK32 = 59,
        A2R10G10B10_USCALED_PACK32 = 60,
        A2R10G10B10_SSCALED_PACK32 = 61,
        A2R10G10B10_UINT_PACK32 = 62,
        A2R10G10B10_SINT_PACK32 = 63,
        A2B10G10R10_UNORM_PACK32 = 64,
        A2B10G10R10_SNORM_PACK32 = 65,
        A2B10G10R10_USCALED_PACK32 = 66,
        A2B10G10R10_SSCALED_PACK32 = 67,
        A2B10G10R10_UINT_PACK32 = 68,
        A2B10G10R10_SINT_PACK32 = 69,
        R16_UNORM = 70,
        R16_SNORM = 71,
        R16_USCALED = 72,
        R16_SSCALED = 73,
        R16_UINT = 74,
        R16_SINT = 75,
        R16_SFLOAT = 76,
        R16G16_UNORM = 77,
        R16G16_SNORM = 78,
        R16G16_USCALED = 79,
        R16G16_SSCALED = 80,
        R16G16_UINT = 81,
        R16G16_SINT = 82,
        R16G16_SFLOAT = 83,
        R16G16B16_UNORM = 84,
        R16G16B16_SNORM = 85,
        R16G16B16_USCALED = 86,
        R16G16B16_SSCALED = 87,
        R16G16B16_UINT = 88,
        R16G16B16_SINT = 89,
        R16G16B16_SFLOAT = 90,
        R16G16B16A16_UNORM = 91,
        R16G16B16A16_SNORM = 92,
        R16G16B16A16_USCALED = 93,
        R16G16B16A16_SSCALED = 94,
        R16G16B16A16_UINT = 95,
        R16G16B16A16_SINT = 96,
        R16G16B16A16_SFLOAT = 97,
        R32_UINT = 98,
        R32_SINT = 99,
        R32_SFLOAT = 100,
        R32G32_UINT = 101,
        R32G32_SINT = 102,
        R32G32_SFLOAT = 103,
        R32G32B32_UINT = 104,
        R32G32B32_SINT = 105,
        R32G32B32_SFLOAT = 106,
        R32G32B32A32_UINT = 107,
        R32G32B32A32_SINT = 108,
        R32G32B32A32_SFLOAT = 109,
        R64_UINT = 110,
        R64_SINT = 111,
        R64_SFLOAT = 112,
        R64G64_UINT = 113,
        R64G64_SINT = 114,
        R64G64_SFLOAT = 115,
        R64G64B64_UINT = 116,
        R64G64B64_SINT = 117,
        R64G64B64_SFLOAT = 118,
        R64G64B64A64_UINT = 119,
        R64G64B64A64_SINT = 120,
        R64G64B64A64_SFLOAT = 121,
        B10G11R11_UFLOAT_PACK32 = 122,
        E5B9G9R9_UFLOAT_PACK32 = 123,
        D16_UNORM = 124,
        X8_D24_UNORM_PACK32 = 125,
        D32_SFLOAT = 126,
        S8_UINT = 127,
        D16_UNORM_S8_UINT = 128,
        D24_UNORM_S8_UINT = 129,
        D32_SFLOAT_S8_UINT = 130,
        BC1_RGB_UNORM_BLOCK = 131,
        BC1_RGB_SRGB_BLOCK = 132,
        BC1_RGBA_UNORM_BLOCK = 133,
        BC1_RGBA_SRGB_BLOCK = 134,
        BC2_UNORM_BLOCK = 135,
        BC2_SRGB_BLOCK = 136,
        BC3_UNORM_BLOCK = 137,
        BC3_SRGB_BLOCK = 138,
        BC4_UNORM_BLOCK = 139,
        BC4_SNORM_BLOCK = 140,
        BC5_UNORM_BLOCK = 141,
        BC5_SNORM_BLOCK = 142,
        BC6H_UFLOAT_BLOCK = 143,
        BC6H_SFLOAT_BLOCK = 144,
        BC7_UNORM_BLOCK = 145,
        BC7_SRGB_BLOCK = 146,
        ETC2_R8G8B8_UNORM_BLOCK = 147,
        ETC2_R8G8B8_SRGB_BLOCK = 148,
        ETC2_R8G8B8A1_UNORM_BLOCK = 149,
        ETC2_R8G8B8A1_SRGB_BLOCK = 150,
        ETC2_R8G8B8A8_UNORM_BLOCK = 151,
        ETC2_R8G8B8A8_SRGB_BLOCK = 152,
        EAC_R11_UNORM_BLOCK = 153,
        EAC_R11_SNORM_BLOCK = 154,
        EAC_R11G11_UNORM_BLOCK = 155,
        EAC_R11G11_SNORM_BLOCK = 156,
        ASTC_4x4_UNORM_BLOCK = 157,
        ASTC_4x4_SRGB_BLOCK = 158,
        ASTC_5x4_UNORM_BLOCK = 159,
        ASTC_5x4_SRGB_BLOCK = 160,
        ASTC_5x5_UNORM_BLOCK = 161,
        ASTC_5x5_SRGB_BLOCK = 162,
        ASTC_6x5_UNORM_BLOCK = 163,
        ASTC_6x5_SRGB_BLOCK = 164,
        ASTC_6x6_UNORM_BLOCK = 165,
        ASTC_6x6_SRGB_BLOCK = 166,
        ASTC_8x5_UNORM_BLOCK = 167,
        ASTC_8x5_SRGB_BLOCK = 168,
        ASTC_8x6_UNORM_BLOCK = 169,
        ASTC_8x6_SRGB_BLOCK = 170,
        ASTC_8x8_UNORM_BLOCK = 171,
        ASTC_8x8_SRGB_BLOCK = 172,
        ASTC_10x5_UNORM_BLOCK = 173,
        ASTC_10x5_SRGB_BLOCK = 174,
        ASTC_10x6_UNORM_BLOCK = 175,
        ASTC_10x6_SRGB_BLOCK = 176,
        ASTC_10x8_UNORM_BLOCK = 177,
        ASTC_10x8_SRGB_BLOCK = 178,
        ASTC_10x10_UNORM_BLOCK = 179,
        ASTC_10x10_SRGB_BLOCK = 180,
        ASTC_12x10_UNORM_BLOCK = 181,
        ASTC_12x10_SRGB_BLOCK = 182,
        ASTC_12x12_UNORM_BLOCK = 183,
        ASTC_12x12_SRGB_BLOCK = 184,
        G8B8G8R8_422_UNORM = 1000156000,
        B8G8R8G8_422_UNORM = 1000156001,
        G8_B8_R8_3PLANE_420_UNORM = 1000156002,
        G8_B8R8_2PLANE_420_UNORM = 1000156003,
        G8_B8_R8_3PLANE_422_UNORM = 1000156004,
        G8_B8R8_2PLANE_422_UNORM = 1000156005,
        G8_B8_R8_3PLANE_444_UNORM = 1000156006,
        R10X6_UNORM_PACK16 = 1000156007,
        R10X6G10X6_UNORM_2PACK16 = 1000156008,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
        R12X4_UNORM_PACK16 = 1000156017,
        R12X4G12X4_UNORM_2PACK16 = 1000156018,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
        G16B16G16R16_422_UNORM = 1000156027,
        B16G16R16G16_422_UNORM = 1000156028,
        G16_B16_R16_3PLANE_420_UNORM = 1000156029,
        G16_B16R16_2PLANE_420_UNORM = 1000156030,
        G16_B16_R16_3PLANE_422_UNORM = 1000156031,
        G16_B16R16_2PLANE_422_UNORM = 1000156032,
        G16_B16_R16_3PLANE_444_UNORM = 1000156033,
        G8_B8R8_2PLANE_444_UNORM = 1000330000,
        G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
        G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
        G16_B16R16_2PLANE_444_UNORM = 1000330003,
        A4R4G4B4_UNORM_PACK16 = 1000340000,
        A4B4G4R4_UNORM_PACK16 = 1000340001,
        ASTC_4x4_SFLOAT_BLOCK = 1000066000,
        ASTC_5x4_SFLOAT_BLOCK = 1000066001,
        ASTC_5x5_SFLOAT_BLOCK = 1000066002,
        ASTC_6x5_SFLOAT_BLOCK = 1000066003,
        ASTC_6x6_SFLOAT_BLOCK = 1000066004,
        ASTC_8x5_SFLOAT_BLOCK = 1000066005,
        ASTC_8x6_SFLOAT_BLOCK = 1000066006,
        ASTC_8x8_SFLOAT_BLOCK = 1000066007,
        ASTC_10x5_SFLOAT_BLOCK = 1000066008,
        ASTC_10x6_SFLOAT_BLOCK = 1000066009,
        ASTC_10x8_SFLOAT_BLOCK = 1000066010,
        ASTC_10x10_SFLOAT_BLOCK = 1000066011,
        ASTC_12x10_SFLOAT_BLOCK = 1000066012,
        ASTC_12x12_SFLOAT_BLOCK = 1000066013,
        A1B5G5R5_UNORM_PACK16 = 1000470000,
        A8_UNORM = 1000470001,
        PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
        PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
        PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
        PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
        PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
        PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
        PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
        PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
        ASTC_3x3x3_UNORM_BLOCK_EXT = 1000288000,
        ASTC_3x3x3_SRGB_BLOCK_EXT = 1000288001,
        ASTC_3x3x3_SFLOAT_BLOCK_EXT = 1000288002,
        ASTC_4x3x3_UNORM_BLOCK_EXT = 1000288003,
        ASTC_4x3x3_SRGB_BLOCK_EXT = 1000288004,
        ASTC_4x3x3_SFLOAT_BLOCK_EXT = 1000288005,
        ASTC_4x4x3_UNORM_BLOCK_EXT = 1000288006,
        ASTC_4x4x3_SRGB_BLOCK_EXT = 1000288007,
        ASTC_4x4x3_SFLOAT_BLOCK_EXT = 1000288008,
        ASTC_4x4x4_UNORM_BLOCK_EXT = 1000288009,
        ASTC_4x4x4_SRGB_BLOCK_EXT = 1000288010,
        ASTC_4x4x4_SFLOAT_BLOCK_EXT = 1000288011,
        ASTC_5x4x4_UNORM_BLOCK_EXT = 1000288012,
        ASTC_5x4x4_SRGB_BLOCK_EXT = 1000288013,
        ASTC_5x4x4_SFLOAT_BLOCK_EXT = 1000288014,
        ASTC_5x5x4_UNORM_BLOCK_EXT = 1000288015,
        ASTC_5x5x4_SRGB_BLOCK_EXT = 1000288016,
        ASTC_5x5x4_SFLOAT_BLOCK_EXT = 1000288017,
        ASTC_5x5x5_UNORM_BLOCK_EXT = 1000288018,
        ASTC_5x5x5_SRGB_BLOCK_EXT = 1000288019,
        ASTC_5x5x5_SFLOAT_BLOCK_EXT = 1000288020,
        ASTC_6x5x5_UNORM_BLOCK_EXT = 1000288021,
        ASTC_6x5x5_SRGB_BLOCK_EXT = 1000288022,
        ASTC_6x5x5_SFLOAT_BLOCK_EXT = 1000288023,
        ASTC_6x6x5_UNORM_BLOCK_EXT = 1000288024,
        ASTC_6x6x5_SRGB_BLOCK_EXT = 1000288025,
        ASTC_6x6x5_SFLOAT_BLOCK_EXT = 1000288026,
        ASTC_6x6x6_UNORM_BLOCK_EXT = 1000288027,
        ASTC_6x6x6_SRGB_BLOCK_EXT = 1000288028,
        ASTC_6x6x6_SFLOAT_BLOCK_EXT = 1000288029,
        R8_BOOL_ARM = 1000460000,
        R16G16_SFIXED5_NV = 1000464000,
        R10X6_UINT_PACK16_ARM = 1000609000,
        R10X6G10X6_UINT_2PACK16_ARM = 1000609001,
        R10X6G10X6B10X6A10X6_UINT_4PACK16_ARM = 1000609002,
        R12X4_UINT_PACK16_ARM = 1000609003,
        R12X4G12X4_UINT_2PACK16_ARM = 1000609004,
        R12X4G12X4B12X4A12X4_UINT_4PACK16_ARM = 1000609005,
        R14X2_UINT_PACK16_ARM = 1000609006,
        R14X2G14X2_UINT_2PACK16_ARM = 1000609007,
        R14X2G14X2B14X2A14X2_UINT_4PACK16_ARM = 1000609008,
        R14X2_UNORM_PACK16_ARM = 1000609009,
        R14X2G14X2_UNORM_2PACK16_ARM = 1000609010,
        R14X2G14X2B14X2A14X2_UNORM_4PACK16_ARM = 1000609011,
        G14X2_B14X2R14X2_2PLANE_420_UNORM_3PACK16_ARM = 1000609012,
        G14X2_B14X2R14X2_2PLANE_422_UNORM_3PACK16_ARM = 1000609013,
    };

    enum class PipelineStage
    {
        TOP_OF_PIPE,
        TRANSFER,
        FRAGMENT_SHADER,
        BOTTOM_OF_PIPE,
        COLOR_ATTACHMENT_OUTPUT
    };

    enum class AccessFlagBits
    {
        TRANSFER_WRITE_BIT = 0x00001000,
        SHADER_READ_BIT = 0x00000020,
        COLOR_ATTACHMENT_WRITE = 0x00000100,
    };

    enum class DescriptorType
    {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        COMBINED_IMAGE_SAMPLER
    };

    enum class ShaderStageFlagBits : uint32_t
    {
        VERTEX   = 0x00000001,
        FRAGMENT = 0x00000010,
        ALL      = 0x7FFFFFFF,
    };

    enum class BufferUsageFlagBits : uint32_t
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

    struct PushConstantRange
    {
        std::size_t size = 0;
        std::size_t offset = 0;
        ShaderStageFlagBits stage = ShaderStageFlagBits::VERTEX;
    };

    struct StageInfo
    {
        std::string name;
        ShaderModuleID module;
        ShaderStageFlagBits stage;
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
        InputRate inputRate = InputRate::VERTEX;
    };

    struct VertexAttribute
    {
        size_t location = 0;
        size_t binding = 0;
        size_t offset = 0;
        Format format = Format::R32G32B32_SFLOAT;
    };

    struct PipelineInfo
    {
        PipelineLayoutID layout;
        std::span<const Format> colorAttachmentFormats;
        Format depthFormat;
        sh::ShaderInfo shaderInfo;
        std::span<VertexBinding> vertexBindings;
        std::span<VertexAttribute> vertexAttributes;
    };

    struct SwapchainInfo
    {
        bool vsync = false;
        Extent2D extent;
        SurfaceID surface;
        Format format;
    };

    struct DescriptorSetLayoutBinding
    {
        std::size_t binding = 0;
        std::size_t descriptorCount = 0;
        DescriptorType descriptorType = DescriptorType::UNIFORM_BUFFER;
        ShaderStageFlagBits stageFlags = ShaderStageFlagBits::ALL;
    };

    enum class Filter
    {
        NEAREST,
        LINEAR,
    };

    enum class AddressMode
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    };

    enum class BorderColor
    {
        TRANSPARENT_BLACK,
        OPAQUE_BLACK,
        OPAQUE_WHITE,
    };

    struct SamplerInfo
    {
        Filter filter = Filter::NEAREST;
        AddressMode addressMode = AddressMode::CLAMP_TO_EDGE;
        BorderColor borderColor = BorderColor::OPAQUE_BLACK;
    };

    struct SubmitInfo
    {
        FenceID fence;
        CommandBufferID commandBuffer;
        std::span<SemaphoreID> waitSemaphores;
        std::span<SemaphoreID> signalSemaphores;
        std::span<PipelineStage> waitDstStageMask;
    };

    struct PresentInfo
    {
        std::span<SemaphoreID> waitSemaphores;
        std::span<SwapchainID> swapchains;
        std::span<uint32_t> imageIndices;
    };

    struct BufferCopy
    {
        size_t srcOffset = 0;
        size_t dstOffset = 0;
        size_t size = 0;
    };

    enum class ImageLayout
    {
        UNDEFINED,
        GENERAL,
        COLOR_ATTACHMENT_OPTIMAL,
        DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        SHADER_READ_ONLY_OPTIMAL,
        TRANSFER_SRC_OPTIMAL,
        TRANSFER_DST_OPTIMAL,
        READ_ONLY_OPTIMAL,
        PRESENT_SRC,
    };

    enum class ImageAspect
    {
        COLOR,
        DEPTH,
        STENCIL,
    };

    struct ImageSubresource
    {
        ImageAspect aspect = ImageAspect::COLOR;
        uint32_t baseMipLevel = 0;
        uint32_t baseArrayLayer = 0;
        uint32_t layerCount = 1;
    };

    struct ImageInfo
    {
        std::size_t width = 0;
        std::size_t height = 0;
        std::size_t arrayLayers = 1;
        Format format = Format::R8G8B8A8_UNORM;
        ImageSubresource subresourceRange;
    };

    struct BufferImageCopy
    {
        size_t bufferOffset{};
        uint32_t bufferRowLength{};
        uint32_t bufferImageHeight{};
        ImageSubresource imageSubresource;
        Offset3D imageOffset{};
        Extent3D imageExtent{};
    };

    struct ImageMemoryBarrier
    {
        ImageLayout oldLayout;
        ImageLayout newLayout;
        ImageID image;
        ImageSubresource subresourceRange;
        AccessFlagBits srcAccessMask;
        AccessFlagBits dstAccessMask;
    };

    struct RenderingAttachmentInfo
    {
        ImageViewID imageView;
        ImageLayout imageLayout;
        Color clearValue;
    };

    struct RenderingInfo
    {
        std::span<RenderingAttachmentInfo> colorAttachments;
        RenderingAttachmentInfo depthAttachment;
        Extent2D extent{};
    };

    struct Rect2D
    {
        Offset2D offset;
        Extent2D extent;
    };

    enum class PipelineBindPoint
    {
        GRAPHICS,
        COMPUTE
    };

    enum class CullMode
    {
        NONE,
        FRONT,
        BACK,
    };

    struct SurfaceCapabilities
    {
        Extent2D currentExtent;
    };

    struct FormatProperties
    {

    };

    LE_DEFINE_BITMASK(AccessFlagBits);
    LE_DEFINE_BITMASK(BufferUsageFlagBits);
    LE_DEFINE_BITMASK(ShaderStageFlagBits);

    using AccessFlags = Flags<AccessFlagBits>;
    using BufferUsageFlags = Flags<BufferUsageFlagBits>;
    using ShaderStageFlags = Flags<ShaderStageFlagBits>;
}
