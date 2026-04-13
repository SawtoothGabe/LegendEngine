#include <LE/Graphics/Types.hpp>

namespace le
{
    BufferUsageFlagBits operator|(BufferUsageFlagBits lhs, BufferUsageFlagBits rhs)
    {
        return static_cast<BufferUsageFlagBits>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }
}