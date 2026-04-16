#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2DArray.hpp>

namespace le
{
    Texture2DArray::Texture2DArray(const size_t width, const size_t height, const uint8_t channels,
        const std::span<TextureData*>& textureData)
        :
        m_resources(Application::Get().GetGraphicsContext().GetResources()),
        m_Width(width),
        m_Height(height),
        m_Channels(channels)
    {
        m_handle = m_resources.CreateTexture2DArray(width, height, channels, textureData);
    }

    Texture2DArray::~Texture2DArray()
    {
        m_resources.DestroyTexture2DArray(m_handle);
    }

    size_t Texture2DArray::GetWidth() const
    {
        return m_Width;
    }

    size_t Texture2DArray::GetHeight() const
    {
        return m_Height;
    }

    uint8_t Texture2DArray::GetChannels() const
    {
        return m_Channels;
    }

    ImageID Texture2DArray::GetImage() const
    {
        return m_resources.GetTexture2DArrayImage(m_handle);
    }

    ImageViewID Texture2DArray::GetImageView() const
    {
        return m_resources.GetTexture2DArrayImageView(m_handle);
    }

    Ref<Texture2DArray> Texture2DArray::Create(size_t width, size_t height, uint8_t channels,
        const std::span<TextureData*>& textureData)
    {
        return std::make_shared<Texture2DArray>(width, height, channels, textureData);
    }
}
