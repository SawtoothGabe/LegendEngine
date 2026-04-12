#pragma once

#include <cwchar>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class Buffer
    {
    public:
        struct Desc
        {
            BufferID buffer;
            size_t size;
        };

        virtual ~Buffer() = default;

        virtual void Update(std::size_t size, std::size_t offset, const void* data) = 0;
        virtual void Resize(std::size_t newSize) = 0;

        virtual Desc GetDesc() const = 0;
    };
}
