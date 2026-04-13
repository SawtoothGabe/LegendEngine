#pragma once

#include <LE/Graphics/Types.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/IO/TextureData.hpp>

namespace le
{
    class ExplicitTexture2DArray final
    {
    public:
        ExplicitTexture2DArray(const ExplicitRenderer& renderer,
            size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData);
        ~ExplicitTexture2DArray();
    private:
        void Upload(const std::span<TextureData*>& textureData, Extent3D extent, const QueueID& queue,
            const CommandPoolID& commandPool) const;
        static void CopyImagesToBuffer(uint8_t* data, size_t size, const std::span<TextureData*>& textureData);

        ExplicitDriver& m_driver;

        ImageID m_image;
        ImageViewID m_view;
    };
}
