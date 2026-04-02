#pragma once

#include <cstdint>

namespace le
{
    class Buffer
    {
    public:
        enum class UsageFlags : uint32_t
        {
            TRANSFER_SRC   = 0x00000001,
            TRANSFER_DST   = 0x00000002,
            UNIFORM_BUFFER = 0x00000010,
            STORAGE_BUFFER = 0x00000020,
            INDEX_BUFFER   = 0x00000040,
            VERTEX_BUFFER  = 0x00000080,
        };

        virtual ~Buffer() = default;

        virtual void Update(std::size_t size, std::size_t offset, const void* data) = 0;
        virtual void Resize(std::size_t newSize) = 0;
        virtual std::size_t GetSize() = 0;

        virtual void* GetMappedData() { return nullptr; }
    };
}