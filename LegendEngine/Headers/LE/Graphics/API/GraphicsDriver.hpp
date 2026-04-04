#pragma once
#include <cstdint>
#include <functional>
#include <LE/Graphics/API/Types.hpp>

namespace le
{
    struct GraphicsHandle
    {
        GraphicsHandle(uint64_t id, std::function<void(uint64_t)> free)
            : id(id), m_free(free)
        {}

        ~GraphicsHandle()
        {
            m_free(id);
        }

        uint64_t id = 0;
    private:
        std::function<void(uint64_t)> m_free = nullptr;
    };

    class GraphicsDriver
    {
    public:
        virtual ~GraphicsDriver() = default;

        virtual GraphicsHandle CreatePipeline() = 0;
        virtual GraphicsHandle CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) = 0;
    };
}
