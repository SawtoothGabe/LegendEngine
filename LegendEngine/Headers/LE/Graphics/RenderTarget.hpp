#pragma once

#include <cstdint>

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Update() {}
        [[nodiscard]] virtual uint32_t GetWidth() const;
        [[nodiscard]] virtual uint32_t GetHeight() const;
    };
}
