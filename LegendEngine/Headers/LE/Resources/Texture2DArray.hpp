#pragma once

#include <cstdint>
#include <LE/Resources/Texture.hpp>

namespace le
{
	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(size_t width, size_t height, uint8_t channels,
			const std::span<TextureData*>& textureData);
		~Texture2DArray();

		[[nodiscard]] size_t GetWidth() const;
		[[nodiscard]] size_t GetHeight() const;
		[[nodiscard]] uint8_t GetChannels() const;

		static Ref<Texture2DArray> Create(size_t width, size_t height, uint8_t channels,
			const std::span<TextureData*>& textureData);
	private:
		size_t m_Width;
		size_t m_Height;
		uint8_t m_Channels;
	};
}
