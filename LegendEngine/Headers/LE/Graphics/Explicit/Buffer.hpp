#pragma once

#include <cwchar>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        virtual void Update(std::size_t size, std::size_t offset, const void* data) = 0;
        virtual void Resize(std::size_t newSize) = 0;
        virtual std::size_t GetSize() = 0;

        virtual BufferID GetBuffer() = 0;
        virtual void* GetMappedData() { return nullptr; }
    };
}
