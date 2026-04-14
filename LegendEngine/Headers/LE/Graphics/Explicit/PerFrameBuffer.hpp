#pragma once

#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class PerFrameBuffer : public Buffer
    {
    public:
        PerFrameBuffer(const ExplicitRenderer& renderer, BufferUsageFlagBits usage, size_t size);
        ~PerFrameBuffer() override;

        void Update(std::size_t size, std::size_t offset, const void* data, size_t currentFrame) override;
        void Resize(std::size_t newSize) override;

        [[nodiscard]] Desc GetDesc(size_t currentFrame) const override;
    private:
        void CreateBuffer(BufferID& buffer) const;
        void RecreateBuffer(size_t currentFrame);

        ExplicitDriver& m_driver;

        size_t m_size = 0;

        std::vector<BufferID> m_buffers;
        std::vector<size_t> m_sizes;

        BufferUsageFlagBits m_usage;
        bool m_mapped = false;
    };
}
