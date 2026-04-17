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
        ExplicitTexture2DArray(ExplicitResources& resources,
            size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData);
        ~ExplicitTexture2DArray();

        [[nodiscard]] ImageID GetImage() const;
        [[nodiscard]] ImageViewID GetImageView() const;
    private:
        void Upload(const std::span<TextureData*>& textureData, Extent3D extent, const QueueID& queue,
            const CommandPoolID& commandPool) const;
        static void CopyImagesToBuffer(uint8_t* data, size_t size, const std::span<TextureData*>& textureData);

        ExplicitDriver& m_driver;
        std::mutex& m_mutex;

        ImageID m_image;
        ImageViewID m_view;
    };
}
