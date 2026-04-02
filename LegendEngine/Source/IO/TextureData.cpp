#include <stb_image.h>
#include <LE/Common/Assert.hpp>
#include <LE/IO/TextureData.hpp>

namespace le
{
    TextureData::TextureData(const uint64_t width,
        const uint64_t height, const uint8_t channels, uint8_t* data, bool shouldFree)
    {
        m_Width = width;
        m_Height = height;
        m_Channels = channels;
        m_Data = data;
        m_ShouldFree = shouldFree;
    }

    TextureData::TextureData(TextureData&& o) noexcept
        :
        m_Width(o.m_Width),
        m_Height(o.m_Height),
        m_Channels(o.m_Channels),
        m_Data(o.m_Data),
        m_ShouldFree(o.m_ShouldFree)
    {
        o.m_Data = nullptr;
        o.m_ShouldFree = false;
    }

    TextureData::~TextureData()
    {
        if (m_ShouldFree)
            free(m_Data);
    }

    uint64_t TextureData::GetWidth() const
    {
        return m_Width;
    }

    uint64_t TextureData::GetHeight() const
    {
        return m_Height;
    }

    uint8_t TextureData::GetChannels() const
    {
        return m_Channels;
    }

    uint8_t* TextureData::GetData() const
    {
        return m_Data;
    }

    TextureData TextureData::FromData(const uint64_t width,
                               const uint64_t height, const uint8_t channels, uint8_t* data)
    {
        return TextureData(width, height, channels, data);
    }

    std::future<TextureData> TextureData::FromFile(const std::string_view path)
    {
        return std::async(std::launch::async, [=]
        {
            int width, height, channels;
            uint8_t* data = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
            LE_ASSERT(data, "Failed to load image {}", path.data());

            // I believe stb_image always structures the data for 4 channels
            channels = 4;

            return FromData(width, height, channels, data);
        });
    }
}
