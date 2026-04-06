#pragma once

#include <cstdint>
#include <LE/Resources/Texture.hpp>
#include <LE/IO/TextureData.hpp>

namespace le
{
	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray();

		[[nodiscard]] size_t GetWidth() const;
		[[nodiscard]] size_t GetHeight() const;
		[[nodiscard]] uint8_t GetChannels() const;
	private:
		size_t m_Width;
		size_t m_Height;
		uint8_t m_Channels;
	};
}
