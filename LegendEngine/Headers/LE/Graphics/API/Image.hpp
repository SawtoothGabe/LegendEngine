#pragma once

#include <cstdint>

namespace le
{
    class Image
    {
    public:
        enum class Format
        {
            R8,
            R8G8,
            R8G8B8,
            R8G8B8A8,
        };

        enum class Layout
        {
            UNDEFINED,
            GENERAL,
            COLOR_ATTACHMENT_OPTIMAL,
            SHADER_READ_ONLY_OPTIMAL,
            TRANSFER_SRC_OPTIMAL,
            TRANSFER_DST_OPTIMAL,
            READ_ONLY_OPTIMAL,
            PRESENT_SRC,
        };

        enum class Aspect
        {
            COLOR,
            DEPTH,
            STENCIL,
        };

        enum class Type
        {
            TYPE_2D,
            TYPE_2D_ARRAY,
        };

        struct Info
        {
            std::size_t width = 0;
            std::size_t height = 0;
            std::size_t arrayLayers = 1;
            Format format = Format::R8G8B8A8;
            Type type = Type::TYPE_2D;
        };

        struct Subresource
        {
            Aspect aspect;
            uint32_t mipLevel;
            uint32_t baseArrayLayer;
            uint32_t layerCount;
        };

        virtual ~Image() = default;
    };
}