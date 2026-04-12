#pragma once
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class SmartBuffer : public Buffer
    {
    public:
        SmartBuffer(ExplicitRenderer& renderer, BufferUsageFlags usage);

        void Update(std::size_t size, std::size_t offset, const void* data) override;

        void Resize(std::size_t newSize) override;

        std::size_t GetSize() override;

        BufferID GetBuffer() override;
    };
}
