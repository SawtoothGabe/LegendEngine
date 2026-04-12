#include <LE/Graphics/Types.hpp>

namespace le
{
    BufferUsageFlags operator|(BufferUsageFlags lhs, BufferUsageFlags rhs)
    {
        return static_cast<BufferUsageFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }
}