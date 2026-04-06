#include <LE/Application.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2D.hpp>

namespace le
{
    Texture2D::Texture2D()
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

    Ref<Texture2D> Texture2D::Create(const TextureData& loader)
    {
        return std::make_shared<Texture2D>();
    }
}
