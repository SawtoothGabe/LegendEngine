#include <LE/Graphics/Explicit/ExplicitTexture2DArray.hpp>

namespace le
{
    ExplicitTexture2DArray::ExplicitTexture2DArray(size_t width, size_t height, uint8_t channels,
        const std::span<TextureData*>& textureData)
    {

    }

    ExplicitTexture2DArray::~ExplicitTexture2DArray() {}

    void ExplicitTexture2DArray::Upload(const std::span<TextureData*>& textureData, Extent3D extent,
        const QueueID& queue, const CommandPoolID& commandPool) const {}

    void ExplicitTexture2DArray::CopyImagesToBuffer(uint8_t* data, size_t size,
        const std::span<TextureData*>& textureData) {}
}
