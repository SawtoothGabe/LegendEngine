#include <LE/Application.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2D.hpp>

namespace le
{
    Texture2D::Texture2D(const TextureData& loader)
        :
        m_Width(loader.GetWidth()),
        m_Height(loader.GetHeight()),
        m_Channels(loader.GetChannels()),
        m_resources(Application::Get().GetGraphicsContext().GetResources())
    {
        m_handle = m_resources.CreateTexture2D(loader);
    }

    Texture2D::~Texture2D()
    {
        m_resources.DestroyTexture2D(m_handle);
    }

    uint64_t Texture2D::GetWidth() const
    {
        return m_Width;
    }

    uint64_t Texture2D::GetHeight() const
    {
        return m_Height;
    }

    uint8_t Texture2D::GetChannels() const
    {
        return m_Channels;
    }

    ImageID Texture2D::GetImage() const
    {
        return m_resources.GetTexture2DImage(m_handle);
    }

    ImageViewID Texture2D::GetImageView() const
    {
        return m_resources.GetTexture2DImageView(m_handle);
    }

    Ref<Texture2D> Texture2D::Create(const TextureData& loader)
    {
        return std::make_shared<Texture2D>(loader);
    }
}
