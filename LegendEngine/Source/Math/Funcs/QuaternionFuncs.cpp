#include <LE/Math/Funcs/QuaternionFuncs.hpp>

namespace le
{
    Quaternion Math::Normalize(const Quaternion& q)
    {
        const float length = Length(q);

        // Check for divide by 0
        if (length == 0)
            return q;

        return {
            q.w / length,
            q.x / length,
            q.y / length,
            q.z / length
        };
    }

    float Math::Dot(const Quaternion& q1, const Quaternion& q2)
    {
        float product = q1.w * q2.w;
        product += q1.x * q2.x;
        product += q1.y * q2.y;
        product += q1.z * q2.z;
        return product;
    }

    float Math::Length(const Quaternion& q)
    {
        return std::sqrt(Dot(q, q));
    }

    Quaternion Math::Conjugate(const Quaternion& q)
    {
        return { q.w, -q.x, -q.y, -q.z };
    }

    Quaternion Math::Inverse(const Quaternion& q)
    {
        return Conjugate(q) / Dot(q, q);
    }
}