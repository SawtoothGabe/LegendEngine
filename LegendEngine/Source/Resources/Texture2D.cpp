#include <LE/Application.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2D.hpp>

namespace le
{
    Texture2D::Texture2D(const TextureData& loader)
        :
        m_Width(loader.GetWidth()),
        m_Height(loader.GetHeight()),
        m_Channels(loader.GetChannels())
    {
        m_impl = Application::Get().GetGraphicsContext().GetRenderer().CreateTexture2D(loader);
    }

    Texture2D::~Texture2D()
    {
        Application::Get().GetGraphicsContext().GetRenderer().DestroyTexture2D(m_impl);
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

    Ref<Texture2D> Texture2D::Create(const TextureData& loader)
    {
        return std::make_shared<Texture2D>(loader);
    }
}
