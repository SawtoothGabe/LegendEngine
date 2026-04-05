#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2DArray.hpp>

namespace le
{
	Texture2DArray::Texture2DArray(const size_t width, const size_t height, const uint8_t channels,
		const std::span<TextureData*>& textureData)
		:
		m_Width(width),
		m_Height(height),
		m_Channels(channels)
	{
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
}
