#pragma once

#include <LE/Graphics/Types.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
	class Texture : public Resource
	{
	public:
		void SetSampler(SamplerID sampler);
		[[nodiscard]] SamplerID GetSampler() const;

		[[nodiscard]] virtual ImageID GetImage() const = 0;
		[[nodiscard]] virtual ImageViewID GetImageView() const = 0;
	private:
		SamplerID m_sampler;
	};
}
