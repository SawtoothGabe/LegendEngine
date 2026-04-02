#pragma once

#include <cstdint>
#include <LE/Common/Defs.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
	class TextureData;
	class Texture2D final : public Texture
	{
	public:
		explicit Texture2D(const TextureData& loader);
	    ~Texture2D() override = default;
		LE_NO_COPY(Texture2D);

		[[nodiscard]] uint64_t GetWidth() const;
		[[nodiscard]] uint64_t GetHeight() const;
		[[nodiscard]] uint8_t GetChannels() const;

		Image& GetImage() override;
	private:
		void Upload(const void* data, size_t size, Image::Format format) const;

		uint64_t m_Width;
		uint64_t m_Height;
		uint8_t m_Channels;

		GraphicsContext& m_context;

		Scope<Image> m_image;
	};
}
