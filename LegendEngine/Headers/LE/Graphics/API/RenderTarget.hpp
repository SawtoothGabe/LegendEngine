#pragma once

#include <cstdint>

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        [[nodiscard]] virtual uint32_t GetWidth() const;
        [[nodiscard]] virtual uint32_t GetHeight() const;
    };
}
