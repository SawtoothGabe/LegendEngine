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
        m_renderer(Application::Get().GetGraphicsContext().GetRenderer())
    {
        m_impl = m_renderer.CreateTexture2D(loader);
    }

    Texture2D::~Texture2D()
    {
        m_renderer.DestroyTexture2D(m_impl);
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
        return m_renderer.GetTexture2DImage(m_impl);
    }

    ImageViewID Texture2D::GetImageView() const
    {
        return m_renderer.GetTexture2DImageView(m_impl);
    }

    Ref<Texture2D> Texture2D::Create(const TextureData& loader)
    {
        return std::make_shared<Texture2D>(loader);
    }
}
