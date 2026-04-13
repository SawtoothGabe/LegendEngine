#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2DArray.hpp>

namespace le
{
	Texture2DArray::Texture2DArray(size_t width, size_t height, uint8_t channels,
	                               const std::span<TextureData*>& textureData)
		:
		m_renderer(Application::Get().GetGraphicsContext().GetRenderer()),
		m_Width(width),
		m_Height(height),
		m_Channels(channels)
	{
		m_impl = m_renderer.CreateTexture2DArray(width, height, channels, textureData);
	}

	Texture2DArray::~Texture2DArray()
	{
		m_renderer.DestroyTexture2DArray(m_impl);
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

	ImageID Texture2DArray::GetImage() const
	{
		return m_renderer.GetTexture2DArrayImage(m_impl);
	}

	ImageViewID Texture2DArray::GetImageView() const
	{
		return m_renderer.GetTexture2DArrayImageView(m_impl);
	}

	Ref<Texture2DArray> Texture2DArray::Create(size_t width, size_t height, uint8_t channels,
	                                           const std::span<TextureData*>& textureData)
	{
		return std::make_shared<Texture2DArray>(width, height, channels, textureData);
	}
}
