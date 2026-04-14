#pragma once

#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Resources/Texture2D.hpp>

namespace le
{
    class ExplicitTexture2D final
    {
    public:
        ExplicitTexture2D(const ExplicitRenderer& renderer, const TextureData& loader);
        ~ExplicitTexture2D();

        [[nodiscard]] ImageID GetImage() const;
        [[nodiscard]] ImageViewID GetImageView() const;
    private:
        void Upload(const void* data, size_t size, Extent3D extent, const QueueID& queue, const CommandPoolID& commandPool) const;

        ExplicitDriver& m_driver;

        ImageID m_image;
        ImageViewID m_view;
    };
}
