#pragma once

#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>

namespace le
{
    class SimpleBuffer : public Buffer
    {
    public:
        SimpleBuffer(const ExplicitResources& resources, BufferUsageFlags usage, size_t size);
        ~SimpleBuffer() override;

        void Update(std::size_t size, std::size_t offset, const void* data, size_t currentFrame) override;
        void Resize(std::size_t newSize) override;

        [[nodiscard]] Desc GetDesc(size_t currentFrame) const override;
    private:
        ExplicitDriver& m_driver;

        BufferID m_buffer;
        BufferUsageFlags m_usage;
    };
}
