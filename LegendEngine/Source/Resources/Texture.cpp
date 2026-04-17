#include <LE/Resources/Texture.hpp>

namespace le
{
    void Texture::SetSampler(const SamplerID sampler)
    {
        m_sampler = sampler;
    }

    SamplerID Texture::GetSampler() const
    {
        return m_sampler;
    }
}
