#pragma once
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class PerFrameBuffer : public Buffer
    {
    public:
        PerFrameBuffer(ExplicitRenderer& renderer, BufferUsageFlags usage, size_t size);

        void Update(std::size_t size, std::size_t offset, const void* data) override;

        void Resize(std::size_t newSize) override;

        std::size_t GetSize() override;

        BufferID GetBuffer() override;
    };
}
