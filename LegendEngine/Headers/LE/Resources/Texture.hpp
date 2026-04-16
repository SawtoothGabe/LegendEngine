#pragma once

#include <LE/Resources/Resource.hpp>

namespace le
{
	class Texture : public Resource
	{
	public:
		[[nodiscard]] SamplerID GetSampler();

		[[nodiscard]] virtual ImageID GetImage() const = 0;
		[[nodiscard]] virtual ImageViewID GetImageView() const = 0;
	};
}
