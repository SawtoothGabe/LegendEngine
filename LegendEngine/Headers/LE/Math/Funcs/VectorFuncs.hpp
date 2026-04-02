#pragma once

#include <LE/Math/Vector.hpp>

namespace le::Math
{
    template <typename T, size_t N>
    T Dot(const Vector<T, N>& v1, const Vector<T, N>& v2)
    {
        return v1.Dot(v2);
    }

    template <typename T, size_t N>
    T Length(const Vector<T, N>& vec)
    {
        return vec.Length();
    }

    template <typename T, size_t N>
    Vector<T, N> Normalize(const Vector<T, N>& vec)
    {
        return vec.Normalized();
    }

    template <typename T>
    Vector<T, 3> Cross(Vector<T, 3> lhs, Vector<T, 3> rhs)
    {
        return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x,
        };
    }
}
