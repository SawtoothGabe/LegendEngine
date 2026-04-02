#pragma once

#include <cstdint>
#include <LE/Resources/Texture.hpp>

namespace le
{
	class Texture2DArray final : public Texture
	{
	public:
		Texture2DArray(size_t width, size_t height, uint8_t channels,
			const std::span<TextureData*>& textureData);

		[[nodiscard]] size_t GetWidth() const;
		[[nodiscard]] size_t GetHeight() const;
		[[nodiscard]] uint8_t GetChannels() const;

		Image& GetImage() override;
	private:
		void Upload(GraphicsContext& context, size_t size,
			const std::span<TextureData*>& textureData) const;
		static void CopyImagesToBuffer(uint8_t* data, size_t size,
			const std::span<TextureData*>& textureData);

		size_t m_Width;
		size_t m_Height;
		uint8_t m_Channels;

		Scope<Image> m_image;
	};
}
