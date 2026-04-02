#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/API/Image.hpp>
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
		GraphicsContext& context = Application::Get().GetGraphicsContext();

		Image::Format format;
		switch (m_Channels)
		{
			case 1: format = Image::Format::R8; break;
			case 2: format = Image::Format::R8G8; break;
			case 3: format = Image::Format::R8G8B8; break;
			case 4: format = Image::Format::R8G8B8A8; break;
			default: format = Image::Format::R8; break;
		}

		const Image::Info& imageInfo = {
			.width = width,
			.height = height,
			.arrayLayers = textureData.size(),
			.format = format,
			.type = Image::Type::TYPE_2D_ARRAY,
		};

		m_image = context.CreateImage(imageInfo);

		Upload(context, m_Width * m_Height * m_Channels, textureData);
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

	void Texture2DArray::Upload(GraphicsContext& context, const size_t size, const std::span<TextureData*>& textureData) const
	{
		const Scope<Buffer> buffer = context.CreateSimpleBuffer(
			Buffer::UsageFlags::TRANSFER_SRC, size, true);

		CopyImagesToBuffer(static_cast<uint8_t*>(buffer->GetMappedData()),
			size, textureData);

		const Scope<CommandBuffer> c = context.CreateCommandBuffer(false);
		c->Begin(true);
		{
			CommandBuffer::ImageMemoryBarrier barrier{};
			barrier.oldLayout = Image::Layout::UNDEFINED;
			barrier.newLayout = Image::Layout::TRANSFER_DST_OPTIMAL;
			barrier.dstAccessMask = AccessFlagBits::TRANSFER_WRITE_BIT;
			barrier.image = m_image.get();
			barrier.subresourceRange.aspect = Image::Aspect::COLOR;
			barrier.subresourceRange.layerCount = textureData.size();

			c->CmdPipelineBarrier(PipelineStage::TOP_OF_PIPE,
				PipelineStage::TRANSFER, 1, &barrier);

			std::vector<CommandBuffer::BufferImageCopy> copies(textureData.size());
			for (size_t i = 0; i < copies.size(); i++)
			{
				copies[i].bufferOffset = i * size;
				copies[i].imageSubresource.aspect = Image::Aspect::COLOR;
				copies[i].imageSubresource.baseArrayLayer = i;
				copies[i].imageSubresource.layerCount = 1;
				copies[i].imageOffset = { 0, 0, 0 };
				copies[i].imageExtent = {
					static_cast<uint32_t>(m_Width),
					static_cast<uint32_t>(m_Height),
					1
				};
			}

			c->CmdCopyBufferToImage(*buffer, *m_image,
				Image::Layout::TRANSFER_DST_OPTIMAL, copies.size(),
				copies.data());

			barrier.oldLayout = Image::Layout::TRANSFER_DST_OPTIMAL;
			barrier.newLayout = Image::Layout::SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = AccessFlagBits::TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = AccessFlagBits::SHADER_READ_BIT;

			c->CmdPipelineBarrier(PipelineStage::TRANSFER,
				PipelineStage::FRAGMENT_SHADER, 1, &barrier);
		}
		c->End();
		c->Submit(true);
	}

	void Texture2DArray::CopyImagesToBuffer(uint8_t* data, const size_t size, const std::span<TextureData*>& textureData)
	{
		for (size_t i = 0; i < textureData.size(); i++)
		{
			const TextureData* texture = textureData[i];
			const size_t textureSize = texture->GetWidth() * texture->GetHeight()
				* texture->GetChannels();

			LE_ASSERT(textureSize == size, "All textures given to Texture2DArray must be the same size");

			memcpy(data + i * size, texture->GetData(), size);
		}
	}

	Image& Texture2DArray::GetImage()
	{
		return *m_image;
	}
}
