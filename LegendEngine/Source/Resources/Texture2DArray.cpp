#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2DArray.hpp>

namespace le
{
	Texture2DArray::Texture2DArray()
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
