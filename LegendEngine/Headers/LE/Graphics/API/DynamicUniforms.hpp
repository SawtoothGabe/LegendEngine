#pragma once

#include <cstdint>
#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/Sampler.hpp>

namespace le
{
    class DynamicUniforms
    {
    public:
        enum class UpdateFrequency
        {
            OCCASIONAL,
            PER_FRAME,
        };

        virtual ~DynamicUniforms() = default;

        virtual void UpdateUniformBuffer(Buffer& buffer, uint32_t binding) = 0;
        virtual void UpdateStorageBuffer(Buffer& buffer, uint32_t binding) = 0;
        virtual void UpdateSampledImage(Image& image, uint32_t binding) = 0;
        virtual void UpdateSampler(Sampler& sampler, uint32_t binding) = 0;
        virtual void UpdateCombinedImageSampler(Image& image, Sampler& sampler, uint32_t binding) = 0;

        virtual void Invalidate() = 0;
    };
}
