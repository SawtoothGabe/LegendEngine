#pragma once

#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class SimpleBuffer : public Buffer
    {
    public:
        SimpleBuffer(const ExplicitRenderer& renderer, BufferUsageFlags usage, size_t size);
        ~SimpleBuffer() override;

        void Update(std::size_t size, std::size_t offset, const void* data) override;
        void Resize(std::size_t newSize) override;

        std::size_t GetSize() override;
        BufferID GetBuffer() override;
    private:
        ExplicitDriver& m_driver;

        BufferID m_buffer;
        BufferUsageFlags m_usage;
    };
}
