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
        m_context(Application::Get().GetGraphicsContext())
    {

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
}
